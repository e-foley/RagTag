using namespace std;

#include "tag_map.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("TagMap addTag(), getTag(), removeTag(), numTags()", "[all][TagMap-1]") {
  TagMap map;
  CHECK(map.numTags() == 0);

  // Getting an unregistered tag should fail.
  CHECK_FALSE(map.getTag(8675309).has_value());

  // Adding a tag and then getting it should succeed.
  REQUIRE(map.addTag(8675309, "Jenny"));
  CHECK(map.numTags() == 1);
  auto tag_ret = map.getTag(8675309);
  REQUIRE(tag_ret.has_value());
  CHECK(*tag_ret == "Jenny");

  // Attempting to add already-added tag should fail.
  CHECK_FALSE(map.addTag(8675309, "Tommy"));
  CHECK(map.numTags() == 1);

  // Attempted re-registration shouldn't overwrite old tag string.
  tag_ret = map.getTag(8675309);
  REQUIRE(tag_ret.has_value());
  CHECK(*tag_ret == "Jenny");

  // Can't remove tag that doesn't exist yet.
  CHECK_FALSE(map.removeTag(456));
  CHECK(map.numTags() == 1);

  // Removing a tag that exists should succeed.
  REQUIRE(map.removeTag(8675309));
  CHECK(map.numTags() == 0);
  tag_ret = map.getTag(8675309);
  CHECK_FALSE(tag_ret.has_value());
}

TEST_CASE("TagMap tag re-registration with new value", "[all][TagMap-2]") {
  TagMap map;
  REQUIRE_FALSE(map.getTag(555).has_value());

  CHECK(map.addTag(555, "Tag A"));
  CHECK(map.numTags() == 1);
  auto tag_ret = map.getTag(555);
  REQUIRE(tag_ret.has_value());
  CHECK(*tag_ret == "Tag A");

  REQUIRE(map.removeTag(555));
  CHECK(map.numTags() == 0);
  CHECK(map.addTag(555, "Tag Z"));
  CHECK(map.numTags() == 1);
  tag_ret = map.getTag(555);
  REQUIRE(tag_ret.has_value());
  CHECK(*tag_ret == "Tag Z");
}

TEST_CASE("TagMap getAllTags()", "[all][TagMap-3]") {
  TagMap map;
  CHECK(map.getAllTags().size() == 0);

  // Intentionally add tag numbers out of order to ensure later that getAllTags() sorts by value.
  REQUIRE(map.addTag(300, "#300"));
  REQUIRE(map.addTag(100, "#100"));
  REQUIRE(map.addTag(400, "#400"));
  REQUIRE(map.addTag(500, "#500"));
  REQUIRE(map.addTag(200, "#200"));

  auto all_tags = map.getAllTags();
  REQUIRE(all_tags.size() == 5);

  // Confirm that the tags are sorted numerically by ID.
  CHECK(all_tags.at(0) == std::pair<TagMap::id_t, TagMap::tag_t>(100, "#100"));
  CHECK(all_tags.at(1) == std::pair<TagMap::id_t, TagMap::tag_t>(200, "#200"));
  CHECK(all_tags.at(2) == std::pair<TagMap::id_t, TagMap::tag_t>(300, "#300"));
  CHECK(all_tags.at(3) == std::pair<TagMap::id_t, TagMap::tag_t>(400, "#400"));
  CHECK(all_tags.at(4) == std::pair<TagMap::id_t, TagMap::tag_t>(500, "#500"));

  // Confirm that removing elements refreshes the results from getAllTags().
  REQUIRE(map.removeTag(300));
  REQUIRE(map.removeTag(500));
  REQUIRE(map.removeTag(100));
  all_tags = map.getAllTags();
  REQUIRE(all_tags.size() == 2);
  CHECK(all_tags.at(0) == std::pair<TagMap::id_t, TagMap::tag_t>(200, "#200"));
  CHECK(all_tags.at(1) == std::pair<TagMap::id_t, TagMap::tag_t>(400, "#400"));
}
