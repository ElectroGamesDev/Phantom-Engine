#pragma once

#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>
#include "ConsoleLogger.h"

namespace fs = std::filesystem;

//extern std::string cscPath = "C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe";

void CompileScripts(const fs::path& parentPath, const fs::path& outputPath);
