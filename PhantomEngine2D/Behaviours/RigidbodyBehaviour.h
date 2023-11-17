#pragma once

#include "Behaviour.h"
#include "../GameObject.h"

class RigidbodyBehaviour : public Behaviour {
public:
    RigidbodyBehaviour(float gravity = 300);
    void Update(float deltaTime) override;
    virtual ~RigidbodyBehaviour();
    void Start() override;

private:
    float gravity;
};
