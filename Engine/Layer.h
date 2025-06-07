#pragma once
class Actor;
class Layer {
public:
    Layer() = default;
    Layer(const Layer& other);                     // ���� ������
    std::unique_ptr<Layer> Clone() const;          // ������Ÿ�� ���Ͽ� ���� �Լ�

    void Update(float deltaTime);                  // Actor ������Ʈ
    void AddActor(std::unique_ptr<Actor> actor);   // Actor �߰�
    const std::vector<std::unique_ptr<Actor>>& GetActors() const;

private:
    std::vector<std::unique_ptr<Actor>> actors;
};