// CameraComponent.h
#pragma once
#include "Component.h"

class CameraComponent : public Component {
public:
    float fov = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;
    bool isCurrentCamera = false;

    glm::mat4 GetViewMatrix() const;

    glm::mat4 GetProjectionMatrix() const;

    void SetCurrentCamera(bool value) { isCurrentCamera = value; }
    bool IsCurrentCamera() const { return isCurrentCamera; }

    void Update(float deltaTime) override {}
};