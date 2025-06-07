#include "pch.h"
#include "Engine.h"
#include "InputSystem.h"
#include "RenderSystem.h"
#include "OpenGLRenderer.h"
#include "OpenGLShader.h"
#include "Material.h"
#include "Mesh.h"

bool OpenGLRenderer::Initialize(int width, int height, const std::string& title)
{
    if (!glfwInit()) {
        std::cerr << "[Error] GLFW initialization failed.\n";
        return false;
    }

    // OpenGL context version 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    
    // 윈도우 생성 및 컨텍스트 생성
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "[Error] Window creation failed.\n";
        return false;
    }

    // 메인 스레드에서 context 잠깐 활성화
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[Error] Failed to initialize GLAD\n";
        return false;
    }

    // ImGui 초기화
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // 콜백 세팅
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetKeyCallback(window, KeyboardCallback);
    glfwSetCursorPosCallback(window, MouseCursorCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);

    return true;
}

void OpenGLRenderer::MultithreadingSetup()
{
    glfwMakeContextCurrent(NULL);
}

bool OpenGLRenderer::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void OpenGLRenderer::MainThreadEventsBegin()
{
    glfwPollEvents();
}

void OpenGLRenderer::LoopSetup()
{
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 1280, 720); // 필수!

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[Error] Failed to initialize GLAD\n";
    }

    InitializeShadowMap(); // 👈 바로 여기 추가!
    shadowShader = CreateShader("Shader/shadow_default.vert", "Shader/shadow_default.frag");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f); // 밝은 하늘색
}

void OpenGLRenderer::BeginShadowPass()
{
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::EndShadowPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1280, 720); // 원래 창 크기로 복구
}

void OpenGLRenderer::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void OpenGLRenderer::MainThreadEventsEnd() {}

void OpenGLRenderer::Shutdown()
{
    glfwMakeContextCurrent(NULL);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

int OpenGLRenderer::UploadMesh(std::shared_ptr<Mesh> mesh)
{
    GPUMeshData data;
    glGenVertexArrays(1, &data.vao);
    glGenBuffers(1, &data.vbo);

    glBindVertexArray(data.vao);
    glBindBuffer(GL_ARRAY_BUFFER, data.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), mesh->vertices.data(), GL_STATIC_DRAW);

    // 정점 속성 설정
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    data.vertexCount = mesh->vertices.size();

    if (!mesh->indices.empty()) {
        data.hasIndex = true;
        data.indexCount = mesh->indices.size();

        glGenBuffers(1, &data.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indexCount * sizeof(uint32_t), mesh->indices.data(), GL_STATIC_DRAW);
    }
    data.drawMode = mesh->drawMode; // 핵심!

    int meshID = nextMeshID++;
    gpuMeshes[meshID] = data;

    return meshID;
}

std::shared_ptr<Shader> OpenGLRenderer::CreateShader(const std::string& vertPath, const std::string& fragPath) {
    
    std::string key = vertPath + "+" + fragPath; // 유일한 키로 만듦

    auto it = shaderCache.find(key);
    if (it != shaderCache.end())
    {
        return it->second; // 이미 로드된 셰이더 반환
    }

    // 파일 로딩
    std::string vertCode = LoadFile(vertPath);
    std::string fragCode = LoadFile(fragPath);

    // 컴파일
    GLuint vertexShader = CompileShader(vertCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragCode, GL_FRAGMENT_SHADER);

    // 프로그램 생성 및 링크
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // 링크 상태 확인
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        throw std::runtime_error("Shader linking error:\n" + std::string(infoLog));
    }

    // 이제 Shader 객체를 생성
    auto shader = std::make_shared<OpenGLShader>(program);

    // 컴파일된 셰이더는 더 이상 필요 없으므로 삭제
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

GLuint OpenGLRenderer::LoadTexture(const std::string& path)
{
    // 이미 로드되어 있으면 바로 반환
    if (textureCache.count(path))
        return textureCache[path];

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data)
    {
        std::cerr << "[Error] Failed to load texture: " << path << std::endl;
        return 0;
    }

    GLenum format = GL_RGB;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 텍스처 기본 파라미터 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    textureCache[path] = textureID;
    return textureID;
}

std::string OpenGLRenderer::LoadFile(const std::string& path)
{
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint OpenGLRenderer::CompileShader(const std::string& source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::string typeName = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        throw std::runtime_error(typeName + " shader compilation error:\n" + infoLog);
    }

    return shader;
}

void OpenGLRenderer::InitializeShadowMap()
{
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

    // 1. FBO 생성
    glGenFramebuffers(1, &shadowFBO);

    // 2. 깊이 텍스처 생성
    glGenTextures(1, &shadowDepthTexture);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // 테두리 색을 설정해줘야 테두리 문제를 피할 수 있어
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 3. FBO 설정
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTexture, 0);
    glDrawBuffer(GL_NONE); // 색 버퍼 안 씀
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 4. Light Space Matrix 계산
    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.0f, 30.0f);
    glm::mat4 lightView = glm::lookAt(-lightDir * 10.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
}

void OpenGLRenderer::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Engine::GetInstance().GetRenderSystem().SetWindowSize(width, height);
}

void OpenGLRenderer::KeyboardCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
    Engine::GetInstance().GetInputSystem().FeedKeyEvent(key, action);
}

void OpenGLRenderer::MouseButtonCallback(GLFWwindow*, int button, int action, int mods)
{
    Engine::GetInstance().GetInputSystem().FeedMouseButton(button, action);
}

void OpenGLRenderer::MouseScrollCallback(GLFWwindow*, double xOffset, double yOffset)
{
    Engine::GetInstance().GetInputSystem().FeedMouseScroll(xOffset, yOffset);
}

void OpenGLRenderer::MouseCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    Engine::GetInstance().GetInputSystem().FeedMouseMove(xpos, ypos);
}

void OpenGLRenderer::DrawMesh(int meshID, const glm::mat4& modelMatrix, Material* material)
{

    const auto& data = gpuMeshes.at(meshID);
    glBindVertexArray(data.vao);

    
    if (!material)
    {

        shadowShader->Use();
        shadowShader->SetUniform("lightViewProj", lightSpaceMatrix);
        shadowShader->SetUniform("model", modelMatrix);
    }
    else
    {
        material->Use();
        material->SetMat4("model", modelMatrix);
        material->SetMat4("lightViewProj", lightSpaceMatrix);
        material->SetTexture("shadowMap", shadowDepthTexture);

    }


    if (data.hasIndex) {
        glDrawElements(data.drawMode, static_cast<GLsizei>(data.indexCount), GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(data.drawMode, 0, static_cast<GLsizei>(data.vertexCount));
    }
}
