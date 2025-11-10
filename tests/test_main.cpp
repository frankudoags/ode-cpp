#include <catch2/catch_test_macros.hpp>

TEST_CASE("Basic test", "[hello]") {
    REQUIRE(1 + 1 == 2);
}