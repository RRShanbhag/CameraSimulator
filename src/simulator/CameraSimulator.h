#pragma once

#include "../utils/CamSimCommons.h"
class CameraSimulator {
public:
    IFrameSourceType m_eSourceType = IFrameSourceType::E_FRMSRC_UNKNOWN;
public:
    bool CameraSimulator_Initialize(IFrameSourceType eSourceType);
    void CameraSimulator_Run();
};