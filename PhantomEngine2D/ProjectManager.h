#pragma once

#include <json.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <variant>

class ProjectManager {
public:
    static std::filesystem::path CreateProject(std::string projectName);
    static bool OpenProject(std::variant<std::string, std::filesystem::path> project);
    static bool SaveProject(std::filesystem::path projectPath);
    static bool CloseProject(std::filesystem::path projectPath);
    static bool GetProjectName(std::filesystem::path projectPath);
    static bool GetProjectSettings(std::filesystem::path projectPath);
    static bool SetProjectSettings(std::filesystem::path projectPath);
};
