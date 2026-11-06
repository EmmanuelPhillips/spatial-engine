cmake_minimum_required(VERSION 3.16)

project(raylib_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main app/main.cpp)

# Raylib header and static library
target_include_directories(main PRIVATE include)

add_library(raylib STATIC IMPORTED)
set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libraylib.a"
)

# System dependencies required by Raylib on Linux
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

target_link_libraries(main PRIVATE
    raylib
    OpenGL::GL
    Threads::Threads
    ${CMAKE_DL_LIBS}
    m
    rt
    X11::X11
)
