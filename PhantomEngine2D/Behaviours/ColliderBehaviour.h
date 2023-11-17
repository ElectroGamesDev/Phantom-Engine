#pragma once

#include "Behaviour.h"
#include <vector>
#include <functional>

class ColliderBehaviour : public Behaviour {
public:
    ColliderBehaviour();
    void Start() override;
    void Update(float deltaTime) override;
    bool IsCollidingWith(GameObject& otherObject);
    std::vector<GameObject>* GetCollidingObjects();
    virtual ~ColliderBehaviour();

    typedef std::function<void(GameObject&)> CollisionCallback;
    void AddCollisionEnterCallback(CollisionCallback callback);
    void AddCollisionStayCallback(CollisionCallback callback);
    void AddCollisionExitCallback(CollisionCallback callback);

private:
    std::vector<GameObject>* objectsColliding;
    std::vector<CollisionCallback> collisionEnterCallbacks;
    std::vector<CollisionCallback> collisionStayCallbacks;
    std::vector<CollisionCallback> collisionExitCallbacks;
};