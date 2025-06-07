#include "pch.h"
#include "TransformComponent.h"

glm::mat4 TransformComponent::GetMatrix() const
{
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotate = glm::toMat4(glm::quat(rotation));
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
    return translate * rotate * scaleMat;
}

glm::vec3 TransformComponent::GetRight() const
{
	return glm::toMat4(glm::quat(rotation)) * glm::vec4{ 1.f,0.f,0.f ,0.f };
}

glm::vec3 TransformComponent::GetUp() const
{
	return glm::toMat4(glm::quat(rotation)) * glm::vec4{ 0.f,1.f,0.f ,0.f };
}

glm::vec3 TransformComponent::GetForward() const
{
	return glm::toMat4(glm::quat(rotation)) * glm::vec4{ 0.f,0.f,-1.f ,0.f };
}


void TransformComponent::SetForward(const glm::vec3& direction)
{
	// Is the normal up (nearly) parallel to direction?
	glm::vec3 up = glm::vec3{ 0.f,1.f,0.f };
	if (glm::abs(glm::dot(direction, up)) > .9999f) {
		// Use alternative up
		glm::vec3 result = glm::eulerAngles(glm::quatLookAt(glm::normalize(direction), { 0.f,1.01f,0.f }));
		rotation = result;

	}

	glm::vec3 result = glm::eulerAngles(glm::quatLookAt(glm::normalize(direction), up));
	rotation = result;
}
