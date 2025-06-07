#include "pch.h"
#include "Engine.h"

int main() {
    try {
        Engine& engine = Engine::GetInstance();
        engine.Initialize(GraphicsType::OpenGL);
        engine.Run();
        engine.Shutdown();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

