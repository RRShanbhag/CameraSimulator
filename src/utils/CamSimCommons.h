#pragma once
#include <string>
#include <stdlib.h>
#include <cstdint>
#include <chrono>
#include <memory>

enum class CamSimErrorType : int
{
    E_ERRORCODE_NOERROR,
    E_ERRORCODE_MEMORY_ALLOC_FAILED,
    E_ERRORCODE_INITIALIZATION_FAILED,
    E_ERRORCODE_UNKNOWN_FORMAT,
    E_ERRORCODE_INVALID_FILE_OR_DIRECTORY,
    E_ERRORCODE_FRAME_GRABBING_FAILURE,
    E_ERRORCODE_TOTAL_ERRORS
};

enum class CamSimStatusType : int
{
    E_STATUS_SUCCESS,
    E_STATUS_FAILURE,
    E_STATUS_TOTAL
};

// Macro to stringify CamSimErrorType
#define CAMSIMERRORSTRING(err) [&] {                         \
    switch (err) {                                           \
        case CamSimErrorType::E_ERRORCODE_NOERROR: return "ERRORCODE_NOERROR"; \
        case CamSimErrorType::E_ERRORCODE_MEMORY_ALLOC_FAILED: return "ERRORCODE_MEMORY_ALLOC_FAILED"; \
        case CamSimErrorType::E_ERRORCODE_INITIALIZATION_FAILED: return "ERRORCODE_INITIALIZATION_FAILED"; \
        case CamSimErrorType::E_ERRORCODE_UNKNOWN_FORMAT: return "ERRORCODE_UNKNOWN_FORMAT"; \
        case CamSimErrorType::E_ERRORCODE_INVALID_FILE_OR_DIRECTORY: return "ERRORCODE_INVALID_FILE_OR_DIRECTORY"; \
        case CamSimErrorType::E_ERRORCODE_FRAME_GRABBING_FAILURE: return "ERRORCODE_FRAME_GRABBING_FAILURE"; \
        default: return "UNKNOWN_ERRORCODE";                 \
    }                                                        \
}()

// Macro to stringify CamSimStatusType
#define CAMSIMSTATUSSTRING(status) [&] {                     \
    switch (status) {                                        \
        case CamSimStatusType::E_STATUS_SUCCESS: return "STATUS_SUCCESS"; \
        case CamSimStatusType::E_STATUS_FAILURE: return "STATUS_FAILURE"; \
        default: return "UNKNOWN_STATUS";                    \
    }                                                        \
}()

#define MAX_FRAME_WIDTH                                         4096    //4K DCI Resolution Width
#define MAX_FRAME_HEIGHT                                        2160    //4K DCI Resolution Height
#define MAX_FRAME_CHANNELS                                      4       //ARGB channels???


enum class IFrameFormat 
{ 
    E_UNKNOWN = -1,
    E_GRAY8 = 0,
    E_GRAY16, 
    E_RGB,
    E_BGR,
    E_UYVY 
};

enum class IFrameSourceType : int
{
    E_FRMSRC_UNKNOWN = -1,
    E_FRMSRC_DISK_VIDEO = 0,
    E_FRMSRC_TYPES_TOTAL
};

typedef struct IFrame {
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
}IFrame;