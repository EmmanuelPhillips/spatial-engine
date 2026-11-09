cmake_minimum_required(VERSION 3.15)
project(spatial-engine CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(main app/main.cpp)

target_link_libraries(main PRIVATE 
    ${CMAKE_SOURCE_DIR}/lib/libraylib.a
    X11
    Xrandr
    Xinerama
    Xi
    Xcursor
    dl
    pthread
    m
)
