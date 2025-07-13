#include <memory>
#include "FrameSourceRunner.h"
#include "DiskFrameSource.h"
#include "../utils/CamSimLogger.h"

FrameSourceRunner::FrameSourceRunner(IFrameSourceType sourceType)
{
    // optional: validate source
    std::string filesrcdir = "./../data"; 
    int outFileSrcWidth = 1280;
    int outFileSrcHeight = 720;
    int outFileSrcFPS = 60;
    bool outFileSrcLoopback = false;
    IFrameFormat outFileSrcFmt = IFrameFormat::E_RGB;
    switch (sourceType)
    {
        case IFrameSourceType::E_FRMSRC_DISK_VIDEO:
        {
            m_source = std::make_shared<DiskFrameSource>();
            if(m_source->IFrameSource_Init(filesrcdir, outFileSrcWidth, outFileSrcHeight, outFileSrcFmt, outFileSrcFPS, outFileSrcLoopback) != CamSimStatusType::E_STATUS_SUCCESS)
            {
                CAMSIMLOG_ERROR("Frame Source Initialization Failed!");
            }
            else
            {
                CAMSIMLOG_INFO("Frame Source Initialization Success");
            }
            break;
        }

        default:
        {
            CAMSIMLOG_ERROR("Unknown Frame Source Type");
            break;
        }
    }
}

FrameSourceRunner::FrameSourceRunner(FrameSourceRunner&& other) noexcept
    : m_source(std::move(other.m_source)),
      m_running(other.m_running.load()),
      m_worker(std::move(other.m_worker)),
      m_error(other.m_error)
{
    other.m_running = false;
}

FrameSourceRunner::~FrameSourceRunner()
{
    FrameSource_Stop();  // ensures thread is joined
}

FrameSourceRunner& FrameSourceRunner::operator=(FrameSourceRunner&& other) noexcept
{
    if (this != &other)
    {
        FrameSource_Stop(); // stop own thread if running

        m_source = std::move(other.m_source);
        m_running = other.m_running.load();
        m_worker = std::move(other.m_worker);
        m_error = other.m_error;

        other.m_running = false;
    }
    return *this;
}

CamSimStatusType FrameSourceRunner::FrameSource_Run(std::function<void(const IFrame&, CamSimErrorType eError, CamSimStatusType eStatus)> frameCallback)
{
    
    if (m_running || !m_source)
        return CamSimStatusType::E_STATUS_FAILURE;


    m_running = true;
    if(m_source->IFrameSource_Start() != CamSimStatusType::E_STATUS_SUCCESS)
    {
        CAMSIMLOG_ERROR("Frame Source Start Failed");
        return CamSimStatusType::E_STATUS_FAILURE;
    }

    m_worker = std::thread([this, callback = std::move(frameCallback)] {
        IFrame frame;
        CamSimStatusType eStatus = CamSimStatusType::E_STATUS_SUCCESS;
        while (m_running)
        {
            eStatus = m_source->IFrameSource_GetNextFrame(frame); 
            if (eStatus != CamSimStatusType::E_STATUS_SUCCESS)
            {
                CAMSIMLOG_ERROR("Get Next Frame failed");
                if(m_source->IFrameSource_Stop() != CamSimStatusType::E_STATUS_SUCCESS)
                {
                    CAMSIMLOG_ERROR("Frame Source Stop Failed");
                }
                break;
            }
            callback(frame, m_source->IFrameSource_GetErrorStatus(), eStatus);
        }
    });

    return CamSimStatusType::E_STATUS_SUCCESS;
}

CamSimStatusType FrameSourceRunner::FrameSource_Stop()
{
    if (m_running)
    {
        m_running = false;
        if (m_worker.joinable())
            m_worker.join();
    }

    if(m_source->IFrameSource_Stop() != CamSimStatusType::E_STATUS_SUCCESS)
    {
        CAMSIMLOG_ERROR("Frame Source Stop Failed");
        return CamSimStatusType::E_STATUS_FAILURE;
    }
    return CamSimStatusType::E_STATUS_SUCCESS;
}