#include "FrameSourceRunner.h"

CamSimStatusType FrameSource_Run(std::shared_ptr<IFrameSource> src, std::function<void(IFrame)> frameCallback)
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;

    return eStatus;
}

CamSimStatusType FrameSource_Stop()
{
    CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;

    return eStatus;
}