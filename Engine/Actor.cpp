#include "pch.h"
#include "Actor.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "Renderer.h"
#include "Model.h"

Actor::Actor() {}

void Actor::Update(float deltaTime) {
    for (auto& comp : components)
        comp->Update(deltaTime);
}

std::vector<RenderProxy> Actor::ExtractRenderProxies(Renderer* renderer) {
    std::vector<RenderProxy> proxies;

    auto transform = GetComponent<TransformComponent>();
    if (!transform) return proxies;

    auto modelComp = GetComponent<ModelComponent>();
    if (!modelComp || !modelComp->GetModel()) return proxies;

    // 루트 노드부터 재귀적으로 순회
    ProcessNodeForRenderProxy(renderer, modelComp->GetModel()->rootNode, transform->GetMatrix(), modelComp, proxies);
    return proxies;
}

void Actor::ProcessNodeForRenderProxy(Renderer* renderer,
    const std::shared_ptr<Node>& node,
    const glm::mat4& parentTransform,
    ModelComponent* modelComp,
    std::vector<RenderProxy>& proxies)
{
    glm::mat4 globalTransform = parentTransform * node->transform;

    for (auto& [mesh, material] : node->meshMaterials) {
        RenderProxy proxy;
        proxy.meshID = modelComp->GetGPUMeshID(renderer, mesh);
        proxy.material = material ? material : modelComp->GetOverrideMaterial();
        proxy.modelMatrix = globalTransform;
        proxies.push_back(proxy);
    }

    for (auto& child : node->children) {
        ProcessNodeForRenderProxy(renderer, child, globalTransform, modelComp, proxies);
    }
}