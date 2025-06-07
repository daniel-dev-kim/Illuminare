// LightComponent.h
#pragma once
#include "Component.h"
#include "pch.h"

class LightComponent : public Component {
public:
    glm::vec3 color{ 1.0f, 1.0f, 1.0f };
    float intensity = 1.0f;

    void Update(float deltaTime) override {}
};