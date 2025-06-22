#pragma once
#include <string>
#include <stdlib.h>

enum class CamSimErrorType : int
{
    E_ERRORCODE_NOERROR,
    E_ERRORCODE_MEMORY_ALLOC_FAILED,
    E_ERRORCODE_INITIALIZATION_FAILED,
    E_ERRORCODE_UNKNOWN_FORMAT,
    E_ERRORCODE_INVALID_FILE_OR_DIRECTORY,
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