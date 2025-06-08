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

struct Frame {
    unsigned char* data = nullptr;
    int width = 0;
    int height = 0;
    IFrameFormat format = IFrameFormat::E_UNKNOWN;
    std::chrono::steady_clock::time_point timestamp;
};

class IFrameSource {
public:
    virtual ~IFrameSource() = default;

    virtual CamSimStatusType IFrameSource_Init(int width, int height, IFrameFormat format, int fps, bool loop) = 0;
    virtual CamSimStatusType IFrameSource_GetNextFrame(Frame& frame) = 0;

    virtual CamSimStatusType IFrameSource_Start() = 0;
    virtual CamSimStatusType IFrameSource_Stop() = 0;
};
