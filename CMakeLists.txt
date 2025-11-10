cmake_minimum_required(VERSION 3.20)
project(my_project VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(my_project src/main.cpp)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Install target for publishing
install(TARGETS my_project DESTINATION bin)