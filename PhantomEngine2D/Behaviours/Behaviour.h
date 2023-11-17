#pragma once

#include <string>
class GameObject;

class Behaviour
{
public:
    Behaviour();
    virtual ~Behaviour();
    virtual void Update(float deltaTime);
    virtual void Start();
    void SetName(const std::string& name);
    GameObject* GetGameObject();
    void SetGameObject(GameObject* object);
    const std::string& GetName() const;
    void SetRenderInEditMode(bool value);
    bool GetRenderInEditMode();

private:
    std::string behaviourName = "Unnamed Behaviour";
    GameObject* gameObject;
    bool updateInEditMode = false;
};
