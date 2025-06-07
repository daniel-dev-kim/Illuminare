#include "pch.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "Model.h"
#include "Material.h"
#include "Renderer.h"

int ModelComponent::GetGPUMeshID(Renderer* renderer, const std::shared_ptr<Mesh>& mesh) {
    auto it = gpuMeshIDs.find(mesh);
    if (it != gpuMeshIDs.end()) {
        return it->second;
    }

    int newGPUID = renderer->UploadMesh(mesh);
    gpuMeshIDs[mesh] = newGPUID;

    return newGPUID;
}

std::shared_ptr<Model> ModelComponent::GetModel() const
{
    return model;
}

std::shared_ptr<Material> ModelComponent::GetOverrideMaterial() const {
    return overrideMaterial;
}
