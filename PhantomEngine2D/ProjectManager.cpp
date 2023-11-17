#include "ProjectManager.h"
#include "PhantomEngine2D.h"
#include "ConsoleLogger.h"
#include "EditorWindow.h"
#include <json.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <variant>
#include "imgui.h"
#include <imgui_internal.h>
#include "Scenes/SceneManager.h"

std::filesystem::path ProjectManager::CreateProject(std::string projectName)
{
    ConsoleLogger::InfoLog("Project '" + projectName + "' is being created");

    char* userProfile;
    size_t len;
    _dupenv_s(&userProfile, &len, "USERPROFILE");

    std::filesystem::path documentsPath = std::filesystem::path(userProfile) / "Documents";
    std::filesystem::path phantomEnginePath = documentsPath / "Phantom Engine";
    std::filesystem::path projectsPath = phantomEnginePath / "Projects";
    std::filesystem::path projectPath = projectsPath / projectName.c_str();

    if (!std::filesystem::exists(phantomEnginePath)) std::filesystem::create_directory(phantomEnginePath);
    if (!std::filesystem::exists(projectsPath)) std::filesystem::create_directory(projectsPath);

    if (std::filesystem::exists(projectPath)) {
        ConsoleLogger::WarningLog("Project '" + projectName + "' already exists, cancelling creation");
        return std::filesystem::path(); // Empty path
    }
    else std::filesystem::create_directory(projectPath);
    std::filesystem::create_directory(projectPath / "Assets");
    std::filesystem::create_directory(projectPath / "Assets" / "Scenes");
    std::filesystem::create_directory(projectPath / "Assets" / "Sprites");
    std::filesystem::create_directory(projectPath / "Assets" / "Behaviours");
    std::filesystem::create_directory(projectPath / "Plugins");
    std::filesystem::create_directory(projectPath / "Settings");

    nlohmann::json projectSettings = {
        {"phantomEngineVersion", 0.1},
        {"name", projectName}
    };

    std::ofstream settingsFile(projectPath / "Settings" / "Settings.json");
    settingsFile << projectSettings;
    settingsFile.close();

    nlohmann::json defaultScene = {};
    std::ofstream sceneFile(projectPath / "Assets" / "Scenes" / "Default.json");
    sceneFile << defaultScene;
    sceneFile.close();

    // Copies preset behaviours into Scripts folder
    std::filesystem::path presetBehavioursPath = std::filesystem::current_path() / "PresetBehaviours";
    std::filesystem::path projectPresetBehavioursPath = projectPath / "Assets" / "Behaviours";
    for (const auto& entry : std::filesystem::directory_iterator(presetBehavioursPath))
    {
        if (entry.is_regular_file())
        {
            std::filesystem::path destPath = projectPresetBehavioursPath / entry.path().filename();
            std::filesystem::copy(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
        }
    }

    ConsoleLogger::InfoLog("Project '" + projectName + "' has been created");
    return projectPath;
}

bool ProjectManager::OpenProject(std::variant<std::string, std::filesystem::path> project)
{
    std::filesystem::path projectPath = "";
    if (std::holds_alternative<std::string>(project)) {         // handle opening project by name
        std::string projectName = std::get<std::string>(project);
        char* userProfile;
        size_t len;
        _dupenv_s(&userProfile, &len, "USERPROFILE");
        projectPath = std::filesystem::path(userProfile) / "Documents" / "Phantom Engine" / "Projects" / projectName;
        if (!std::filesystem::exists(projectPath)) { 
            ConsoleLogger::WarningLog("Project at '" + projectPath.string() + "' not found");
            return false;
        }
    }
    else if (std::holds_alternative<std::filesystem::path>(project)) {          // handle opening project by path
        projectPath = std::get<std::filesystem::path>(project);
        if (!std::filesystem::exists(projectPath / "Settings" / "Settings.json")) return false;
    }
    else {
        ConsoleLogger::ErrorLog("You have set an incorrect path or project name. If using project name, the project must be in the '/Phantom Engine/Projects' folder");
        return false;
    }
    CleanupSDL();
    PhantomEditor::EditorWindow editorWindow;
    editorWindow.InitSDL();
    if (!SceneManager::LoadScene(projectPath / "Assets" / "Scenes" / "Default.json")) return false;
    //SceneManager::AddScene(scene);
    //SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());
    editorWindow.Init(projectPath);
    ConsoleLogger::InfoLog("Project has successfully opened");
    return true;
}

bool ProjectManager::SaveProject(std::filesystem::path projectPath)
{
    return false;
}

bool ProjectManager::CloseProject(std::filesystem::path projectPath)
{
    return false;
}

bool ProjectManager::GetProjectName(std::filesystem::path projectPath)
{
    return false;
}

bool ProjectManager::GetProjectSettings(std::filesystem::path projectPath)
{
    return false;
}
bool ProjectManager::SetProjectSettings(std::filesystem::path projectPath)
{
    return false;
}
