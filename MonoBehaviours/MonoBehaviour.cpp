#include "pch.h"
#include "MonoBehaviour.h"
#include "Behaviour.h" 

// Constructor
MonoBehaviour::MonoBehaviour()
{
    nativeBehaviour = new Behaviour();
}

// Implementation of Update function
void MonoBehaviour::Update(float deltaTime)
{
    nativeBehaviour->Update(deltaTime);
}

// Implementation of Start function
void MonoBehaviour::Start()
{
    nativeBehaviour->Start();
}
