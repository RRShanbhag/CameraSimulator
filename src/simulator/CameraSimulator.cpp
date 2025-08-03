#include "CameraSimulator.h"
#include "../utils/CamSimLogger.h"
#include "../utils/CamSimCommons.h"
#include "../framesource/FrameSourceRunner.h"

#include <sstream>

bool CameraSimulator::CameraSimulator_Initialize(FrameSourceType eSourceType) {
    CAMSIMLOG_INFO("[Simulator] Initializing...");
    m_eSourceType = eSourceType;
    return true;
}

void CameraSimulator::CameraSimulator_Run() {
    CAMSIMLOG_INFO("[Simulator] Running main loop...");
    // TODO: feed frames to pipeline or device
    FrameSourceRunner runner(m_eSourceType);
    int frameCount = 0;
    bool simulatorStop = false;
    runner.FrameSource_Run([&](const Frame& frame, CamSimErrorType eError, CamSimStatusType eStatus) {
        std::cout << "[Simulator]: Received Frame No:" << frameCount << std::endl;

        std::string FrameFileName = "DiskFrame_" + std::to_string(frameCount) + ".raw";
        int ret = CamSimLogger::WriteRawFrameToFile(FrameFileName.c_str(), frame.frameSize, frame.data);
        if(ret < 0)
        {
            CAMSIMLOG_ERROR("Failed To Write To File");
        }
        frameCount = (frameCount + 1);
        if(frameCount == 100 || eStatus == CamSimStatusType::E_STATUS_FAILURE || eError != CamSimErrorType::E_ERRORCODE_NOERROR)
        {
            simulatorStop = true;
        }
    });

    while(simulatorStop == false)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    CAMSIMLOG_INFO("[Simulator] Stopping main loop...");
    runner.FrameSource_Stop();
}