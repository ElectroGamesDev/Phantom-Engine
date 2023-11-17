#include "SceneManager.h"
#include <fstream>
#include <iomanip>
#include <../ConsoleLogger.h>
#include <iostream>
#include <sstream>
#include <SDL_image.h>
#include "../EditorWindow.h"
#include "imgui.h"
#include "BehaviourFactory.h"
#include <algorithm>
#include <cctype>

std::vector<Scene> SceneManager::m_scenes;
Scene* SceneManager::m_activeScene;

SceneManager::SceneManager() {
    m_activeScene = nullptr;
}

Scene* SceneManager::GetActiveScene() {
    return m_activeScene;
}

void SceneManager::SetActiveScene(Scene* scene) {
    m_activeScene = scene;
}

std::vector<Scene>* SceneManager::GetScenes() {
    return &m_scenes; 
}

void SceneManager::SaveScene(Scene* scene)
{
    json sceneData;

    // Save game objects
    for (GameObject& object : scene->GetGameObjects())
    {
        json gameObjectData;

        // Save texture
        if (object.GetTexture() != nullptr) {
            gameObjectData["texture_path"] = object.GetPath();
        }

        // Save name, position, real size, size, rotation, id, tint, zOrder
        gameObjectData["name"] = object.GetName();
        gameObjectData["position"] = { object.GetPosition().x, object.GetPosition().y };
        gameObjectData["real_size"] = { object.GetRealSize().x, object.GetRealSize().y };
        gameObjectData["size"] = { object.GetSize().x, object.GetSize().y };
        gameObjectData["rotation"] = { object.GetRotation().x, object.GetRotation().y };
        gameObjectData["id"] = object.GetId();
        gameObjectData["tint"] = { object.GetTint().Value.x, object.GetTint().Value.y, object.GetTint().Value.z, object.GetTint().Value.w };
        gameObjectData["z_order"] = object.GetZOrder();

        // Save behaviours
        json behavioursData;
        for (Behaviour* behaviour : object.GetBehaviours())
        {
            json behaviourData;
            behaviourData["gameObjectId"] = behaviour->GetGameObject()->GetId();
            behaviourData["name"] = behaviour->GetName();
            behavioursData.push_back(behaviourData);
        }
        gameObjectData["behaviours"] = behavioursData;

        // Add game object data to scene data
        sceneData["game_objects"].push_back(gameObjectData);
    }

    //std::cout << sceneData.dump(4) << std::endl;
    // 
    // Write JSON data to file
    //ConsoleLogger::InfoLog("Saved scene at " + scene->GetPath().string());

    std::string formattedPath = scene->GetPath().string();
    std::size_t found = formattedPath.find('\\');
    while (found != std::string::npos) {
        formattedPath.replace(found, 1, "\\\\");
        found = formattedPath.find('\\', found + 2);
    }
    formattedPath.erase(std::remove_if(formattedPath.begin(), formattedPath.end(), [](char c) {
        return !std::isprint(static_cast<unsigned char>(c));
        }), formattedPath.end());

    std::ofstream file(formattedPath);
    file << std::setw(4) << sceneData << std::endl;
    file.close();

    // Check if file was written to successfully
    if (file.fail() && file.bad())
    {
        if (file.eof())
        {
            ConsoleLogger::ErrorLog("End of file reached while saving the scene '" + scene->GetPath().stem().string() + "' at the path '" + scene->GetPath().string() + "'");
        }
        else if (file.fail())
        {
            ConsoleLogger::ErrorLog("The scene '" + scene->GetPath().stem().string() + "' at the path '" + scene->GetPath().string() + "' failed to open. The file can't be found or you have invalid permissions.");
        }
        else if (file.bad())
        {
            char errorMessage[256];
            strerror_s(errorMessage, sizeof(errorMessage), errno);
            ConsoleLogger::WarningLog("The scene '" + scene->GetPath().stem().string() + "' failed to save. Error: " + std::string(errorMessage));
        }
        else if (file.is_open())
        {
            ConsoleLogger::ErrorLog("The scene '" + scene->GetPath().string() + "' failed to save because it is open in another program");
        }
        else
        {
            char errorMessage[256];
            strerror_s(errorMessage, sizeof(errorMessage), errno);
            ConsoleLogger::WarningLog("The scene '" + scene->GetPath().stem().string() + "' failed to save. Error: " + std::string(errorMessage));
        }
        return;
    }
    {
        ConsoleLogger::InfoLog("The scene '" + scene->GetPath().stem().string() + "' has been saved");
    }
}

bool SceneManager::LoadScene(const std::filesystem::path& filePath)
{
    if (!std::filesystem::exists(filePath) || filePath.extension() != ".json")
    {
        ConsoleLogger::WarningLog("The path for the scene '" + filePath.stem().string() + "' is invalid, '" + filePath.string() + "'");
        return false;
    }

    std::string filePathString = filePath.string();
    filePathString.erase(std::remove_if(filePathString.begin(), filePathString.end(),
        [](char c) { return !std::isprint(c); }), filePathString.end());

    // Load file into string
    std::ifstream file(filePathString);
    if (!file.is_open()) {
        ConsoleLogger::WarningLog("Can not open the scene '" + filePath.stem().string() + "' at the path '" + filePath.string() + "'");
        return false;
    }
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    file.close();

    //ConsoleLogger::InfoLog(fileStream.str());

    // Parse JSON data
    json sceneData = json::parse(fileStream.str());

    // Create new scene
    Scene scene = Scene(filePath, {});

    // Create game objects
    for (const auto& gameObjectData : sceneData["game_objects"])
    {
        // Create new game object
        GameObject gameObject = GameObject(gameObjectData["id"]);
        std::string objectName = gameObjectData["name"];

        // Load texture
        if (gameObjectData.find("texture_path") != gameObjectData.end()) {
            std::filesystem::path texturePath = gameObjectData["texture_path"];
            gameObject.SetPath(texturePath.string());
            SDL_Texture* texture = nullptr;
            SDL_Surface* surface = IMG_Load(texturePath.string().c_str());
            if (surface == nullptr)
            {
                std::string warning = "Failed to load the texture for the game object '" + objectName;
                std::string warning2 = "'. Error: ";
                warning.append(warning2);
                warning.append(IMG_GetError());
                ConsoleLogger::ErrorLog(warning);
            }
            else if (surface->w == 0 || surface->h == 0)
            {
                SDL_FreeSurface(surface);
                std::string warning = "Failed to load the texture for the game object '" + objectName;
                std::string warning2 = "' due to the width or height being 0";
                warning.append(warning2);
                ConsoleLogger::ErrorLog(warning);
            }
            else
            {
                texture = SDL_CreateTextureFromSurface(PhantomEditor::EditorWindow::GetRenderer(), surface);
                if (texture == nullptr)
                {
                    std::string warning = "Failed to create texture for the object '" + objectName;
                    std::string warning2 = "'. Error: ";
                    warning.append(warning2);
                    warning.append(SDL_GetError());
                    ConsoleLogger::ErrorLog(warning);
                }
                SDL_FreeSurface(surface);
                gameObject.SetTexture(texture);
            }
        }
        else
        {
            ConsoleLogger::ErrorLog("There was an error finding the texture for the object: " + objectName);
            continue;
        }

        // Load name, position, real size, size, rotation, id, tint, zOrder
        gameObject.SetName(gameObjectData["name"]);
        gameObject.SetPosition(ImVec2(gameObjectData["position"][0], gameObjectData["position"][1]));
        gameObject.SetRealSize(ImVec2(gameObjectData["real_size"][0], gameObjectData["real_size"][1]));
        gameObject.SetSize(ImVec2(gameObjectData["size"][0], gameObjectData["size"][1]));
        gameObject.SetRotation(ImVec2(gameObjectData["rotation"][0], gameObjectData["rotation"][1]));
        //gameObject.SetTint(ImColor(gameObjectData["tint"][0], gameObjectData["tint"][1], gameObjectData["tint"][2], gameObjectData["tint"][3]));
        //gameObject.SetTint(ImColor((int)gameObjectData["tint"][0], (int)gameObjectData["tint"][1], (int)gameObjectData["tint"][2], (int)gameObjectData["tint"][3]));
        gameObject.SetTint(ImVec4(gameObjectData["tint"][0], gameObjectData["tint"][1], gameObjectData["tint"][2], gameObjectData["tint"][3]));
        gameObject.SetZOrder(gameObjectData["z_order"]);

        // Add game object to scene
        scene.AddGameObject(gameObject);

        // Load behaviours
        for (const auto& behaviourData : gameObjectData["behaviours"])
        {
            Behaviour* behaviour = BehaviourFactory::CreateBehaviour(behaviourData);
            if (behaviour != nullptr) scene.GetGameObjects().back().AddBehaviour(behaviour);
        }
        //ConsoleLogger::InfoLog("Count: " + std::to_string(scene.GetGameObjects().back().GetBehaviours().size()));
    }
    bool sceneFound = false;
    for (Scene& scenes : m_scenes) {
        if (scenes.GetPath() == scene.GetPath()) {
            SetActiveScene(&scenes);
            sceneFound = true;
        }
    }
    if (!sceneFound)
    {
        AddScene(scene);
        SetActiveScene(&GetScenes()->back());
    }
    ConsoleLogger::InfoLog("The scene '" + filePath.stem().string() + "' has been loaded");
    return true;
}

void SceneManager::AddScene(Scene scene) {
    m_scenes.push_back(scene);
}

void SceneManager::ResetScene(Scene* scene) {
    //for (GameObject& obj : scene->GetGameObjects())
    //{
    //    scene->RemoveGameObject(&obj);
    //}
    scene->GetGameObjects().clear();
    for (GameObject obj : scene->gameObjectsBackup)
    {
        scene->AddGameObject(obj);
    }
}

void SceneManager::BackupScene(Scene* scene) {
    scene->gameObjectsBackup.clear();
    for (GameObject obj : scene->GetGameObjects())
    {
        scene->gameObjectsBackup.push_back(obj);
    }
}

