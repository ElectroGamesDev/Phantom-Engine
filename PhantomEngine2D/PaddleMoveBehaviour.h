#pragma once

#include "Behaviour.h"
#include "../GameObject.h"

class PaddleMoveBehaviour : public Behaviour {
public:
    PaddleMoveBehaviour(float speed = 300000.0f);
    void Update(float deltaTime) override;
    virtual ~PaddleMoveBehaviour();
    void Start() override;
    float movementSpeed;
};