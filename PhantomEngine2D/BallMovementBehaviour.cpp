#include "SDL.h"
#include "../ConsoleLogger.h"
#include "BallMovementBehaviour.h"
#include "Scenes/SceneManager.h"

BallMovementBehaviour::BallMovementBehaviour(float speed, ImVec2 vel)
    : movementSpeed(speed), velocity(vel)
{
    SetName("Ball Movement Behaviour");
}

void BallMovementBehaviour::Start()
{

}

void BallMovementBehaviour::Update(float deltaTime)
{
    GetGameObject()->SetPosition(ImVec2(GetGameObject()->GetPosition().x + velocity.x * movementSpeed * deltaTime, GetGameObject()->GetPosition().y + velocity.y * movementSpeed * deltaTime));
}

BallMovementBehaviour::~BallMovementBehaviour()
{
    // Destructor code here
}
