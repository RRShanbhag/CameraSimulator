#include <gtest/gtest.h>
#include "../src/utils/CamSimLogger.h"
#include <fstream>
#include <filesystem>
#include <regex>

TEST(CamSimLoggerTest, WritesLogFileWithInfo) {
    std::string testMessage = "Logger test info message.";
    CAMSIMLOG_INFO(testMessage);

    std::string todayLog;
    {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm buf{};
        localtime_r(&now, &buf);
        std::ostringstream oss;
        oss << "logs/" << std::put_time(&buf, "%Y-%m-%d") << ".log";
        todayLog = oss.str();
    }

    ASSERT_TRUE(std::filesystem::exists(todayLog));

    std::ifstream logFile(todayLog);
    std::string line;
    bool found = false;
    while (std::getline(logFile, line)) {
        if (line.find(testMessage) != std::string::npos) {
            found = true;
            break;
        }
    }

    EXPECT_TRUE(found) << "Log message not found in log file.";
}
