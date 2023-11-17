#pragma once

#include <string>
#include <vector>
#include <SDL.h>
#include <imgui.h>
#include "ConsoleLogger.h"

class Behaviour;

class GameObject
{
public:
    GameObject(int id = 0);
    ~GameObject();

    SDL_Texture* GetTexture() const;
    void SetTexture(SDL_Texture* texture);
    SDL_Texture* GetGameTexture() const;
    void SetGameTexture(SDL_Texture* texture);
    std::string GetPath() const;
    void SetPath(std::string path);
    std::string GetName() const;
    void SetName(std::string name);
    ImVec2 GetPosition() const;
    void SetPosition(ImVec2 position);
    ImVec2 GetRealSize() const;
    ImVec2 GetSize() const;
    void SetSize(ImVec2 size);
    void SetRealSize(ImVec2 realSize);
    ImVec2 GetRotation() const;
    void SetRotation(ImVec2 rotation);
    int GetId() const;
    ImColor GetBorder() const;
    void SetBorder(ImColor border);
    ImColor GetTint() const; 
    void SetTint(ImColor tint);
    int GetZOrder() const;
    void SetZOrder(int zOrder);
    bool operator==(const GameObject& other) const;
    bool operator!=(const GameObject& other) const;
    void AddBehaviour(Behaviour* behaviour);
    void RemoveBehaviour(Behaviour* behaviour);
    std::vector<Behaviour*> GetBehaviours();

    template<typename T>
    T* GetBehaviour()
    {
        if (behaviours.data() == nullptr || behaviours.empty()) return nullptr;
        for (auto& behaviour : behaviours)
        {
            if (behaviour == nullptr) return nullptr;
            T* tBehaviour = dynamic_cast<T*>(behaviour);
            if (tBehaviour) return tBehaviour;
        }
        return nullptr;
    }

private:
    SDL_Texture* texture;
    SDL_Texture* gameTexture;
    std::string path;
    std::string name;
    ImVec2 position;
    ImVec2 realSize;
    ImVec2 size;
    ImVec2 rotation;
    int id;
    ImColor border;
    ImColor tint = ImColor(255, 255, 255, 255);
    int zOrder;
    std::vector<Behaviour*> behaviours;
};
