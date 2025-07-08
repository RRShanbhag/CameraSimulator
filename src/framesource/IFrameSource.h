#pragma once

#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include "../utils/CamSimCommons.h"
enum class IFrameFormat 
{ 
    E_UNKNOWN = -1,
    E_GRAY8 = 0,
    E_GRAY16, 
    E_RGB,
    E_BGR,
    E_UYVY 
};

struct IFrame {
    std::unique_ptr<unsigned char[]> dataOwner = nullptr;
    unsigned char* data = nullptr;
    int width = 0;
    int height = 0;
    IFrameFormat format = IFrameFormat::E_UNKNOWN;
    std::chrono::steady_clock::time_point timestamp;
    int frameSize = 0;

    ~IFrame()
    {
        data = nullptr;
        width = 0;
        height = 0;
        format = IFrameFormat::E_UNKNOWN;
        frameSize = 0;
    }

    void ShallowCopy(IFrame& frame)
    {
        data = frame.dataOwner.get();
        width = frame.width;
        height = frame.height;
        format = frame.format;
        frameSize = frame.frameSize;
        timestamp = frame.timestamp;
    }
};

class IFrameSource {

protected:
    IFrame* m_pCurrentFrame = nullptr;
    bool m_bIsIFrameStarted = false;

public:
    virtual ~IFrameSource() = default;

    virtual CamSimStatusType IFrameSource_Init(std::string location, int width, int height, IFrameFormat format, int fps, bool loop) = 0;
    virtual CamSimStatusType IFrameSource_GetNextFrame(IFrame& frame) = 0;

    virtual CamSimStatusType IFrameSource_Start() = 0;
    virtual CamSimStatusType IFrameSource_Stop() = 0;
};
