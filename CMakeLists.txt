cmake_minimum_required(VERSION 3.15)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # <-- Add this line to allow older sub-projects
project(spatial-engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Automatically download and compile Raylib for the current OS
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(main app/main.cpp)

# Link Raylib target (handles all platform-specific frameworks/libraries automatically)
target_link_libraries(main PRIVATE raylib)
