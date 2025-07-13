#pragma once

#include <vector>
#include "../utils/CamSimCommons.h"

class IFrameSource {

protected:
    IFrame* m_pCurrentFrame = nullptr;
    bool m_bIsIFrameStarted = false;
    CamSimErrorType m_eError = CamSimErrorType::E_ERRORCODE_NOERROR;

public:
    virtual ~IFrameSource() = default;

    virtual CamSimStatusType IFrameSource_Init(std::string location, int width, int height, IFrameFormat format, int fps, bool loop) = 0;
    virtual CamSimStatusType IFrameSource_GetNextFrame(IFrame& frame) = 0;

    virtual CamSimStatusType IFrameSource_Start() = 0;
    virtual CamSimStatusType IFrameSource_Stop() = 0;

    CamSimErrorType IFrameSource_GetErrorStatus()
    {
        return m_eError;
    }
};
