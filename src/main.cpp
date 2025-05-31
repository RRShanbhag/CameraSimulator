#include "utils/CamSimLogger.h"
#include "simulator/CameraSimulator.h"

int main(int argc, char* argv[]) {
    CAMSIMLOG_INFO("[Camera Simulator] Starting virtual camera system...\n");

    CameraSimulator simulator;
    if (!simulator.initialize()) {
        CAMSIMLOG_ERROR("Failed to initialize simulator.\n");
        return 1;
    }

    simulator.run();

    CAMSIMLOG_INFO("[Camera Simulator] Shutting down.\n");
    return 0;
}