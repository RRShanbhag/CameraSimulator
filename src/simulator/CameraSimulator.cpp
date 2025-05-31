#include "CameraSimulator.h"
#include <iostream>

bool CameraSimulator::initialize() {
    std::cout << "[Simulator] Initializing...\n";
    // TODO: load config, set up pipeline, etc.
    return true;
}

void CameraSimulator::run() {
    std::cout << "[Simulator] Running main loop...\n";
    // TODO: feed frames to pipeline or device
}