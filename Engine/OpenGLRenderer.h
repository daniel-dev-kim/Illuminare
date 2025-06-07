#pragma once
#include "Renderer.h"
struct GPUMeshData {
    GLuint vao;
    GLuint vbo;
    GLuint ebo; // 필요 시
    size_t vertexCount = 0;
    size_t indexCount = 0;
    bool hasIndex = false;
    GLenum drawMode = GL_TRIANGLES; // 추가: 어떤 도형으로 그릴지

};

class OpenGLRenderer : public Renderer {
public:
    //Init
    virtual bool Initialize(int width, int height, const std::string& title);
    virtual void MultithreadingSetup();
    //Loop
    virtual void MainThreadEventsBegin();
    virtual void LoopSetup();

    virtual void BeginShadowPass();
    virtual void EndShadowPass();

    virtual void BeginFrame();
    virtual void EndFrame();
    virtual void MainThreadEventsEnd();
    //Shutdown
    virtual bool ShouldClose();
    virtual void Shutdown();

public:
    int UploadMesh(std::shared_ptr<Mesh> mesh) override;
    std::shared_ptr<Shader> CreateShader(const std::string& vertPath, const std::string& fragPath) override;
    GLuint LoadTexture(const std::string& path) override;
    void DrawMesh(int meshID, const glm::mat4& modelMatrix, Material* material) override;
    GLFWwindow* GetWindow() { return window; }
private:
    std::string LoadFile(const std::string& path);
    GLuint CompileShader(const std::string& source, GLenum shaderType);
    void InitializeShadowMap();
private:
    static void WindowSizeCallback(GLFWwindow* window, int width, int height);

    static void MouseButtonCallback(GLFWwindow*, int key, int action, int mods);
    static void MouseScrollCallback(GLFWwindow*, double xOffset, double yOffset);
    static void KeyboardCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    static void MouseCursorCallback(GLFWwindow* window, double xpos, double ypos);
private:
    GLFWwindow* window = nullptr;
private:
    int nextMeshID = 0;
    std::unordered_map<int, GPUMeshData> gpuMeshes; // Mesh ID로 관리
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaderCache; // Shader Filepath로 관리
    std::unordered_map<std::string, GLuint> textureCache; // Textuer Filepath로 관리
private:
    GLuint shadowFBO = 0;
    GLuint shadowDepthTexture = 0;
    glm::mat4 lightSpaceMatrix;
    std::shared_ptr<Shader> shadowShader;
};