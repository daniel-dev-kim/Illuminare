#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Engine.h"
#include "RenderSystem.h"
#include "Renderer.h"
void DebugPrintMeshAsOBJFormat(const std::shared_ptr<Mesh>& mesh) {
    const auto& vertices = mesh->vertices;
    const auto& indices = mesh->indices;

    std::cout << "### Mesh Dump (OBJ-like format)\n";

    // 1. 정점 정보
    for (const auto& v : vertices) {
        std::cout << "v " << v.position.x << " " << v.position.y << " " << v.position.z << "\n";
    }

    // 2. 텍스처 좌표
    for (const auto& v : vertices) {
        std::cout << "vt " << v.uv.x << " " << v.uv.y << "\n";
    }

    // 3. 노멀 정보
    for (const auto& v : vertices) {
        std::cout << "vn " << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";
    }

    // 4. 인덱스 (삼각형)
    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        // OBJ 인덱스는 1부터 시작
        int i1 = indices[i] + 1;
        int i2 = indices[i + 1] + 1;
        int i3 = indices[i + 2] + 1;

        std::cout << "f "
            << i1 << "/" << i1 << "/" << i1 << " "
            << i2 << "/" << i2 << "/" << i2 << " "
            << i3 << "/" << i3 << "/" << i3 << "\n";
    }

    std::cout << "### End Mesh Dump\n";
}

bool Model::LoadFromOBJ(const std::string& filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }

    std::vector<std::shared_ptr<Material>> loadedMaterials;
    if (scene->HasMaterials()) {

        auto renderer = Engine::GetInstance().GetRenderSystem().GetRenderer();
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial* aiMat = scene->mMaterials[i];

            // 기본 셰이더 사용
            static std::shared_ptr<Shader> basicShader = renderer->CreateShader(
                "Shader/default.vert", "Shader/default.frag");

            auto material = std::make_shared<Material>(basicShader);
            // Albedo Color (Kd)
            aiColor3D kd(1.0f, 1.0f, 1.0f);
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, kd)) {
                material->SetVec3("albedoColor", glm::vec3(kd.r, kd.g, kd.b));
            }
            else {
                material->SetVec3("albedoColor", glm::vec3(1.0f, 1.0f, 1.0f));
            }

            // Ambient Color (Ka)
            aiColor3D ka(1.0f, 1.0f, 1.0f);
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ka)) {
                material->SetVec3("ambientColor", glm::vec3(ka.r, ka.g, ka.b));
            }
            else {
                material->SetVec3("ambientColor", glm::vec3(1.0f, 1.0f, 1.0f));
            }

            // Specular Color (Ks)
            aiColor3D ks(1.0f, 1.0f, 1.0f);
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_SPECULAR, ks)) {
                material->SetVec3("specularColor", glm::vec3(ks.r, ks.g, ks.b));
            }
            else {
                material->SetVec3("specularColor", glm::vec3(1.0f, 1.0f, 1.0f));
            }

            // Emissive Color (Ke)
            aiColor3D ke(0.0f, 0.0f, 0.0f);
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, ke)) {
                material->SetVec3("emissiveColor", glm::vec3(ke.r, ke.g, ke.b));
            }
            else {
                material->SetVec3("emissiveColor", glm::vec3(0.0f, 0.0f, 0.0f));
            }

            // Shininess -> Roughness
            float shininess = 32.0f;
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_SHININESS, shininess)) {
                float roughness = glm::clamp(1.0f - (shininess / 128.0f), 0.05f, 1.0f);
                material->SetFloat("roughness", roughness);
            }
            else {
                material->SetFloat("roughness", 0.5f);
            }

            // Transparency (d)
            float opacity = 1.0f;
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_OPACITY, opacity)) {
                material->SetFloat("opacity", opacity);
            }
            else {
                material->SetFloat("opacity", 1.0f);
            }

            // Diffuse (Albedo)
            if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString texPath;
                aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
                GLuint texID = renderer->LoadTexture(texPath.C_Str());
                material->SetTexture("albedoMap", texID);
                material->SetInt("hasAlbedoMap", true);
            }
            else
            {
                material->SetInt("hasAlbedoMap", false);
            }

            // Specular (Roughness)
            if (aiMat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
                aiString texPath;
                aiMat->GetTexture(aiTextureType_SPECULAR, 0, &texPath);
                GLuint texID = renderer->LoadTexture(texPath.C_Str());
                material->SetTexture("roughnessMap", texID);
                material->SetInt("hasRoughnessMap", true);
            }
            else
            {
                material->SetInt("hasRoughnessMap", false);
            }

            // Normal map (map_bump)
            if (aiMat->GetTextureCount(aiTextureType_HEIGHT) > 0) { // Assimp는 NormalMap을 HEIGHT로 파싱하기도 해
                aiString texPath;
                aiMat->GetTexture(aiTextureType_HEIGHT, 0, &texPath);
                GLuint texID = renderer->LoadTexture(texPath.C_Str());
                material->SetTexture("normalMap", texID);
                material->SetInt("hasNormalMap", true);
            }
            else
            {
                material->SetInt("hasNormalMap", false);
            }

            // (선택) Bump map (bump)
            if (aiMat->GetTextureCount(aiTextureType_DISPLACEMENT) > 0) {
                aiString texPath;
                aiMat->GetTexture(aiTextureType_DISPLACEMENT, 0, &texPath);
                GLuint texID = renderer->LoadTexture(texPath.C_Str());
                material->SetTexture("bumpMap", texID);
            }

            loadedMaterials.push_back(material);
        }
    }
    //loadedMaterials.erase(loadedMaterials.begin());
    // (2) 메쉬와 머티리얼 매칭
    rootNode = std::make_shared<Node>();
    ProcessNode(scene->mRootNode, scene, rootNode, loadedMaterials);
    return true;
}

//bool Model::LoadFromGLTF(const std::string& filepath) {
//    Assimp::Importer importer;
//    const aiScene* scene = importer.ReadFile(filepath,
//        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
//
//    if (!scene || !scene->HasMeshes()) {
//        throw std::runtime_error("Failed to load glTF: " + filepath);
//        return false;
//    }
//
//    rootNode = std::make_shared<Node>();
//    ProcessNode(scene->mRootNode, scene, rootNode);
//    return true;
//}
//

bool Model::LoadFromFBX(const std::string& filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error("Failed to load FBX: " + filepath);
        return false;
    }

    // (1) Material 로드
    std::vector<std::shared_ptr<Material>> loadedMaterials;
    if (scene->HasMaterials()) {
        auto renderer = Engine::GetInstance().GetRenderSystem().GetRenderer();
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial* aiMat = scene->mMaterials[i];

            // 기본 셰이더 사용
            static std::shared_ptr<Shader> basicShader = renderer->CreateShader(
                "Shader/default.vert", "Shader/default.frag");

            auto material = std::make_shared<Material>(basicShader);

            aiColor3D color(1.0f, 1.0f, 1.0f);
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
                material->SetVec3("albedoColor", glm::vec3(color.r, color.g, color.b));
            }
            else {
                material->SetVec3("albedoColor", glm::vec3(1.0f, 1.0f, 1.0f)); // 기본값
            }

            float shininess = 32.0f;
            if (AI_SUCCESS == aiMat->Get(AI_MATKEY_SHININESS, shininess)) {
                float roughness = glm::clamp(1.0f - (shininess / 128.0f), 0.05f, 1.0f);
                material->SetFloat("roughness", roughness);
            }
            else {
                material->SetFloat("roughness", 0.5f); // 기본 roughness
            }

            // Diffuse 텍스쳐 로드
            if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString texPath;
                aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);

                GLuint texID = renderer->LoadTexture(texPath.C_Str());
                material->SetTexture("albedoMap", texID);
            }

            loadedMaterials.push_back(material);
        }
    }

    // (2) Node와 Mesh 로드 (머티리얼 적용)
    rootNode = std::make_shared<Node>();
    ProcessNode(scene->mRootNode, scene, rootNode, loadedMaterials);

    return true;
}

void Model::ProcessNode(aiNode* ainode, const aiScene* scene, std::shared_ptr<Node> parentNode, const std::vector<std::shared_ptr<Material>>& loadedMaterials) {
    // Assimp 노드의 변형(transform) 처리
    aiMatrix4x4 mat = ainode->mTransformation;
    parentNode->transform = glm::transpose(glm::make_mat4(&mat.a1));

    // Mesh 처리
    for (unsigned int i = 0; i < ainode->mNumMeshes; ++i) {
        aiMesh* aimesh = scene->mMeshes[ainode->mMeshes[i]];
        //std::cout << "Mesh Name: " << aimesh->mName.C_Str()
        //    << ", Material Index: " << aimesh->mMaterialIndex << std::endl;
        auto mesh = ProcessMesh(aimesh);
        std::shared_ptr<Material> material = nullptr;
        if (aimesh->mMaterialIndex < loadedMaterials.size()) {
            material = loadedMaterials[aimesh->mMaterialIndex];
        }
        parentNode->meshMaterials.emplace_back(mesh, material);
    }

    // 자식 노드 처리 (재귀)
    for (unsigned int i = 0; i < ainode->mNumChildren; ++i) {
        auto childNode = std::make_shared<Node>();
        parentNode->children.push_back(childNode);
        ProcessNode(ainode->mChildren[i], scene, childNode, loadedMaterials); // 인자 주의!
    }
}

std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh* aimesh) {
    auto mesh = std::make_shared<Mesh>();

    // Vertex 처리
    for (unsigned int v = 0; v < aimesh->mNumVertices; ++v) {
        Vertex vertex;
        vertex.position = {
            aimesh->mVertices[v].x,
            aimesh->mVertices[v].y,
            aimesh->mVertices[v].z
        };

        vertex.normal = aimesh->HasNormals() ? glm::vec3{
            aimesh->mNormals[v].x,
            aimesh->mNormals[v].y,
            aimesh->mNormals[v].z
        } : glm::vec3{ 0.f };

        if (aimesh->mTextureCoords[0]) {
            vertex.uv = {
                aimesh->mTextureCoords[0][v].x,
                aimesh->mTextureCoords[0][v].y
            };
        }
        else {
            vertex.uv = { 0.f, 0.f };
        }

        mesh->vertices.push_back(vertex);
    }

    // 인덱스 처리
    for (unsigned int f = 0; f < aimesh->mNumFaces; ++f) {
        const aiFace& face = aimesh->mFaces[f];
        if (face.mNumIndices != 3) continue;
        mesh->indices.push_back(face.mIndices[0]);
        mesh->indices.push_back(face.mIndices[1]);
        mesh->indices.push_back(face.mIndices[2]);
    }

    return mesh;
}