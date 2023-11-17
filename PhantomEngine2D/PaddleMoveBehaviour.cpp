#include "SDL.h"
#include "../ConsoleLogger.h"
#include "PaddleMoveBehaviour.h"

PaddleMoveBehaviour::PaddleMoveBehaviour(float speed)
    : movementSpeed(speed)
{
    SetName("Paddle Movement Behaviour");
}

void PaddleMoveBehaviour::Start()
{

}

void PaddleMoveBehaviour::Update(float deltaTime)
{
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP]) {
        GetGameObject()->SetPosition(ImVec2(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y - movementSpeed * deltaTime));
    }
    else if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN]) {
        GetGameObject()->SetPosition(ImVec2(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y + movementSpeed * deltaTime));
    }
}

PaddleMoveBehaviour::~PaddleMoveBehaviour()
{
    // Destructor code here
}
