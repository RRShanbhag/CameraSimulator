#pragma once
#include <memory>
#include <thread>
#include <functional>
#include <atomic>
#include "IFrameSource.h"

class FrameSourceRunner
{
private:
    std::shared_ptr<IFrameSource> m_source = nullptr;
    std::atomic<bool> m_running = false;
    std::thread m_worker;
    CamSimErrorType m_error = CamSimErrorType::E_ERRORCODE_NOERROR;

public:
    // Deleted default constructor
    FrameSourceRunner() = delete;

    // Explicit constructor
    explicit FrameSourceRunner(FrameSourceType sourceType);

    // Destructor (RAII: Stop thread if running)
    ~FrameSourceRunner();

    // Deleted copy constructor & assignment (shared_ptr might be fine but often best to avoid copying threaded objects)
    FrameSourceRunner(const FrameSourceRunner&) = delete;
    FrameSourceRunner& operator=(const FrameSourceRunner&) = delete;

    // Move constructor
    FrameSourceRunner(FrameSourceRunner&& other) noexcept;

    // Move assignment
    FrameSourceRunner& operator=(FrameSourceRunner&& other) noexcept;

    // Run and Stop interface
    CamSimStatusType FrameSource_Run(std::function<void(const Frame&, CamSimErrorType eError, CamSimStatusType eStatus)> frameCallback);
    CamSimStatusType FrameSource_Stop();

    // Query error state
    CamSimErrorType GetLastError() const noexcept;
};