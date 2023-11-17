#include "ColliderBehaviour.h"
#include "../Scenes/SceneManager.h"
#include <algorithm>
#include "../ConsoleLogger.h"

ColliderBehaviour::ColliderBehaviour()
{
    SetName("Collider Behaviour");
    objectsColliding = new std::vector<GameObject>();
}

void ColliderBehaviour::Start()
{
}

bool ColliderBehaviour::IsCollidingWith(GameObject& other) {
    if (other.GetBehaviour<ColliderBehaviour>() == nullptr) return false;

    SDL_Rect rect1 = { static_cast<int>(GetGameObject()->GetPosition().x), static_cast<int>(GetGameObject()->GetPosition().y), GetGameObject()->GetSize().x, GetGameObject()->GetSize().y};
    SDL_Rect rect2 = { static_cast<int>(other.GetPosition().x), static_cast<int>(other.GetPosition().y), other.GetSize().x, other.GetSize().y};

    return static_cast<bool>(SDL_HasIntersection(&rect1, &rect2) == SDL_TRUE);
}

std::vector<GameObject>* ColliderBehaviour::GetCollidingObjects() {
    return objectsColliding;
}

void ColliderBehaviour::Update(float deltaTime)
{
    for (GameObject& object : SceneManager::GetActiveScene()->GetGameObjects()) {
        if (object.GetId() != GetGameObject()->GetId())
        {
            if (IsCollidingWith(object))
            {

                if (std::find_if(objectsColliding->begin(), objectsColliding->end(),
                    [&](GameObject& obj) { return obj.GetId() == object.GetId(); }) != objectsColliding->end())
                {
                    //ConsoleLogger::InfoLog("Stayed in collision");
                    for (auto& callback : collisionStayCallbacks) {
                        callback(object);
                    }
                }
                else
                {
                    objectsColliding->push_back(object);
                    //ConsoleLogger::InfoLog("Entered collision");
                    for (auto& callback : collisionEnterCallbacks) {
                        callback(object);
                    }
                }
            }
            else if (!objectsColliding->empty())
            {
                auto it = std::find(objectsColliding->begin(), objectsColliding->end(), object);
                if (it != objectsColliding->end())
                {
                    objectsColliding->erase(it);
                }
                for (auto& callback : collisionExitCallbacks) {
                    callback(object);
                }
                //ConsoleLogger::InfoLog("Exited collision");
            }
        }
    }
}

void ColliderBehaviour::AddCollisionEnterCallback(CollisionCallback callback) {
    collisionEnterCallbacks.push_back(callback);
}

void ColliderBehaviour::AddCollisionStayCallback(CollisionCallback callback) {
    collisionStayCallbacks.push_back(callback);
}

void ColliderBehaviour::AddCollisionExitCallback(CollisionCallback callback) {
    collisionExitCallbacks.push_back(callback);
}

ColliderBehaviour::~ColliderBehaviour()
{
    delete objectsColliding;
}