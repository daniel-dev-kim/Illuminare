// World.h
#pragma once
#include "RenderProxy.h"

class Actor;
class Renderer;

class World {
public:
    void Update(float deltaTime);
    void AddActor(std::unique_ptr<Actor> actor);

    std::vector<RenderProxy> ExtractRenderProxies(Renderer* renderer);
    void SetGlobalShaderMatrices() const;
    void CreateTestScene();

private:
    Actor* GetCurrentCamera() const;
    std::vector<std::shared_ptr<Actor>> actors;
};
