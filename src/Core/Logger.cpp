#include "Core/Logger.h"

namespace Core {

std::mutex Logger::m_mutex;

void Logger::Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    switch (level) {
        case LogLevel::Info:    std::cout << "[INFO] "; break;
        case LogLevel::Warning: std::cout << "[WARN] "; break;
        case LogLevel::Error:   std::cout << "[ERR]  "; break;
        case LogLevel::Debug:   std::cout << "[DEBUG] "; break;
    }
    
    std::cout << message << std::endl;
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::Info, message);
}

void Logger::Warning(const std::string& message) {
    Log(LogLevel::Warning, message);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::Error, message);
}

} // namespace Core
