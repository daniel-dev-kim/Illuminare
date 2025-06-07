// Input.h
#pragma once
#include <unordered_map>
#include <glm/glm.hpp>

enum class Keyboard {
    A = 65, B, C, D, E, F, G, H, I, J,
    K, L, M, N, O, P, Q, R, S, T,
    U, V, W, X, Y, Z,
    Space = 32,
    Enter = 257,
    Escape = 256,
    Left = 263,
    Right = 262,
    Up = 265,
    Down = 264
};

class InputSystem {
public:
    void Update();

    void FeedKeyEvent(int key, int action);
    void FeedMouseMove(double xpos, double ypos);
    void FeedMouseScroll(double xoffset, double yoffset);
    void FeedMouseButton(int button, int action);

    bool IsKeyDown(Keyboard key) const;
    bool IsKeyPressed(Keyboard key) const;
    bool IsKeyReleased(Keyboard key) const;

    bool IsMouseDown(int button) const;
    bool IsMouseClicked(int button) const;
    bool IsMouseReleased(int button) const;

    glm::vec2 GetMousePos() const { return mousePos; }
    glm::vec2 GetMouseDelta() const { return mouseDelta; }
    float GetScrollDelta() const { return scrollDelta; }

private:
    std::unordered_map<int, bool> keyDown, keyPressed, keyReleased;
    std::unordered_map<int, bool> mouseDown, mousePressed, mouseReleased;

    glm::vec2 mousePos{ 0 }, lastMousePos{ 0 }, mouseDelta{ 0 };
    bool firstMouse = true;
    float scrollDelta = 0.0f;
};