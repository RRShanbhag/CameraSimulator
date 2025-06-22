#include "CameraSimulator.h"
#include "../utils/CamSimLogger.h"
#include "../utils/CamSimCommons.h"
#include <iostream>

bool CameraSimulator::initialize() {
    CAMSIMLOG_INFO("[Simulator] Initializing...");
    // TODO: load config, set up pipeline, etc.
    return true;
}

void CameraSimulator::run() {
    CAMSIMLOG_INFO("[Simulator] Running main loop...");
    // TODO: feed frames to pipeline or device
}