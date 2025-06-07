#pragma once
#include "RenderProxy.h"
class World;
class Renderer;
class WorldManager
{
public:
    void AddWorld(const std::string& name, std::shared_ptr<World> world);
    void RemoveWorld(const std::string& name);
    void SetActiveWorld(const std::string& name);

    void Update(float deltaTime);
    std::vector<RenderProxy> ExtractRenderProxies(Renderer* renderer);

    std::shared_ptr<World> GetActiveWorld();

private:
    std::unordered_map<std::string, std::shared_ptr<World>> worlds;
    std::string activeWorldName;
};