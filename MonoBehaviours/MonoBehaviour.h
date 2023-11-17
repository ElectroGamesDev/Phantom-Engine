#pragma once

class Behaviour; // Forward declaration of the C++ Behaviour class

public ref class MonoBehaviour
{
public:
    MonoBehaviour(); // Constructor
    void Update(float deltaTime);
    void Start();
private:
    Behaviour* nativeBehaviour; // Pointer to the C++ Behaviour instance
};
