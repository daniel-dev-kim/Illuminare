// RenderSystem.h
#pragma once
#include "RenderProxy.h"
class Renderer;
class RenderSystem {
public:
    RenderSystem(std::unique_ptr<Renderer> renderer);
    Renderer* GetRenderer() { return renderer.get(); }

    bool Initialize(int width, int height, const std::string& title);

    glm::ivec2 GetWindowSize() { return { windowSize }; }
    void SetWindowSize(int width, int height);

    bool ShouldClose();
    void MainThreadEventsBegin();
    void RenderLoop();
    void MainThreadEventsEnd();

    void Shutdown();
   
    void StartRenderThread();
    void StopRenderThread();
protected:
    glm::ivec2 windowSize;

    std::unique_ptr<Renderer> renderer;
    std::vector<RenderProxy> currentFrameProxies;

    std::mutex renderMutex;
    std::thread renderThread;
    std::atomic<bool> running = false;
};