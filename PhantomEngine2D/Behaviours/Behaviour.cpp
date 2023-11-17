#include "Behaviour.h"
#include "../ConsoleLogger.h"
#include "../GameObject.h"

Behaviour::Behaviour()
    : gameObject(nullptr)
{
}

Behaviour::~Behaviour()
{
}


void Behaviour::Update(float deltaTime)
{
    // Implementation of the Update function
}

void Behaviour::Start()
{
    // Implementation of the Start function
}

void Behaviour::SetName(const std::string& name) {
    behaviourName = name;
}

GameObject* Behaviour::GetGameObject() {
    return gameObject;
}

/**
 * This function should not be used in behaviors, it is for internal use only.
 */
void Behaviour::SetGameObject(GameObject* object) {
    gameObject = object;
}

const std::string& Behaviour::GetName() const {
    return behaviourName;
}

void Behaviour::SetRenderInEditMode(bool value) {
    updateInEditMode = value;
}

bool Behaviour::GetRenderInEditMode()
{
    return updateInEditMode;
}