#include "pch.h"
#include "World.h"
#include "Mesh.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FreecamComponent.h"
#include "Engine.h"
#include "RenderSystem.h"
#include "Renderer.h"
#include "Actor.h"
#include "TaskManager.h"

void World::Update(float deltaTime) {
    std::vector<std::future<void>> futures;
    for (auto& actor : actors) {
        futures.push_back(
            Engine::GetInstance().GetTaskManager().Enqueue([actor = actor.get(), deltaTime]() {
                actor->Update(deltaTime);
                })
        );
    }

    for (auto& f : futures)
        f.get();
}

void World::AddActor(std::unique_ptr<Actor> actor) {
    actors.push_back(std::move(actor));
}

std::vector<RenderProxy> World::ExtractRenderProxies(Renderer* renderer) {
    std::vector<RenderProxy> proxies;
    for (const auto& actor : actors) {
        auto actorProxies = actor->ExtractRenderProxies(renderer);
        proxies.insert(proxies.end(),
            std::make_move_iterator(actorProxies.begin()),
            std::make_move_iterator(actorProxies.end()));
    }
    return proxies;
}

void World::SetGlobalShaderMatrices() const {
    auto cam = GetCurrentCamera();
    if (!cam) return;

    auto camTransform = cam->GetComponent<TransformComponent>();
    auto camCamera = cam->GetComponent<CameraComponent>();

    glm::vec3 pos = camTransform->position;
    glm::mat4 view = camCamera->GetViewMatrix();
    glm::mat4 proj = camCamera->GetProjectionMatrix();

    for (const auto& actor : actors) {
        auto modelComp = actor->GetComponent<ModelComponent>();
        if (!modelComp || !modelComp->GetModel()) continue;

        std::function<void(const std::shared_ptr<Node>&)> applyToNode;
        applyToNode = [&](const std::shared_ptr<Node>& node) {
            for (auto& [mesh, material] : node->meshMaterials) {
                if (material) {
                    material->SetVec3("viewPos", pos);
                    material->SetVec3("lightDir", glm::normalize(glm::vec3(-1, -1, -1)));
                    material->SetMat4("view", view);
                    material->SetMat4("projection", proj);
                }
            }
            for (auto& child : node->children) {
                applyToNode(child);
            }
            };

        applyToNode(modelComp->GetModel()->rootNode);
    }
}

std::shared_ptr<Model> WrapMeshAsModel(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat = nullptr)
{
    auto model = std::make_shared<Model>();
    auto node = std::make_shared<Node>();
    node->meshMaterials.push_back({ mesh, mat });
    model->rootNode = node;
    return model;
}

std::shared_ptr<Mesh> GenerateNormalLines(const std::shared_ptr<Mesh>& mesh, float length = 0.2f)
{
    auto lineMesh = std::make_shared<Mesh>();
    lineMesh->vertices.clear();
    lineMesh->indices.clear();

    for (const auto& v : mesh->vertices) {
        Vertex start = v;
        Vertex end = v;

        // end 위치는 normal 방향으로 이동
        end.position += v.normal * length;

        lineMesh->vertices.push_back(start);
        lineMesh->vertices.push_back(end);
    }

    return lineMesh;
}

void World::CreateTestScene() {
    auto cameraActor = std::make_unique<Actor>();
    auto cameraTransform = cameraActor->AddComponent<TransformComponent>();
    cameraTransform->position = { 0.f, 0.f, 5.f };
    cameraActor->AddComponent<CameraComponent>()->SetCurrentCamera(true);
    cameraActor->AddComponent<FreecamComponent>();
    AddActor(std::move(cameraActor));

    auto carModel = std::make_shared<Model>();
    carModel->LoadFromOBJ("Mesh/bugatti.obj");

    auto carActor = std::make_unique<Actor>();
    carActor->AddComponent<TransformComponent>();
    carActor->AddComponent<ModelComponent>(carModel);
    AddActor(std::move(carActor));

    auto buildingModel = std::make_shared<Model>();
    buildingModel->LoadFromOBJ("Mesh/GARAGE FLOOR.obj");

    auto buildingActor = std::make_unique<Actor>();
    auto buildingTransform = buildingActor->AddComponent<TransformComponent>();
    buildingTransform->scale = glm::vec3{ 0.1f };
    buildingTransform->position = glm::vec3{ 0.f, 0.f, 0.f };
    buildingActor->AddComponent<ModelComponent>(buildingModel);
    AddActor(std::move(buildingActor));

    //auto lightModel = std::make_shared<Model>();
    //lightModel->LoadFromOBJ("Mesh/sphere.obj"); // 작고 둥근 모델이 이상적
    
    //auto lightShader = Engine::GetInstance().GetRenderSystem().GetRenderer()->CreateShader("Shader/light_default.vert", "Shader/light_default.frag");
    //auto lightMat = std::make_shared<Material>(lightShader);
    //lightMat->SetVec3("albedo", glm::vec3(1.0f, 1.0f, 0.3f)); // 노란빛

    //std::function<void(std::shared_ptr<Node>)> applyLightMat;
    //applyLightMat = [&](std::shared_ptr<Node> node) {
    //    for (auto& pair : node->meshMaterials) {
    //        pair.second = lightMat;
    //    }
    //    for (auto& child : node->children) {
    //        applyLightMat(child);
    //    }
    //    };
    //applyLightMat(lightModel->rootNode);

    //auto lightActor = std::make_unique<Actor>();
    //auto lightTransform = lightActor->AddComponent<TransformComponent>();
    //lightTransform->position = { 0.f, 5.f, 5.f };
    //lightTransform->scale = { 0.2f, 0.2f, 0.2f }; // 작게 표시
    //lightActor->AddComponent<ModelComponent>(lightModel);
    //AddActor(std::move(lightActor));
}

Actor* World::GetCurrentCamera() const {
    for (const auto& actor : actors) {
        auto cam = actor->GetComponent<CameraComponent>();
        if (cam && cam->IsCurrentCamera()) {
            return actor.get();
        }
    }
    return nullptr;
}
