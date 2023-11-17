#pragma once

#include "Behaviour.h"
#include "../GameObject.h"

class EnemyPaddleMoveBehaviour : public Behaviour {
public:
    EnemyPaddleMoveBehaviour(float speed = 2000000.0f);
    void Update(float deltaTime) override;
    virtual ~EnemyPaddleMoveBehaviour();
    void Start() override;
    float movementSpeed;
};