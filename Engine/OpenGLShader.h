#pragma once
#include "Shader.h"

class OpenGLShader : public Shader {
public:
    OpenGLShader(GLuint programID);
    ~OpenGLShader();

    void Use() const override;
    void SetUniform(const std::string& name, int value) const override;
    void SetUniform(const std::string& name, float value) const override;
    void SetUniform(const std::string& name, const glm::vec3& value) const override;
    void SetUniform(const std::string& name, const glm::mat4& value) const override;

private:
    GLuint programID;
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;

    GLint GetUniformLocation(const std::string& name) const;
};