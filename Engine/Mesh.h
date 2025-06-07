#pragma once
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices; // optional
    GLenum drawMode = GL_TRIANGLES; // default
};