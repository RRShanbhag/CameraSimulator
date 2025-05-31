cmake_minimum_required(VERSION 3.14)
project(CameraSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GStreamer (required for video stream output)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0>=1.14)

include_directories(
    ${GSTREAMER_INCLUDE_DIRS}
    src
)

link_directories(
    ${GSTREAMER_LIBRARY_DIRS}
)
add_subdirectory(external/fmt)
add_definitions(${GSTREAMER_CFLAGS_OTHER})

# Automatically include all .cpp files under src/
file(GLOB_RECURSE SIM_SRC CONFIGURE_DEPENDS "src/*.cpp")

# Create the executable
add_executable(CameraSimulator ${SIM_SRC})


target_link_libraries(CameraSimulator
    PRIVATE fmt::fmt-header-only
    ${GSTREAMER_LIBRARIES}
)

# ---- Unit Testing ----
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS "tests/*.cpp")

add_executable(test_runner ${TEST_SRC})
target_include_directories(test_runner PRIVATE src)
target_link_libraries(test_runner PRIVATE gtest_main fmt::fmt)

add_test(NAME LoggerTests COMMAND test_runner)
