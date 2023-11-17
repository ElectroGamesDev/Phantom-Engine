#include "MoveBehaviour.h"
#include "SDL.h"
#include "../ConsoleLogger.h"

MoveBehaviour::MoveBehaviour(float speed, float jumpForce)
    : movementSpeed(speed), jumpSpeed(jumpForce)
{
    SetName("Movement Behaviour");
}

void MoveBehaviour::Start()
{

}

void MoveBehaviour::Update(float deltaTime)
{
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_A]) {
        GetGameObject()->SetPosition(ImVec2(GetGameObject()->GetPosition().x - movementSpeed * deltaTime, GetGameObject()->GetPosition().y));
    }
    else if (currentKeyStates[SDL_SCANCODE_D]) {
        GetGameObject()->SetPosition(ImVec2(GetGameObject()->GetPosition().x + movementSpeed * deltaTime, GetGameObject()->GetPosition().y));
    }

    //if (currentKeyStates[SDL_SCANCODE_SPACE] && !isJumping) {
    //    isJumping = true;
    //}

    //if (isJumping) {
    //    GetGameObject()->SetPosition(ImVec2(GetGameObject()->GetPosition().x, GetGameObject()->GetPosition().y - jumpSpeed * deltaTime));
    //    jumpSpeed -= 300.0f * deltaTime;

    //    if (GetGameObject()->GetPosition().y >= 600.0f) {
    //        GetGameObject()->SetPosition(ImVec2(GetGameObject()->GetPosition().x, 600.0f));
    //        jumpSpeed = 400.0f;
    //        isJumping = false;
    //    }
    //}
}

MoveBehaviour::~MoveBehaviour()
{
    // Destructor code here
}