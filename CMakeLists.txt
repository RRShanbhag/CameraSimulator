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

#Fetch FFMPEG Components
set(FFMPEG_COMPONENTS
    avformat
    avcodec
    avutil
    swscale
)

foreach(lib ${FFMPEG_COMPONENTS})
    pkg_check_modules(FFMPEG_${lib} REQUIRED lib${lib})
endforeach()

set(FFMPEG_INCLUDE_DIRS "")
set(FFMPEG_LIBRARIES "")
foreach(lib ${FFMPEG_COMPONENTS})
    list(APPEND FFMPEG_INCLUDE_DIRS ${FFMPEG_${lib}_INCLUDE_DIRS})
    list(APPEND FFMPEG_LIBRARIES ${FFMPEG_${lib}_LIBRARIES})
endforeach()
message(STATUS "FFmpeg include dirs: ${FFMPEG_INCLUDE_DIRS}")
message(STATUS "FFmpeg libraries: ${FFMPEG_LIBRARIES}")

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# --------------------------
# FrameSource Library
# --------------------------
file(GLOB PROFILER_SRC CONFIGURE_DEPENDS "src/profiler/*.cpp")
add_library(Profiler ${PROFILER_SRC})
target_include_directories(Profiler PUBLIC ${CMAKE_SOURCE_DIR}/src/profiler)

# --------------------------
# FrameSource Library
# --------------------------
file(GLOB FRAME_SRC CONFIGURE_DEPENDS "src/framesource/*.cpp")
add_library(FrameSource ${FRAME_SRC})
target_include_directories(FrameSource PUBLIC ${CMAKE_SOURCE_DIR}/src/framesource ${CMAKE_SOURCE_DIR}/src/utils ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(FrameSource PUBLIC ${FFMPEG_LIBRARIES})

# --------------------------
# Simulator Library
# --------------------------
file(GLOB SIM_SRC CONFIGURE_DEPENDS "src/simulator/*.cpp")
file(GLOB UTILS_SRC CONFIGURE_DEPENDS "src/utils/*.cpp")
add_library(Simulator ${SIM_SRC} ${UTILS_SRC})
target_include_directories(Simulator PUBLIC src)
target_link_libraries(Simulator PUBLIC Profiler FrameSource)

# --------------------------
# Main Executable
# --------------------------
add_executable(CameraSimulator src/main.cpp)
target_link_libraries(CameraSimulator
    PRIVATE
    Simulator
    ${FFMPEG_LIBRARIES}
)

# --------------------------
# Unit Test Executable
# --------------------------
enable_testing()
file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(CameraSimulatorTest_Runner ${TEST_SRC})
target_include_directories(CameraSimulatorTest_Runner PRIVATE src ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(CameraSimulatorTest_Runner
    PRIVATE
    Simulator
    gtest
    gtest_main
    ${FFMPEG_LIBRARIES}
)

add_test(NAME AllTests COMMAND CameraSimulatorTest_Runner)
