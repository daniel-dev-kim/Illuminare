#pragma once

class Actor;
class Component {
public:
    Actor* owner = nullptr;
public:
    virtual void Update(float deltaTime) = 0;
    virtual ~Component() {}
};