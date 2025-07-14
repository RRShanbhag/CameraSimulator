#include <gtest/gtest.h>
#include "../src/utils/CamSimLogger.h"
#include "../src/utils/CamSimCommons.h"
#include <fstream>
#include <filesystem>
#include <regex>

/**
 * @brief Checks if logging works on a new file. 
 * 
 */
TEST(CamSimLoggerTest, WritesLogFileWithInfo) {
    std::string testMessage = "Logger test info message.";
    CAMSIMLOG_INFO(testMessage.c_str());

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

TEST(CamSimLoggerTest, WritesReadRawFrameToFile)
{

    std::string FrameFileName = "logs/myTestRawFrameFile.raw";
    int frameSize = 1280*720*3;
    unsigned char* frameData = new unsigned char[frameSize]; 
    memset(frameData, 255, frameSize);
    int ret = CamSimLogger::WriteRawFrameToFile(FrameFileName.c_str(), frameSize, frameData);
    EXPECT_EQ(ret, 0);
    memset(frameData, 0, frameSize);
    ret = CamSimLogger::ReadRawFrameFromFile(FrameFileName.c_str(), 1, frameSize, frameData);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(frameData[0], 255);
    delete[] frameData;
    frameData = nullptr;

    ASSERT_TRUE(std::filesystem::exists(FrameFileName));
}
