cmake_minimum_required(VERSION 3.20)
project(ode_solver VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ODE Solver Library
add_library(ode_lib
    src/ode/euler.cpp
    src/ode/rk2.cpp
    src/ode/rk4.cpp
    src/ode/rk45.cpp
)

target_include_directories(ode_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Main executable
add_executable(my_project src/main.cpp)
target_link_libraries(my_project PRIVATE ode_lib)

# Examples
add_executable(exponential_decay examples/exponential_decay.cpp)
target_link_libraries(exponential_decay PRIVATE ode_lib)

add_executable(harmonic_oscillator examples/harmonic_oscillator.cpp)
target_link_libraries(harmonic_oscillator PRIVATE ode_lib)

add_executable(lorenz examples/lorenz.cpp)
target_link_libraries(lorenz PRIVATE ode_lib)

add_executable(solver_comparison examples/solver_comparison.cpp)
target_link_libraries(solver_comparison PRIVATE ode_lib)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Install targets
install(TARGETS my_project ode_lib DESTINATION bin)