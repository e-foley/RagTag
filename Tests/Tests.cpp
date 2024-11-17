using namespace std;

#include "tag_map.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("TagMap addTag(), getTag(), removeTag()", "[all][TagMap-1]") {
  TagMap map;

  // Getting an unregistered tag should fail.
  CHECK_FALSE(map.getTag(8675309).has_value());

  // Adding the tag and then getting it should succeed.
  REQUIRE(map.addTag(8675309, "Jenny"));
  auto tag_ret = map.getTag(8675309);
  REQUIRE(tag_ret.has_value());
  CHECK(*tag_ret == "Jenny");

  // Can't remove tag that doesn't exist yet.
  CHECK_FALSE(map.removeTag(456));

  // Removing a tag that exists should succeed.
  REQUIRE(map.removeTag(8675309));
  tag_ret = map.getTag(8675309);
  CHECK_FALSE(tag_ret.has_value());
}
