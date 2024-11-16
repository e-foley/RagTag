using namespace std;

#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("First", "[tests]") {
  REQUIRE(2 > 1);
}
