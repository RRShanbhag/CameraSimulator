#include "utils/CamSimLogger.h"
#include "simulator/CameraSimulator.h"
#include "utils/CamSimCommons.h"
#include <iostream>
int main(int argc, char* argv[]) {
    std::cout << "[Camera Simulator] Starting virtual camera system..." << std::endl;

    CameraSimulator simulator;
    if (!simulator.initialize()) {
        CAMSIMLOG_ERROR("Failed to initialize simulator.\n");
        return 1;
    }

    simulator.run();

    std::cout << "[Camera Simulator] Shutting down." << std::endl;
    return 0;
}