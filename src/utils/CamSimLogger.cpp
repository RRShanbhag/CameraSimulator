#include "CamSimLogger.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

void CamSimLogger::writeLog(CamSimLogLevel level, const std::string& message,
                      const char* file, const char* function, int line) {
    ensureLogDirectoryExists();

    std::ofstream logFile(getLogFileName(), std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "[Logger] Failed to open log file.\n";
        return;
    }

    logFile << "[" << getTimestamp() << "] "
            << logLevelToString(level) << " "
            << "(" << file << ":" << function << ":" << line << ") "
            << message << "\n";
}

std::string CamSimLogger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm buf{};
    localtime_r(&time, &buf);
    std::ostringstream oss;
    oss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string CamSimLogger::getLogFileName() {
    auto now = std::chrono::system_clock::now();
    auto date = std::chrono::system_clock::to_time_t(now);
    std::tm buf{};
    localtime_r(&date, &buf);
    std::ostringstream oss;
    oss << "logs/" << std::put_time(&buf, "%Y-%m-%d") << ".log";
    return oss.str();
}

std::string CamSimLogger::logLevelToString(CamSimLogLevel level) {
    switch (level) {
        case CamSimLogLevel::INFO:  return "[INFO]";
        case CamSimLogLevel::WARN:  return "[WARN]";
        case CamSimLogLevel::ERROR: return "[ERROR]";
        default:              return "[UNKNOWN]";
    }
}

void CamSimLogger::ensureLogDirectoryExists() {
    std::filesystem::create_directories("logs");
}
