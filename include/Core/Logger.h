#pragma once
#include <string>
#include <iostream>
#include <mutex>

namespace Core {

enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug
};

/**
 * @brief Thread-safe Logger.
 * 
 * Replaces direct std::cout usages for better debugging and future file output.
 */
class Logger {
public:
    static void Log(LogLevel level, const std::string& message);
    static void Info(const std::string& message);
    static void Warning(const std::string& message);
    static void Error(const std::string& message);

private:
    static std::mutex m_mutex;
};

} // namespace Core
