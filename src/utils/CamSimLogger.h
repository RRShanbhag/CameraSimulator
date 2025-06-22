#pragma once

#include <string>
#include <iostream>
#include <cstring>
enum class CamSimLogLevel { INFO, WARN, ERROR };

// handy filename‐only macro:
#define __FILENAME__                                                        (strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__)

// The maximum size of any one log message
#ifndef CAMSIM_LOG_BUFFER_SIZE
  #define CAMSIM_LOG_BUFFER_SIZE 512
#endif

#define CAMSIMLOG(level, fmt, ...)                                          do{                                                                    \
                                                                                char _buf[CAMSIM_LOG_BUFFER_SIZE];                                 \
                                                                                if (sizeof(#__VA_ARGS__) > 1)                                      \
                                                                                  std::snprintf(_buf, sizeof(_buf), fmt, ##__VA_ARGS__);           \
                                                                                else                                                               \
                                                                                  std::snprintf(_buf, sizeof(_buf), "%s", fmt);                    \
                                                                                CamSimLogger::logRaw(level,                                        \
                                                                                                    __FILENAME__,                                  \
                                                                                                    __FUNCTION__,                                  \
                                                                                                    __LINE__,                                      \
                                                                                                    _buf);                                         \
                                                                            } while(0)

class CamSimLogger {
public:
    // New “raw” API: takes a fully formatted C‐string
    static void logRaw(CamSimLogLevel level,
                       const char* file,
                       const char* function,
                       int line,
                       const char* message);
    static std::string getTimestamp();
    static std::string getLogFileName();
    static std::string logLevelToString(CamSimLogLevel level);
    static void ensureLogDirectoryExists();
};

#define CAMSIMLOG_INFO(fmt, ...)                                CAMSIMLOG(CamSimLogLevel::INFO,  fmt, ##__VA_ARGS__)
#define CAMSIMLOG_WARN(fmt, ...)                                CAMSIMLOG(CamSimLogLevel::WARN,  fmt, ##__VA_ARGS__)
#define CAMSIMLOG_ERROR(fmt, ...)                               CAMSIMLOG(CamSimLogLevel::ERROR, fmt, ##__VA_ARGS__)
