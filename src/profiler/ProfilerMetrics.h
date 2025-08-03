#pragma once

//Includes
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace camsim::profiler
{

typedef struct StageLabel
{
    std::string moduleName;
    std::string threadName;

    // Equality operator
    bool operator==(const StageLabel& other) const {
        return moduleName == other.moduleName && threadName == other.threadName;
    }

}StageLabel;

// Define a hash function for the struct
typedef struct StageLabelHash {
    std::size_t operator()(const StageLabel& k) const {
        return std::hash<std::string>()(k.moduleName) ^ (std::hash<std::string>()(k.threadName) << 1);
    }
}StageLabelHash;

typedef struct StageTiming
{
    std::chrono::steady_clock::time_point frameStart;
    std::chrono::steady_clock::time_point frameStop;

    StageTiming(const std::chrono::steady_clock::time_point& start, 
                const std::chrono::steady_clock::time_point& stop)
                : frameStart(start), frameStop(stop)
    {

    }
}StageTiming;

typedef struct
{
    uint64_t frameID = 0;
    std::unordered_map<StageLabel, StageTiming, StageLabelHash>stages;
}FrameMetrics;

class StageMetrics
{

private:
    uint64_t m_nFrameCount = 0;
    std::unordered_map<uint64_t, std::chrono::microseconds> m_vStageDuration;
    std::vector<uint64_t> m_vFrameIDs;
    StageLabel m_sStageLabel;
    //Later metrics CPUPercentage, Latency, etc.

    // increment frame counts in the current stage
    void IncrementFrameCount();
    // calculates execution Time of each frame in the current stage 
    std::chrono::microseconds CalculateExecutionDuration(const std::chrono::time_point<std::chrono::steady_clock>& startTime, 
                                const std::chrono::time_point<std::chrono::steady_clock>& endTime);

public:
    // Default Constructor
    StageMetrics(const StageLabel& stageLabel);
    
    //Increments frame counter, execution time and stage label in frameMetric.
    void UpdateStageMetric(FrameMetrics& frameMetric);

    // returns the stage label.
    const StageLabel& GetStageLabel();

    // return frame counts of this stage
    const uint64_t& GetFramesCount();

    // return execution duration of a particular frame
    const std::chrono::microseconds GetStageDuration(int64_t frameID);

    // return total execution duration
    const std::chrono::microseconds GetTotalStageDuration();

    // return average execution duration
    const std::chrono::microseconds GetAverageStageDuration();
};
} 

