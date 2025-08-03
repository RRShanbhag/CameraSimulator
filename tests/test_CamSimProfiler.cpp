#include <gtest/gtest.h>
#include <thread>
#include "../src/utils/CamSimCommons.h"
#include "../src/utils/CamSimLogger.h"
#include "../src/profiler/ProfilerMetrics.h"

/**
 * @brief Stage Creation Test:
 *         with fake stages.
 */
TEST(CamSimProflerTest, CreateStageTest) {
    CAMSIMLOG_INFO("CamSimProflerTest::CreateStageTest");
    camsim::profiler::StageLabel testLabel = {"TestStage", "TestThread"};

    camsim::profiler::StageMetrics stageMetric(testLabel);

    EXPECT_EQ(testLabel.moduleName, stageMetric.GetStageLabel().moduleName);
    EXPECT_EQ(testLabel.threadName, stageMetric.GetStageLabel().threadName);
}

/**
 * @brief Stage Duration Test:
 *         with fake stages.
 */
TEST(CamSimProflerTest, StageDurationTest) {
    CAMSIMLOG_INFO("CamSimProflerTest::StageDurationTest");
    camsim::profiler::StageLabel testLabel = {"TestStage", "TestThread"};
    camsim::profiler::StageMetrics stageMetric(testLabel);
    
    auto start = std::chrono::steady_clock::now();
    auto stop = start + std::chrono::milliseconds(10);   

    camsim::profiler::FrameMetrics newframeMetrics; 
    newframeMetrics.frameID = 10; 
    newframeMetrics.stages.emplace(std::pair(testLabel, camsim::profiler::StageTiming(start, stop)));

    stageMetric.UpdateStageMetric(newframeMetrics);
    EXPECT_EQ(stageMetric.GetFramesCount(), 1);
    EXPECT_NEAR(stageMetric.GetStageDuration(newframeMetrics.frameID).count(), 10 * ONE_MILLISECOND_IN_us, 2 * ONE_MILLISECOND_IN_us);
}

/**
 * @brief Stage Metric Test:
 *         with fake stages. Average and Total
 */
TEST(CamSimProflerTest, StageMetricTest) {
    CAMSIMLOG_INFO("CamSimProflerTest::StageMetricTest::AverageAndTotal");
    camsim::profiler::StageLabel testLabel = {"TestStage", "TestThread"};
    camsim::profiler::StageMetrics stageMetric(testLabel);
    
    uint64_t testFrameCount = 10;
    std::chrono::microseconds totalTestDuration(0);
    for(uint64_t i = 1; i < testFrameCount; i++)
    {
        auto start = std::chrono::steady_clock::now();
        auto stop = start + std::chrono::seconds(i);
        
        camsim::profiler::FrameMetrics newframeMetrics; 
        newframeMetrics.frameID = i; 
        newframeMetrics.stages.emplace(std::pair(testLabel, camsim::profiler::StageTiming(start, stop)));
        stageMetric.UpdateStageMetric(newframeMetrics);
        totalTestDuration += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::seconds(i));
    }
    EXPECT_EQ(stageMetric.GetFramesCount(), testFrameCount-1);
    EXPECT_NEAR(stageMetric.GetStageDuration(testFrameCount-1).count(), std::chrono::microseconds((testFrameCount-1)* ONE_SECOND_IN_us).count(), 2 * ONE_MILLISECOND_IN_us);
    EXPECT_NEAR(stageMetric.GetStageDuration(testFrameCount-2).count(), std::chrono::microseconds((testFrameCount-2)* ONE_SECOND_IN_us).count(), 2 * ONE_MILLISECOND_IN_us);
    EXPECT_NEAR(stageMetric.GetStageDuration(testFrameCount-3).count(), std::chrono::microseconds((testFrameCount-3)* ONE_SECOND_IN_us).count(), 2 * ONE_MILLISECOND_IN_us);
    EXPECT_NEAR(stageMetric.GetTotalStageDuration().count(), totalTestDuration.count(), 10 * ONE_MILLISECOND_IN_us);
    EXPECT_NEAR(stageMetric.GetAverageStageDuration().count(), (totalTestDuration/(testFrameCount-1)).count(), 2 * ONE_MILLISECOND_IN_us);  
}