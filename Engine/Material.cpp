#include "pch.h"
#include "Material.h"
#include "Shader.h"

Material::Material(std::shared_ptr<Shader> shader)
    : shader(shader) {
}

void Material::Use() const {
    shader->Use();

    for (const auto& [name, value] : intParams)
        shader->SetUniform(name, value);

    for (const auto& [name, value] : floatParams)
        shader->SetUniform(name, value);

    for (const auto& [name, vec] : vec3Params)
        shader->SetUniform(name, vec);

    for (const auto& [name, mat] : mat4Params)
        shader->SetUniform(name, mat);

    int textureUnit = 0;
    for (const auto& [name, texID] : textureParams) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, texID);
        shader->SetUniform(name, textureUnit);
        textureUnit++;
    }
}

void Material::SetInt(const std::string& name, int value)
{
    intParams[name] = value;
}

void Material::SetFloat(const std::string& name, float value) {
    floatParams[name] = value;
}

void Material::SetVec3(const std::string& name, const glm::vec3& vec) {
    vec3Params[name] = vec;
}

void Material::SetMat4(const std::string& name, const glm::mat4& mat) {
    mat4Params[name] = mat;
}

void Material::SetTexture(const std::string& name, GLuint textureID) {
    textureParams[name] = textureID;
}
