#include "pch.h"
#include "Engine.h"
#include "world.h"

//Engine Components
#include "InputSystem.h"
#include "WorldManager.h"
#include "RenderSystem.h"
#include "TaskManager.h"

//Selected Renderer
#include "OpenGLRenderer.h"

Engine& Engine::GetInstance() {
    static Engine instance;
    return instance;
}

void Engine::Initialize(GraphicsType gType) {
    inputSystem = std::make_unique<InputSystem>();

    std::unique_ptr<Renderer> renderer; 
    switch (gType)
    {
    case GraphicsType::OpenGL:
    {
        renderer = std::make_unique<OpenGLRenderer>();
    }break;
    case GraphicsType::DirectX11:
    {

    }break;
    }
    
    renderSystem = std::make_unique<RenderSystem>(std::move(renderer));
    renderSystem->Initialize(1280, 720, "Illuminare");


    worldManager = std::make_unique<WorldManager>();

    auto world = std::make_shared<World>();
    world->CreateTestScene();
    worldManager->AddWorld("Test", world);
    worldManager->SetActiveWorld("Test");

    taskManager = std::make_unique<TaskManager>();

    renderSystem->GetRenderer()->MultithreadingSetup();
}

void Engine::Run() {
    using clock = std::chrono::high_resolution_clock;
    auto last = clock::now();

    //RenderLoop() 함수를 실행하는 쓰레드 생성
    renderSystem->StartRenderThread();

    while (isRunning) {
        auto now = clock::now();
        float deltaTime = std::chrono::duration<float>(now - last).count();
        last = now;

        renderSystem->MainThreadEventsBegin(); 
        auto world = worldManager->GetActiveWorld();
        if (world) {
            world->Update(deltaTime);
        }

        inputSystem->Update();
        renderSystem->MainThreadEventsEnd();
    }

    renderSystem->StopRenderThread();
}

void Engine::Shutdown() {
    renderSystem->Shutdown();
}
