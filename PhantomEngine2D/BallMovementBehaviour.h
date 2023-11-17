#pragma once

#include "Behaviour.h"
#include "../GameObject.h"

class BallMovementBehaviour : public Behaviour {
public:
    BallMovementBehaviour(float speed = 2000000, ImVec2 vel = ImVec2(1, -0.25f));
    void Update(float deltaTime) override;
    virtual ~BallMovementBehaviour();
    void Start() override;
    float movementSpeed;
    ImVec2 velocity;
};