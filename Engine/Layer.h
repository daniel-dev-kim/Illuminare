#pragma once
class Actor;
class Layer {
public:
    Layer() = default;
    Layer(const Layer& other);                     // 복사 생성자
    std::unique_ptr<Layer> Clone() const;          // 프로토타입 패턴용 복제 함수

    void Update(float deltaTime);                  // Actor 업데이트
    void AddActor(std::unique_ptr<Actor> actor);   // Actor 추가
    const std::vector<std::unique_ptr<Actor>>& GetActors() const;

private:
    std::vector<std::unique_ptr<Actor>> actors;
};