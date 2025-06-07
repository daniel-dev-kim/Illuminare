#pragma once
#include "Component.h"
class FreecamComponent : public Component {
public:
    float speed = 5.0f;
    float sensitivity = 0.1f;
    glm::vec2 lastMousePos{ 0.0f };
    bool camMove = false;
    float yaw = 270.f; // 기본 방향 설정
    float pitch = 0.0f;

    void Update(float deltaTime) override;
};