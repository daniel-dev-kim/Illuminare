#pragma once
#include "pch.h"

class Mesh;
class Material;
class Shader;
class Texture;
class Renderer {
public:
    virtual ~Renderer() = default;

    //Init
    virtual bool Initialize(int width, int height, const std::string& title) = 0;
    virtual void MultithreadingSetup() = 0;
    //Loop
    virtual void MainThreadEventsBegin() = 0;
    virtual void LoopSetup() = 0;
    virtual void BeginShadowPass() = 0;
    virtual void EndShadowPass() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void MainThreadEventsEnd() = 0;
    //Shutdown
    virtual bool ShouldClose() = 0;
    virtual void Shutdown() = 0;


    virtual int UploadMesh(std::shared_ptr<Mesh> mesh) = 0;
    virtual std::shared_ptr<Shader> CreateShader(const std::string&, const std::string&) = 0;
    virtual GLuint LoadTexture(const std::string& path) = 0;
    virtual void DrawMesh(int meshID, const glm::mat4& modelMatrix, Material* material) = 0;
};