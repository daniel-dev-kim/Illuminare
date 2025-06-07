#include "pch.h"
#include "FreecamComponent.h"
#include "Engine.h"
#include "InputSystem.h"
#include "Actor.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "RenderSystem.h"
#include "OpenGLRenderer.h"

void FreecamComponent::Update(float deltaTime)
{
    auto& input = Engine::GetInstance().GetInputSystem();
    OpenGLRenderer* opr = dynamic_cast<OpenGLRenderer*>(Engine::GetInstance().GetRenderSystem().GetRenderer());

    if (input.IsKeyPressed(Keyboard::Space))
    {
        camMove = !camMove;
    }

    if (camMove) 
    {
        auto transform = owner->GetComponent<TransformComponent>();
        if (!transform) return;

        glm::vec3 direction{};
        glm::vec3 forward = transform->GetForward();
        glm::vec3 right = transform->GetRight();


        if (input.IsKeyDown(Keyboard::W))
            direction += forward;
        if (input.IsKeyDown(Keyboard::S))
            direction -= forward;
        if (input.IsKeyDown(Keyboard::A))
            direction -= right;
        if (input.IsKeyDown(Keyboard::D))
            direction += right;

        if (glm::length(direction) > 0)
            transform->position += glm::normalize(direction) * speed * deltaTime;

        glm::vec2 offset = input.GetMouseDelta() * sensitivity;
        yaw += offset.x;
        pitch -= offset.y;

        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        transform->SetForward(front);
    }
    
}