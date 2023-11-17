#include "SDL.h"
#include "../ConsoleLogger.h"
#include "EnemyPaddleMoveBehaviour.h"
#include "Scenes/SceneManager.h"

EnemyPaddleMoveBehaviour::EnemyPaddleMoveBehaviour(float speed)
    : movementSpeed(speed)
{
    SetName("Enemy Paddle Movement Behaviour");
}

void EnemyPaddleMoveBehaviour::Start()
{

}

void EnemyPaddleMoveBehaviour::Update(float deltaTime)
{
    GetGameObject()->SetPosition(ImVec2(GetGameObject()->GetPosition().x, SceneManager::GetActiveScene()->GetGameObject("Ball")->GetPosition().y - 50));
}

EnemyPaddleMoveBehaviour::~EnemyPaddleMoveBehaviour()
{
    // Destructor code here
}
