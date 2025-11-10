find_package(Catch2 3 REQUIRED)

add_executable(test_main test_main.cpp)
target_link_libraries(test_main PRIVATE Catch2::Catch2WithMain)

include(CTest)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(test_main)