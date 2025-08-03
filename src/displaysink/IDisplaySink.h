#pragma once

#include "../utils/CamSimCommons.h"


class IDisplaySink {
protected:
    Frame* m_pCurrentFrame = nullptr;
    bool m_bIsDisplaySinkStarted = false;
    CamSimErrorType m_eError = CamSimErrorType::E_ERRORCODE_NOERROR;
public:
    virtual ~IDisplaySink() = default;

    virtual CamSimStatusType DisplaySink_Init(int width, int height, FrameFormat format, int fps) = 0;
    virtual CamSimStatusType DisplaySink_DisplayNextFrame(Frame& frame) = 0;

    virtual CamSimStatusType DisplaySink_Start() = 0;
    virtual CamSimStatusType DisplaySink_Stop() = 0;

    const CamSimErrorType DisplaySink_GetError()
    {
        return m_eError;
    }
};
