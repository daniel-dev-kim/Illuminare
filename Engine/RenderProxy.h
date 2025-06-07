#pragma once
class Material;

struct RenderProxy {
    int meshID; // Renderer가 제공한 ID
    glm::mat4 modelMatrix;
    std::shared_ptr<Material> material;
};