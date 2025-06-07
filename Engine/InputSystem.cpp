// Input.cpp
#include "pch.h"
#include "InputSystem.h"

void InputSystem::Update() {
    keyPressed.clear();
    keyReleased.clear();
    mousePressed.clear();
    mouseReleased.clear();
    scrollDelta = 0.0f;

    mouseDelta = firstMouse ? glm::vec2(0.0f) : mousePos - lastMousePos;
    lastMousePos = mousePos;
    firstMouse = false;
}

void InputSystem::FeedKeyEvent(int key, int action) {
    if (action == 1) { // Press
        if (!keyDown[key]) keyPressed[key] = true;
        keyDown[key] = true;
    }
    else if (action == 0) { // Release
        keyDown[key] = false;
        keyReleased[key] = true;
    }
}

void InputSystem::FeedMouseMove(double xpos, double ypos) {
    mousePos = { (float)xpos, (float)ypos };
}

void InputSystem::FeedMouseScroll(double xoffset, double yoffset) {
    scrollDelta = (float)yoffset;
}

void InputSystem::FeedMouseButton(int button, int action) {
    if (action == 1) {
        if (!mouseDown[button]) mousePressed[button] = true;
        mouseDown[button] = true;
    }
    else if (action == 0) {
        mouseDown[button] = false;
        mouseReleased[button] = true;
    }
}

bool InputSystem::IsKeyDown(Keyboard key) const {
    return keyDown.contains((int)key) && keyDown.at((int)key);
}

bool InputSystem::IsKeyPressed(Keyboard key) const {
    return keyPressed.count((int)key);
}

bool InputSystem::IsKeyReleased(Keyboard key) const {
    return keyReleased.count((int)key);
}

bool InputSystem::IsMouseDown(int button) const {
    return mouseDown.contains(button) && mouseDown.at(button);
}

bool InputSystem::IsMouseClicked(int button) const {
    return mousePressed.count(button);
}

bool InputSystem::IsMouseReleased(int button) const {
    return mouseReleased.count(button);
}