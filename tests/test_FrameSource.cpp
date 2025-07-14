#include <gtest/gtest.h>
#include "../src/framesource/DiskFrameSource.h"
#include "../src/utils/CamSimLogger.h"
#include <fstream>
#include <filesystem>
#include <regex>

class DiskFrameSourceTest : public ::testing::Test 
{
protected:
    std::string testDir = "test_data/frame_source";

    void SetUp() override {
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        std::filesystem::remove_all(testDir);
    }

    void CreateFiles(const std::vector<std::string>& filenames) {
        for (const auto& file : filenames) {
            std::ofstream(testDir + "/" + file).close();
        }
    }

    void InitializeDiskFrameSource(DiskFrameSource& framesource, int width, int height, IFrameFormat fmt, int fps, bool loop)
    {
        std::string dataDir = "./../data";
        CamSimStatusType result = framesource.IFrameSource_Init(dataDir, width, height, fmt, fps, loop);
        EXPECT_EQ(result, CamSimStatusType::E_STATUS_SUCCESS);
    }
};

/**
 * @brief Disk Frame Source Initialization Test:
 *         with valid video files.
 */
TEST_F(DiskFrameSourceTest, InitWithValidVideoFiles) {
    CAMSIMLOG_INFO("DiskFrameSourceTest::InitWithValidVideoFiles");
    CreateFiles({"v1.mp4", "v2.avi", "notes.txt", "clip.mkv"});

    DiskFrameSource source;
    CamSimStatusType result = source.IFrameSource_Init(testDir, 640, 480, IFrameFormat::E_RGB, 30, false);

    EXPECT_EQ(result, CamSimStatusType::E_STATUS_SUCCESS);
    EXPECT_EQ(source.GetFilePaths().size(), 3);
}

/**
 * @brief Disk Frame Source Initialization Test: 
 *           with no files
 */
TEST_F(DiskFrameSourceTest, InitWithNoFiles) {
    CAMSIMLOG_INFO("DiskFrameSourceTest::InitWithNoFiles");
    DiskFrameSource source;
    CamSimStatusType result = source.IFrameSource_Init(testDir, 320, 240, IFrameFormat::E_GRAY8, 15, true);

    EXPECT_EQ(result, CamSimStatusType::E_STATUS_FAILURE);
    EXPECT_TRUE(source.GetFilePaths().empty());
}

/**
 * @brief Disk Frame Source Initialization Test: 
 *          Initialization with invalid directory
 */
TEST(DiskFrameSourceStandaloneTest, InitWithInvalidDirectory) {
    CAMSIMLOG_INFO("DiskFrameSourceTest::InitWithInvalidDirectory");
    std::string invalidPath = "nonexistent_directory";
    DiskFrameSource source;

    CamSimStatusType result = source.IFrameSource_Init(invalidPath, 640, 480, IFrameFormat::E_UYVY, 25, false);
    EXPECT_EQ(result, CamSimStatusType::E_STATUS_FAILURE);
    EXPECT_TRUE(source.GetFilePaths().empty());
}

/**
 * @brief Disk Frame Source Initialization Test: 
 *          Frame Buffer Memory Check
 */
TEST_F(DiskFrameSourceTest, FrameBufferAllocatedProperly) {
    CAMSIMLOG_INFO("DiskFrameSourceTest::FrameBufferAllocatedProperly");
    CreateFiles({"video.mp4"});
    DiskFrameSource source;

    source.IFrameSource_Init(testDir, 128, 64, IFrameFormat::E_GRAY16, 10, false);

    IFrame* frame = source.GetCurrentFrame(); 
    ASSERT_NE(frame, nullptr);
    EXPECT_EQ(frame->width, 128);
    EXPECT_EQ(frame->height, 64);
    EXPECT_EQ(frame->frameSize, 128 * 64 * 2);
    EXPECT_NE(frame->dataOwner, nullptr);
}

/**
 * @brief Disk Frame Source Initialization Test: 
 *          Prevent reinitialization if m_pCurrentFrame already set
 */
TEST_F(DiskFrameSourceTest, InitTwiceWithoutReset) {
    CAMSIMLOG_INFO("DiskFrameSourceTest::InitTwiceWithoutReset");
    CreateFiles({"a.mp4"});
    DiskFrameSource source;

    source.IFrameSource_Init(testDir, 128, 128, IFrameFormat::E_BGR, 30, true);
    CamSimStatusType result = source.IFrameSource_Init(testDir, 256, 256, IFrameFormat::E_RGB, 60, false);
    
    EXPECT_EQ(result, CamSimStatusType::E_STATUS_FAILURE);
}

/**
 * @brief Disk Frame Source Start Test:
 *          Start should return success always provided initialization is successful.
 */
TEST_F(DiskFrameSourceTest, StartShouldReturnSuccess) {
    DiskFrameSource framesource;
    InitializeDiskFrameSource(framesource, 1280, 720, IFrameFormat::E_RGB, 60, false);
    CamSimStatusType status = framesource.IFrameSource_Start();
    EXPECT_EQ(status, CamSimStatusType::E_STATUS_SUCCESS);
}

/**
 * @brief Disk Frame Source Start Test:
 *          Get Next Frame shall Returns Valid Frame.
 */
TEST_F(DiskFrameSourceTest, GetNextFrameReturnsValidFrame) {

    DiskFrameSource framesource;
    InitializeDiskFrameSource(framesource, 1280, 720, IFrameFormat::E_RGB, 60, false);
    CamSimStatusType status = framesource.IFrameSource_Start();
    EXPECT_EQ(status, CamSimStatusType::E_STATUS_SUCCESS);

    IFrame frame;
    status = framesource.IFrameSource_GetNextFrame(frame);

    EXPECT_EQ(status, CamSimStatusType::E_STATUS_SUCCESS);
    EXPECT_NE(frame.data, nullptr);
    EXPECT_GT(frame.frameSize, 0);
}

/**
 * @brief Disk Frame Source Start Test:
 *          Stop shall always return success.
 */
TEST_F(DiskFrameSourceTest, StopShouldReturnSuccess) {
    DiskFrameSource framesource;
    InitializeDiskFrameSource(framesource, 1280, 720, IFrameFormat::E_RGB, 60, false);
    CamSimStatusType status = framesource.IFrameSource_Start();
    status = framesource.IFrameSource_Stop();
    EXPECT_EQ(status, CamSimStatusType::E_STATUS_SUCCESS);
}

/**
 * @brief Disk Frame Source Start Test:
 *          Get Next Frame Without Start Shall return failure.
 */
TEST_F(DiskFrameSourceTest, GetNextFrameWithoutStartReturnsSuccess) {
    DiskFrameSource framesource;
    InitializeDiskFrameSource(framesource, 1280, 720, IFrameFormat::E_RGB, 60, false);
    IFrame frame;
    CamSimStatusType status = framesource.IFrameSource_GetNextFrame(frame);
    EXPECT_EQ(status, CamSimStatusType::E_STATUS_FAILURE);  // or FAILURE based on your logic
}

/**
 * @brief Disk Frame Source Start Test:
 *          Stop without Start shall also return success. 
 *          Stop should be able to gracefully handle all uninitialized member variables.
 */
TEST_F(DiskFrameSourceTest, StopWithoutStartStillReturnsSuccess) {
    DiskFrameSource framesource;
    InitializeDiskFrameSource(framesource, 1280, 720, IFrameFormat::E_RGB, 60, false);
    CamSimStatusType status = framesource.IFrameSource_Stop();
    EXPECT_EQ(status, CamSimStatusType::E_STATUS_SUCCESS);  // or handled gracefully
}
