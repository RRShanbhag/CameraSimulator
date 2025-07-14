# Camera Simulator

![CI](https://github.com/RRShanbhag/CameraSimulator/actions/workflows/ci.yml/badge.svg)

## Overview

The Camera Simulator project provides a virtual camera system that mimics real hardware behavior for testing and development. It is built with a modular C++ architecture, follows **Test-Driven Development (TDD)** principles, and includes a fully automated **CI pipeline** using GitHub Actions.

This tool is ideal for:
- Developers working on camera firmware or software stacks
- Teams needing to test computer vision or video processing pipelines without physical hardware

---

## Features

- 🧱 Modular and testable C++ components
- 🎥 **FrameSource module** with real-time frame loading from video/image files using FFmpeg
- 🧪 Unit tests using GoogleTest
- 📂 Logger with timestamped file output
- 🔁 GitHub Actions CI (build and test on every push/PR)
- 🔧 CMake-based build system
- 🧪 Future support: simulated frame generation, sensor timing, and V4L2 loopback

---

## FrameSource Module

The `FrameSource` subsystem provides frame acquisition to the rest of the system and currently supports:

- ✅ **DiskFrameSource**: Loads frames from images or video files using FFmpeg.
- ✅ Provides real-time frame delivery at configured FPS.
- ✅ Designed for plug-and-play with `FrameSourceRunner`, which controls acquisition timing and forwards frames via a callback.
- ✅ Supports formats like RGB24, GRAY8, GRAY16 (planned), etc.
- ✅ Future support: camera hardware input, synthetic frame generator, network stream input.

The `FrameSourceRunner` orchestrates reading frames at regular intervals, simulating sensor-like behavior. It runs the grabbing loop in a separate thread and provides RAII safety and error reporting.

---

## Build Instructions

### Dependencies
- C++20 compiler (e.g., `g++`, `clang++`)
- `cmake >= 3.15`
- `libfmt-dev`
- `libgstreamer1.0-dev` (for future modules)
- `libavformat-dev`, `libavcodec-dev`, `libswscale-dev` (FFmpeg libraries)

### Build

```bash
git clone https://github.com/<your-username>/<your-repo-name>.git
cd <your-repo-name>
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build