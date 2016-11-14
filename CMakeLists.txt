cmake_minimum_required(VERSION 2.8)
include(lib830/cmake/frc.cmake)
set(CMAKE_BUILD_TYPE Debug)

project(StonePractice)
add_subdirectory(lib830)
include_directories(src lib830)

set(FRC_FORCE_TARGET mdns)
add_frc_robot(${PROJECT_NAME} 830
    src/Camera.cpp
    src/Robot.cpp
)
target_link_libraries(${PROJECT_NAME} 830)
