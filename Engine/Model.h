#pragma once
class Mesh;
class Material;
class Node {
public:
    glm::mat4 transform{ 1.0f };
    std::vector<std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>>> meshMaterials;
    std::vector<std::shared_ptr<Node>> children;
};

class Model {
public:
    std::shared_ptr<Node> rootNode = std::make_shared<Node>();

    bool LoadFromOBJ(const std::string& filepath);
    //bool LoadFromGLTF(const std::string& filepath);
    bool LoadFromFBX(const std::string& filepath);

private:
    void ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<Node> parentNode, const std::vector<std::shared_ptr<Material>>& loadedMaterials);
    std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh);
};