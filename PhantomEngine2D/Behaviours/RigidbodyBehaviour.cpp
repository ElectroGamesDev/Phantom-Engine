#include "RigidbodyBehaviour.h"
#include "../ConsoleLogger.h"

RigidbodyBehaviour::RigidbodyBehaviour(float gravity)
    : gravity(gravity)
{
    SetName("Rigidbody Behaviour");
}

void RigidbodyBehaviour::Start()
{

}

void RigidbodyBehaviour::Update(float deltaTime)
{
    ImVec2 position = GetGameObject()->GetPosition();
    position.y += gravity * deltaTime;
    GetGameObject()->SetPosition(position);
}

RigidbodyBehaviour::~RigidbodyBehaviour()
{
    // Destructor code here
}
