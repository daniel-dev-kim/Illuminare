#include "pch.h"
#include "OpenGLShader.h"
OpenGLShader::OpenGLShader(GLuint program)
    : programID(program) {
}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(programID);
}

void OpenGLShader::Use() const {
    glUseProgram(programID);
}



GLint OpenGLShader::GetUniformLocation(const std::string& name) const {
    if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
        return uniformLocationCache[name];
    }

    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or unused in shader!" << std::endl;
    }

    uniformLocationCache[name] = location;
    return location;
}

void OpenGLShader::SetUniform(const std::string& name, int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void OpenGLShader::SetUniform(const std::string& name, float value) const {
    glUniform1f(GetUniformLocation(name), value);
}

void OpenGLShader::SetUniform(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void OpenGLShader::SetUniform(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}