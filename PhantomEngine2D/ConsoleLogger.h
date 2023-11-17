#pragma once

#include <string>

class ConsoleLogger
{
public:
    static void InfoLog(const std::string& message);
    static void WarningLog(const std::string& message);
    static void ErrorLog(const std::string& message);

    static enum class ConsoleLogType {
        WARNING,
        INFO,
        ERROR_
    };
    static struct ConsoleLog {
        std::string message;
        ConsoleLogType type;
    };
};
