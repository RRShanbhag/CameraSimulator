#pragma once

#include <vector>
#include <cstdint>
#include <chrono>
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
    unsigned char* data = nullptr;
    int width = 0;
    int height = 0;
    IFrameFormat format = IFrameFormat::E_UNKNOWN;
    std::chrono::steady_clock::time_point timestamp;
    int frameSize = 0;

    ~IFrame()
    {
        if(data != nullptr)
            delete[] data;
        data = nullptr;
        width = 0;
        height = 0;
        format = IFrameFormat::E_UNKNOWN;
        frameSize = 0;
    }
};

class IFrameSource {

protected:
    IFrame* m_pCurrentFrame = nullptr;

public:
    virtual ~IFrameSource() = default;

    virtual CamSimStatusType IFrameSource_Init(std::string location, int width, int height, IFrameFormat format, int fps, bool loop) = 0;
    virtual CamSimStatusType IFrameSource_GetNextFrame(IFrame& frame) = 0;

    virtual CamSimStatusType IFrameSource_Start() = 0;
    virtual CamSimStatusType IFrameSource_Stop() = 0;
};
