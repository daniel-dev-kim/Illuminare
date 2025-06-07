#include "pch.h"
#include "WorldManager.h"
#include "World.h"

void WorldManager::AddWorld(const std::string& name, std::shared_ptr<World> world) {
    worlds[name] = world;
}

void WorldManager::RemoveWorld(const std::string& name) {
    worlds.erase(name);
    if (activeWorldName == name)
        activeWorldName.clear();
}

void WorldManager::SetActiveWorld(const std::string& name) {
    if (worlds.find(name) == worlds.end())
        throw std::runtime_error("World not found: " + name);
    activeWorldName = name;
}

std::shared_ptr<World> WorldManager::GetActiveWorld() {
    if (activeWorldName.empty()) return nullptr;
    return worlds.at(activeWorldName);
}

void WorldManager::Update(float deltaTime) {
    auto world = GetActiveWorld();
    if (world) world->Update(deltaTime);
}

std::vector<RenderProxy> WorldManager::ExtractRenderProxies(Renderer* renderer) {
    auto world = GetActiveWorld();
    if (world) return world->ExtractRenderProxies(renderer);
    return {};
}
