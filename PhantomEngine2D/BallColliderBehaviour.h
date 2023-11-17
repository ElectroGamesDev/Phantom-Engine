#pragma once

#include "Behaviour.h"
#include <vector>
#include <functional>

class BallColliderBehaviour : public Behaviour {
public:
    BallColliderBehaviour();
    void Start() override;
    void Update(float deltaTime) override;
    bool IsCollidingWith(GameObject& otherObject);
    virtual ~BallColliderBehaviour();
};