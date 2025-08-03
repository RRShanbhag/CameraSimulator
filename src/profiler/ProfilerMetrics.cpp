#include "ProfilerMetrics.h"

namespace camsim::profiler
{

StageMetrics::StageMetrics(const StageLabel& stageLabel)
    : m_sStageLabel(stageLabel)
{

}
// increment frame counts in the current stage
void StageMetrics::IncrementFrameCount()
{
    m_nFrameCount++;
}

// calculates execution Time of each frame in the current stage 
std::chrono::microseconds StageMetrics::CalculateExecutionDuration(const std::chrono::time_point<std::chrono::steady_clock>& startTime, 
                            const std::chrono::time_point<std::chrono::steady_clock>& endTime)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
}

//Increments frame counter, execution time and stage label in frameMetric.
void StageMetrics::UpdateStageMetric(FrameMetrics& frameMetric)
{
    std::unordered_map<StageLabel, StageTiming, StageLabelHash>::iterator it = frameMetric.stages.find(m_sStageLabel);
    if(it != frameMetric.stages.end())
    {
        IncrementFrameCount();
        m_vStageDuration[frameMetric.frameID] = CalculateExecutionDuration(it->second.frameStart, it->second.frameStop);
        m_vFrameIDs.push_back(frameMetric.frameID);
    }
}

// returns the stage label.
const StageLabel& StageMetrics::GetStageLabel()
{
    return m_sStageLabel;
}

// return frame counts of this stage
const uint64_t& StageMetrics::GetFramesCount()
{
    return m_nFrameCount;
}

// return execution duration of a particular frame
const std::chrono::microseconds StageMetrics::GetStageDuration(int64_t frameID)
{
    auto it = m_vStageDuration.find(frameID);
    if(it != m_vStageDuration.end())
        return m_vStageDuration[frameID];
    return std::chrono::microseconds(0);
}

// return total execution duration
const std::chrono::microseconds StageMetrics::GetTotalStageDuration()
{
    std::chrono::microseconds totalduration(0);
    for(auto iter = m_vStageDuration.begin() ; iter != m_vStageDuration.end(); iter++)
    {
        totalduration += iter->second;
    }
    return totalduration;
}

// return average execution duration
const std::chrono::microseconds StageMetrics::GetAverageStageDuration()
{
    std::chrono::microseconds totalduration(0);
    for(auto iter = m_vStageDuration.begin() ; iter != m_vStageDuration.end(); iter++)
    {
        totalduration += iter->second;
    }
    return totalduration/m_nFrameCount;
}
}