#!/bin/sh
cd ./build

#For CI Pipeline run with ctest instead of running the application. 
#ctest --output-on-failure

#This is meant to test in local repository only.
./CameraSimulatorTest_Runner