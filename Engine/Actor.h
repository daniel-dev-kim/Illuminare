#pragma once
#include "RenderProxy.h"
class Renderer;
class Node;
class Component;
class ModelComponent;

class Actor {
public:
    Actor();

    void Update(float deltaTime);

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->owner = this;
        T* raw = comp.get();
        components.push_back(std::move(comp));
        componentMap[typeid(T)] = raw;
        return raw;
    }

    template<typename T>
    T* GetComponent() {
        auto it = componentMap.find(typeid(T));
        if (it != componentMap.end()) return static_cast<T*>(it->second);
        return nullptr;
    }

    std::vector<std::unique_ptr<Component>>& GetComponents() { return components; }

    std::vector<RenderProxy> ExtractRenderProxies(Renderer* renderer);

private:
    std::vector<std::unique_ptr<Component>> components;
    std::unordered_map<std::type_index, Component*> componentMap;

    void ProcessNodeForRenderProxy(Renderer* renderer,
        const std::shared_ptr<Node>& node,
        const glm::mat4& parentTransform,
        ModelComponent* modelComp,
        std::vector<RenderProxy>& proxies);
};