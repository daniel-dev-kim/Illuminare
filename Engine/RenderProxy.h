#pragma once
class Material;

struct RenderProxy {
    int meshID; // Renderer�� ������ ID
    glm::mat4 modelMatrix;
    std::shared_ptr<Material> material;
};