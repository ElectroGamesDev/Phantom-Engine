#include "GameObject.h"
#include "Behaviours/ColliderBehaviour.h"
#include "Scenes/SceneManager.h"
#include "ConsoleLogger.h"
#include "EditorWindow.h"

GameObject::GameObject(int id)
{
    this->texture = texture;
    this->gameTexture = gameTexture;
    this->path = path;
    this->name = name;
    this->position = position;
    this->realSize = realSize;
    this->size = size;
    this->rotation = rotation;
    this->id = id;
}

void GameObject::AddBehaviour(Behaviour* behaviour)
{
    behaviours.push_back(behaviour);
}

void GameObject::RemoveBehaviour(Behaviour* behaviour)
{
    auto iter = std::find(behaviours.begin(), behaviours.end(), behaviour);
    if (iter != behaviours.end())
    {
        behaviours.erase(iter);
        delete behaviour;
    }
}

std::vector<Behaviour*> GameObject::GetBehaviours()
{
    return behaviours;
}

SDL_Texture* GameObject::GetTexture() const
{
    return texture;
}

void GameObject::SetTexture(SDL_Texture* texture)
{
    this->texture = texture;
}

SDL_Texture* GameObject::GetGameTexture() const
{
    return gameTexture;
}

void GameObject::SetGameTexture(SDL_Texture* gameTexture)
{
    this->gameTexture = gameTexture;
}

std::string GameObject::GetPath() const
{
    return path;
}

void GameObject::SetPath(std::string path)
{
    this->path = path;
}

std::string GameObject::GetName() const
{
    return name;
}

void GameObject::SetName(std::string name)
{
    this->name = name;
}

ImVec2 GameObject::GetPosition() const
{
    return position;
}

void GameObject::SetPosition(ImVec2 position)
{
    if (PhantomEditor::EditorWindow::IsPlaying() && GetBehaviour<ColliderBehaviour>() != nullptr)
    {
        for (GameObject& object : SceneManager::GetActiveScene()->GetGameObjects()) {
            if (object.GetId() == GetId() || !object.GetBehaviour<ColliderBehaviour>()) continue;
            SDL_Rect rect1 = { static_cast<int>(position.x), static_cast<int>(position.y ), GetSize().x, GetSize().y };
            SDL_Rect rect2 = { static_cast<int>(object.GetPosition().x), static_cast<int>(object.GetPosition().y), object.GetSize().x, object.GetSize().y };
            //SDL_SetRenderDrawColor(PhantomEditor::EditorWindow::GameRenderer(), 255, 0, 0, 255);
            //SDL_RenderDrawRect(PhantomEditor::EditorWindow::GameRenderer(), &rect1);
            //SDL_SetRenderDrawColor(PhantomEditor::EditorWindow::GameRenderer(), 0, 255, 0, 255);
            //SDL_RenderDrawRect(PhantomEditor::EditorWindow::GameRenderer(), &rect2);
            if (!static_cast<bool>(SDL_HasIntersection(&rect1, &rect2)))
            {
                //ConsoleLogger::InfoLog("Not colliding");
                this->position = position;
                return;
            }
            else
            {
                //ConsoleLogger::InfoLog("Colliding with " + object.GetName());
                return; // Exit the loop if there is a collision
            }
        }
        // If there is no collision, set the position after the loop
        this->position = position;
    }
    else
    {
        this->position = position;
    }
}


ImVec2 GameObject::GetRealSize() const
{
    return realSize;
}

ImVec2 GameObject::GetSize() const
{
    return size;
}

void GameObject::SetSize(ImVec2 size)
{
    this->size = size;
}

void GameObject::SetRealSize(ImVec2 realSize)
{
    this->realSize = realSize;
}

ImVec2 GameObject::GetRotation() const
{
    return rotation;
}

void GameObject::SetRotation(ImVec2 rotation)
{
    this->rotation = rotation;
}

int GameObject::GetId() const
{
    return id;
}

ImColor GameObject::GetBorder() const
{
    return border;
}

void GameObject::SetBorder(ImColor border)
{
    this->border = border;
}

ImColor GameObject::GetTint() const
{
    return tint;
}

void GameObject::SetTint(ImColor tint)
{
    this->tint = tint;
}

int GameObject::GetZOrder() const
{
    return zOrder;
}

void GameObject::SetZOrder(int zOrder)
{
    this->zOrder = zOrder;
}

bool GameObject::operator==(const GameObject& other) const
{
    return this->id == other.id;
}

bool GameObject::operator!=(const GameObject& other) const
{
    return this->id != other.id;
}

GameObject::~GameObject()       // TODO: FIX THIS
{
    //for (auto& behaviour : behaviours)
    //{
    //    if (behaviour != nullptr)
    //    {
    //        delete behaviour;
    //        behaviour = nullptr;
    //    }
    //}
    //behaviours.clear();
}