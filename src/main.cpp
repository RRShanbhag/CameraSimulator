#include "utils/CamSimLogger.h"
#include "simulator/CameraSimulator.h"
#include "utils/CamSimCommons.h"
#include <iostream>
int main(int argc, char* argv[]) {
    if(argc > 1 && argv != nullptr)
    {

    }
    
    CAMSIMLOG_INFO("[Camera Simulator] Starting virtual camera system...");
    std::cout << "[Camera Simulator] Starting virtual camera system..." << std::endl;

    CameraSimulator simulator;
    if (!simulator.CameraSimulator_Initialize(IFrameSourceType::E_FRMSRC_DISK_VIDEO)) {
        CAMSIMLOG_ERROR("Failed to initialize simulator.\n");
        return 1;
    }

    simulator.CameraSimulator_Run();

    std::cout << "[Camera Simulator] Shutting down." << std::endl;
    CAMSIMLOG_INFO("[Camera Simulator] Shutting down.");
    return 0;
}