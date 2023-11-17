#include "ConsoleLogger.h"
#include "EditorWindow.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

void ConsoleLogger::InfoLog(const std::string& message)
{
    std::cout << "[INFO] " << message << std::endl;
    PhantomEditor::EditorWindow::consoleLogs.push_back(ConsoleLog{ "[INFO] " + message, ConsoleLogType::INFO });
}

void ConsoleLogger::WarningLog(const std::string& message)
{
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 14); // Set color to yellow
#endif
    std::cout << "[WARNING] " << message << std::endl;
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, 15); // Reset color
#endif
    PhantomEditor::EditorWindow::consoleLogs.push_back(ConsoleLog{ "[WARNING] " + message, ConsoleLogType::WARNING });
}

void ConsoleLogger::ErrorLog(const std::string& message)
{
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 12); // Set color to red
#endif
    std::cerr << "[ERROR] " << message << std::endl;
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, 15); // Reset color
#endif
    PhantomEditor::EditorWindow::consoleLogs.push_back(ConsoleLog{ "[ERROR] " + message, ConsoleLogType::ERROR_ });
}
