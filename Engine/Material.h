#pragma once
class Shader;

class Material {
public:
    Material(std::shared_ptr<Shader> shader);
    void Use() const;

    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec3(const std::string& name, const glm::vec3& vec);
    void SetMat4(const std::string& name, const glm::mat4& mat);
    void SetTexture(const std::string& name, GLuint textureID);

private:
    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, int> intParams;
    std::unordered_map<std::string, float> floatParams;
    std::unordered_map<std::string, glm::vec3> vec3Params;
    std::unordered_map<std::string, glm::mat4> mat4Params;
    std::unordered_map<std::string, GLuint> textureParams;
};