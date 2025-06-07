// Shader.h
#pragma once
#include "pch.h"

class Shader {
public:
    virtual ~Shader() = default;

    virtual void Use() const = 0;
    virtual void SetUniform(const std::string& name, int value) const = 0;
    virtual void SetUniform(const std::string& name, float value) const = 0;
    virtual void SetUniform(const std::string& name, const glm::vec3& value) const = 0;
    virtual void SetUniform(const std::string& name, const glm::mat4& value) const = 0;
};