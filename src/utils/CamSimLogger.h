#pragma once

#include <string>
#include <fmt/core.h>

enum class CamSimLogLevel { INFO, WARN, ERROR };

class CamSimLogger {
public:
    template<typename... Args>
    static void logMessage(CamSimLogLevel level, const std::string& formatStr, Args&&... args,
                    const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) {
        std::string message = fmt::format(formatStr, std::forward<Args>(args)...);
        writeLog(level, message, file, function, line);
    }

private:
    static void writeLog(CamSimLogLevel level, const std::string& message,
                         const char* file, const char* function, int line);
    static std::string getTimestamp();
    static std::string getLogFileName();
    static std::string logLevelToString(CamSimLogLevel level);
    static void ensureLogDirectoryExists();
};

// Macros to auto-fill file/function/line
#define CAMSIMLOG_INFO(fmt, ...)  CamSimLogger::logMessage(CamSimLogLevel::INFO,  fmt, ##__VA_ARGS__)
#define CAMSIMLOG_WARN(fmt, ...)  CamSimLogger::logMessage(CamSimLogLevel::WARN,  fmt, ##__VA_ARGS__)
#define CAMSIMLOG_ERROR(fmt, ...) CamSimLogger::logMessage(CamSimLogLevel::ERROR, fmt, ##__VA_ARGS__)
