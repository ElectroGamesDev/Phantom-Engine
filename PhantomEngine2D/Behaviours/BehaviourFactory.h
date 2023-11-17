#pragma once
#include "Behaviour.h"
#include "ColliderBehaviour.h"
#include "RigidbodyBehaviour.h"
#include "../PresetBehaviours/MoveBehaviour.h"
#include <json.hpp>

class BehaviourFactory
{
public:
    static Behaviour* CreateBehaviour(const nlohmann::json& behaviourData)
    {
        if (behaviourData["name"] == "Movement Behaviour")
        {
            //float speed = behaviourData["speed"];
            return new MoveBehaviour();
        }
        else if (behaviourData["name"] == "Collider Behaviour")
        {
            return new ColliderBehaviour();
        }
        else if (behaviourData["name"] == "Rigibody Behaviour")
        {
            return new RigidbodyBehaviour();
        }
        else
        {
            return nullptr;
        }
    }
};
