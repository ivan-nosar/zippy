cmake_minimum_required(VERSION 3.1)

# Sets the PROJECT_NAME variable
project(zippy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# Set the output directory for the build artifacts / executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)

# Find header files
include_directories(${CMAKE_SOURCE_DIR}/src/headers)

# Find implementation files
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

# Main file
add_executable(${PROJECT_NAME} ${SOURCES})
