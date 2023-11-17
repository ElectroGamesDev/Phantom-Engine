#pragma once

#include <vector>

#include "../GameObject.h"
#include "Scene.h"
#include <json.hpp>
#include "Behaviour.h"
using json = nlohmann::json;

class SceneManager {
public:
    SceneManager();
    static Scene* GetActiveScene();
    static void SetActiveScene(Scene* scene);
    /**
    * @brief This function should not be used in behaviors and is for internal use only.
    */
    static void AddScene(Scene scene);
    static std::vector<Scene>* GetScenes();
    static void SaveScene(Scene* scene);
    static bool LoadScene(const std::filesystem::path& filePath);
    /**
    * @brief Resets the scene to how it was before it was modified in play/build mode.
    */
    static void ResetScene(Scene* scene);
    /**
    * @brief This function should not be used in behaviors and is for internal use only.
    */
    static void BackupScene(Scene* scene);
private:
    static Scene* m_activeScene;
    static std::vector<Scene> m_scenes;
};
