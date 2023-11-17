#pragma once
#include <vector>
#include "../GameObject.h"
#include <filesystem>

class Scene {
public:
    Scene(const std::filesystem::path& path, std::vector<GameObject> gameObjects);
    ~Scene();

    std::filesystem::path GetPath() const { return m_Path; }
    void AddGameObject(GameObject& gameObject);
    void RemoveGameObject(GameObject* gameObject);
    GameObject* GetGameObject(const std::string& name);
    std::vector<GameObject>& GetGameObjects();
    /**
    * @brief This variable should not be used in behaviors and is for internal use only.
    */
    std::vector<GameObject> gameObjectsBackup;
private:
    std::filesystem::path m_Path;
    std::vector<GameObject> m_GameObjects;
};
