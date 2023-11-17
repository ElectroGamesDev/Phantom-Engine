#include "ScriptCompiler.h"

std::string compilerPath = "C:\\Program Files\\dotnet\\sdk\\7.0.306\\Roslyn\\bincore\\csc.dll";
//std::string compilerPath = "C:\\Program Files\\Mono\\lib\\mono\\4.5\\mcs.exe";

void CompileScripts(const fs::path& parentPath, const fs::path& outputPath)
{
    ConsoleLogger::InfoLog("Starting script compilation");
    std::string scriptPaths = "";

    for (const auto& entry : std::filesystem::recursive_directory_iterator(parentPath)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".cs") continue;
        //ConsoleLogger::InfoLog("Found " + entry.path().string());
        scriptPaths += (" \"" + entry.path().string() + "\"");
    }

    if (scriptPaths == "") return;

    std::string mscorlibPath = "C:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319\\mscorlib.dll"; // Todo: get relative path

    //ConsoleLogger::InfoLog("Input: dotnet exec \"" + compilerPath + "\" /target:library /out:\"" + outputPath.string() + "\"/GameBehaviours.dll" + scriptPaths);
    int result = std::system(("dotnet exec \"" + compilerPath + "\" /target:library /out:\"" + outputPath.string() + "\"/GameScripts.dll" + " /reference:\"" + mscorlibPath + "\"" + scriptPaths).c_str());
    //ConsoleLogger::InfoLog("\"" + compilerPath + "\" /target:library /out:\"" + outputPath.string() + "\"/GameBehaviours.dll" + scriptPaths);
    //int result = std::system(("\"" + compilerPath + "\" /target:library /out:\"" + outputPath.string() + "\"/GameScripts.dll" + scriptPaths).c_str());

    if (result == 0) ConsoleLogger::InfoLog("Successfully compiled scripts");
    else ConsoleLogger::ErrorLog("Failed to compile scripts");
}
