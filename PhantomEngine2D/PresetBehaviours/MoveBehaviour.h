#pragma once

#include "Behaviour.h"
#include "../GameObject.h"

class MoveBehaviour : public Behaviour {
public:
    MoveBehaviour(float speed = 500.0f, float jumpForce = 2000);
    void Update(float deltaTime) override;
    virtual ~MoveBehaviour();
    void Start() override;
    float movementSpeed;
    float jumpSpeed;
    bool isJumping = false;
};