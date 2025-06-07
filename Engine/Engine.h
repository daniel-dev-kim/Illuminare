#pragma once
class InputSystem;
class RenderSystem;
class TaskManager;
class WorldManager;
enum class GraphicsType
{
    OpenGL,
    DirectX11,
};
class Engine {

public:
    static Engine& GetInstance();
    InputSystem& GetInputSystem() { return *inputSystem; }
    RenderSystem& GetRenderSystem() { return *renderSystem; }
    TaskManager& GetTaskManager() { return *taskManager; }
    WorldManager& GetWorldManager() { return *worldManager; }

    void Initialize(GraphicsType type);
    void Run();
    void Shutdown();

private:
    Engine() = default;
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;

    std::unique_ptr<InputSystem> inputSystem;
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<TaskManager> taskManager;
    std::unique_ptr<WorldManager> worldManager;
    bool isRunning = true;
};
