#!/bin/sh
rm -rf ./build/*
sleep 1
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build