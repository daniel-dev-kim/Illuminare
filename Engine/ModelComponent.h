// MeshComponent.h
#pragma once
#include "Component.h"

class Model;
class Mesh;
class Material;
class Renderer;
class ModelComponent : public Component {
    std::shared_ptr<Material> overrideMaterial;
    std::shared_ptr<Model> model;

public:

    explicit ModelComponent(const std::shared_ptr<Model>& model)
        : model(model) {
    }

    explicit ModelComponent(const std::shared_ptr<Model>& model, const std::shared_ptr<Material>& mat)
        : model(model), overrideMaterial(mat) {
    }
    void Update(float deltaTime) override {}

    int GetGPUMeshID(Renderer* renderer, const std::shared_ptr<Mesh>& mesh);

    std::shared_ptr<Model> GetModel() const;
    std::shared_ptr<Material> GetOverrideMaterial() const;
private:
    std::unordered_map<std::shared_ptr<Mesh>, int> gpuMeshIDs;
};