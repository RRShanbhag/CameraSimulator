#pragma once
#include <memory>
#include <thread>
#include <functional>
#include <atomic>
#include "IFrameSource.h"

class FrameSourceRunner 
{
    std::shared_ptr<IFrameSource> source = nullptr;
    std::atomic<bool> running = false;
    std::thread worker;
    CamSimErrorType m_eError = CamSimErrorType::E_ERRORCODE_NOERROR;

public:
    CamSimStatusType FrameSource_Run(std::shared_ptr<IFrameSource> src, std::function<void(IFrame)> frameCallback);
    CamSimStatusType FrameSource_Stop();
};