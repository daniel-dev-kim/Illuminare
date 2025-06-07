#include "pch.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "Actor.h"

glm::mat4 CameraComponent::GetViewMatrix() const
{
    auto transform = owner->GetComponent<TransformComponent>();
    glm::vec3 position = transform->position;
    glm::vec3 forward = transform->GetForward();
    glm::vec3 up = glm::vec3(0, 1, 0);

    return glm::lookAt(position, position + forward, up);
}

glm::mat4 CameraComponent::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}
