cmake_minimum_required(VERSION 3.14)
project(CameraSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wno-format-security")

# --------------------------
# Find Required Packages
# --------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0>=1.14)

include(FetchContent)

# Fetch fmt
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)
FetchContent_MakeAvailable(fmt)

# Fetch GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# --------------------------
# FrameSource Library
# --------------------------
file(GLOB FRAME_SRC CONFIGURE_DEPENDS "src/framesource/*.cpp")
add_library(FrameSource ${FRAME_SRC})
target_include_directories(FrameSource PUBLIC ${CMAKE_SOURCE_DIR}/src/framesource ${CMAKE_SOURCE_DIR}/src/utils)
target_link_libraries(FrameSource PUBLIC fmt::fmt-header-only)

# --------------------------
# Simulator Library
# --------------------------
file(GLOB SIM_SRC CONFIGURE_DEPENDS "src/simulator/*.cpp")
file(GLOB UTILS_SRC CONFIGURE_DEPENDS "src/utils/*.cpp")
add_library(Simulator ${SIM_SRC} ${UTILS_SRC})
target_include_directories(Simulator PUBLIC src)
target_link_libraries(Simulator PUBLIC FrameSource)

# --------------------------
# Main Executable
# --------------------------
add_executable(CameraSimulator src/main.cpp)
target_link_libraries(CameraSimulator
    PRIVATE
    Simulator
    ${GSTREAMER_LIBRARIES}
)

# --------------------------
# Unit Test Executable
# --------------------------
enable_testing()
file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(CameraSimulatorTest_Runner ${TEST_SRC})
target_include_directories(CameraSimulatorTest_Runner PRIVATE src)
target_link_libraries(CameraSimulatorTest_Runner
    PRIVATE
    Simulator
    gtest
    gtest_main
    fmt::fmt-header-only
    ${GSTREAMER_LIBRARIES}
)

add_test(NAME AllTests COMMAND CameraSimulatorTest_Runner)
