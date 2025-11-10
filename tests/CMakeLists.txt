find_package(Catch2 3 REQUIRED)

# Test executables
add_executable(test_analytical test_analytical.cpp)
target_link_libraries(test_analytical PRIVATE ode_lib Catch2::Catch2WithMain)

add_executable(test_convergence test_convergence.cpp)
target_link_libraries(test_convergence PRIVATE ode_lib Catch2::Catch2WithMain)

add_executable(test_harmonic test_harmonic.cpp)
target_link_libraries(test_harmonic PRIVATE ode_lib Catch2::Catch2WithMain)

# Register tests with CTest
include(CTest)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

catch_discover_tests(test_analytical)
catch_discover_tests(test_convergence)
catch_discover_tests(test_harmonic)