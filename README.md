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
- 🧪 Unit tests using GoogleTest
- 📂 Logger with timestamped file output
- 🔁 GitHub Actions CI (build and test on every push/PR)
- 🔧 CMake-based build system
- 🧪 Future support: simulated frame generation, sensor timing, and V4L2 loopback

---

## Build Instructions

### Dependencies
- C++20 compiler (e.g., `g++`, `clang++`)
- `cmake >= 3.15`
- `libfmt-dev`
- `libgstreamer1.0-dev`

### Build

```bash
git clone https://github.com/<your-username>/<your-repo-name>.git
cd <your-repo-name>
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build