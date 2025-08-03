#pragma once

#include "../utils/CamSimCommons.h"
class CameraSimulator {
public:
    FrameSourceType m_eSourceType = FrameSourceType::E_FRMSRC_UNKNOWN;
public:
    bool CameraSimulator_Initialize(FrameSourceType eSourceType);
    void CameraSimulator_Run();
};