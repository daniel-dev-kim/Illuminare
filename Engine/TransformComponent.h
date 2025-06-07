#pragma once
#include "Component.h"

class TransformComponent : public Component {
public:
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale{ 1.0f };

    glm::mat4 GetMatrix() const;

    glm::vec3 GetForward() const;
    glm::vec3 GetUp() const;
    glm::vec3 GetRight() const;

    void SetForward(const glm::vec3& direction);

    void Update(float deltaTime) override {}
};