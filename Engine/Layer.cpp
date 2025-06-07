#include "pch.h"
#include "Layer.h"
#include "Actor.h"

Layer::Layer(const Layer& other) {
    for (const auto& actor : other.actors) {
        actors.push_back(actor->Clone());
    }
}

std::unique_ptr<Layer> Layer::Clone() const {
    return std::make_unique<Layer>(*this); // 복사 생성자 호출
}

void Layer::AddActor(std::unique_ptr<Actor> actor) {
    actors.emplace_back(std::move(actor));
}

void Layer::Update(float deltaTime) {
    std::for_each(
        std::execution::par_unseq,               // 병렬 실행
        actors.begin(),
        actors.end(),
        [deltaTime](const std::unique_ptr<Actor>& actor) {
            actor->Update(deltaTime);
        }
    );
}

const std::vector<std::unique_ptr<Actor>>& Layer::GetActors() const {
    return actors;
}