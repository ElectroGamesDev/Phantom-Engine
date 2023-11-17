#include "Scene.h"
#include <algorithm>

Scene::Scene(const std::filesystem::path& path, std::vector<GameObject> gameObjects)
    : m_Path(path), m_GameObjects(gameObjects)
{
}

Scene::~Scene()
{
}

void Scene::AddGameObject(GameObject& gameObject)
{
    m_GameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(GameObject* gameObject)
{
    auto it = std::find_if(m_GameObjects.begin(), m_GameObjects.end(),
        [gameObject](const GameObject& go) { return go.GetId() == gameObject->GetId(); });
    if (it != m_GameObjects.end()) {
        //delete& (*it);
        m_GameObjects.erase(it);
    }
}


std::vector<GameObject>& Scene::GetGameObjects() {
    return m_GameObjects;
}

GameObject* Scene::GetGameObject(const std::string& name) {
    for (GameObject& gameObject : m_GameObjects) {
        if (gameObject.GetName() == name) {
            return &gameObject;
        }
    }
    return nullptr;
}