#include "pch.h"
#include "RenderSystem.h"
#include "Renderer.h"
#include "Engine.h"
#include "WorldManager.h"
#include "World.h"
RenderSystem::RenderSystem(std::unique_ptr<Renderer> renderer) : renderer(std::move(renderer)) {}

bool RenderSystem::Initialize(int width, int height, const std::string& title) {
    SetWindowSize(width, height);

    return renderer->Initialize(windowSize.x = width, windowSize.y = height, title);
}

void RenderSystem::SetWindowSize(int width, int height)
{
    windowSize = {width, height};
}

bool RenderSystem::ShouldClose() {
    return renderer->ShouldClose();
}

void RenderSystem::MainThreadEventsBegin()
{
    renderer->MainThreadEventsBegin();
}



void RenderSystem::MainThreadEventsEnd() {
    renderer->MainThreadEventsEnd();
}

void RenderSystem::StartRenderThread() {
    running = true;
    renderThread = std::thread(&RenderSystem::RenderLoop, this);
}

void RenderSystem::StopRenderThread() {
    running = false;
    if (renderThread.joinable())
        renderThread.join();
}

void RenderSystem::RenderLoop() {
    renderer->LoopSetup(); // 각종 셰이더 로드
    auto world = Engine::GetInstance().GetWorldManager().GetActiveWorld();
    while (running && !ShouldClose()) {
        // Shadow Pass
        renderer->BeginShadowPass(); // 프레임 버퍼 초기화 바인딩
        if (world) {
            auto proxies = world->ExtractRenderProxies(renderer.get());
            for (auto& proxy : proxies) {
                // Shadow Pass의 머터리얼은 지정된 그림자 생성용 셰이더이므로 nullptr 입력
                renderer->DrawMesh(proxy.meshID, proxy.modelMatrix, nullptr);
            }
        }
        renderer->EndShadowPass(); // 프레임 버퍼 해제
        
        // Rendering Pass
        renderer->BeginFrame(); // ImGui 시작 및 컬러 비트 초기화
        if (world) {
            world->SetGlobalShaderMatrices();
            auto proxies = world->ExtractRenderProxies(renderer.get());
            for (auto& proxy : proxies)
            {
                renderer->DrawMesh(proxy.meshID, proxy.modelMatrix, 
                    proxy.material.get());
            }
        }
        renderer->EndFrame(); // ImGui 마무리 및 버퍼 스왑
    }
    renderer->Shutdown();
}

void RenderSystem::Shutdown() {
    StopRenderThread();
    renderer->Shutdown();
}
