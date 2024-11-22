using namespace std;

#include "tag_map.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iostream>

namespace ragtag {
  TEST_CASE("TagMap registerTag(), deleteTag(), numTags()", "[all][TagMap-1]") {
    TagMap map;

    CHECK(map.numTags() == 0);
    CHECK_FALSE(map.isTagRegistered("Benny"));
    REQUIRE(map.registerTag("Benny"));
    CHECK(map.numTags() == 1);
    CHECK(map.isTagRegistered("Benny"));

    // Attempting to add already-added tag should fail.
    CHECK_FALSE(map.registerTag("Benny"));
    CHECK(map.numTags() == 1);

    // Can't remove tag that doesn't exist yet.
    CHECK_FALSE(map.deleteTag("Charlie"));
    CHECK(map.numTags() == 1);

    // Removing a tag that exists should succeed.
    REQUIRE(map.deleteTag("Benny"));
    CHECK(map.numTags() == 0);
    CHECK_FALSE(map.isTagRegistered("Benny"));
  }

  //TEST_CASE("TagMap tag re-registration with new properties", "[all][TagMap-2]") {
  //  TagMap map;
  //  REQUIRE_FALSE(map.getTag(555).has_value());

  //  CHECK(map.registerTag(555, "Tag A"));
  //  CHECK(map.numTags() == 1);
  //  auto tag_ret = map.getTag(555);
  //  REQUIRE(tag_ret.has_value());
  //  CHECK(*tag_ret == "Tag A");

  //  REQUIRE(map.deleteTag(555));
  //  CHECK(map.numTags() == 0);
  //  CHECK(map.registerTag(555, "Tag Z"));
  //  CHECK(map.numTags() == 1);
  //  tag_ret = map.getTag(555);
  //  REQUIRE(tag_ret.has_value());
  //  CHECK(*tag_ret == "Tag Z");
  //}

  TEST_CASE("TagMap getAllTags()", "[all][TagMap-3]") {
    TagMap map;
    CHECK(map.getAllTags().size() == 0);

    // Intentionally add tag numbers out of order to ensure later that getAllTags() sorts by value.
    REQUIRE(map.registerTag("300"));
    REQUIRE(map.registerTag("100"));
    REQUIRE(map.registerTag("400"));
    REQUIRE(map.registerTag("500"));
    REQUIRE(map.registerTag("200"));

    auto all_tags = map.getAllTags();
    REQUIRE(all_tags.size() == 5);

    // Confirm that the tags are sorted numerically by ID.
    CHECK(all_tags.at(0) == std::pair<tag_t, TagProperties>("100", TagProperties{}));
    CHECK(all_tags.at(1) == std::pair<tag_t, TagProperties>("200", TagProperties{}));
    CHECK(all_tags.at(2) == std::pair<tag_t, TagProperties>("300", TagProperties{}));
    CHECK(all_tags.at(3) == std::pair<tag_t, TagProperties>("400", TagProperties{}));
    CHECK(all_tags.at(4) == std::pair<tag_t, TagProperties>("500", TagProperties{}));

    // Confirm that removing elements refreshes the results from getAllTags().
    REQUIRE(map.deleteTag("300"));
    REQUIRE(map.deleteTag("500"));
    REQUIRE(map.deleteTag("100"));
    all_tags = map.getAllTags();
    REQUIRE(all_tags.size() == 2);
    CHECK(all_tags.at(0) == std::pair<tag_t, TagProperties>("200", TagProperties{}));
    CHECK(all_tags.at(1) == std::pair<tag_t, TagProperties>("400", TagProperties{}));
  }

  TEST_CASE("TagMap JSON read/write reflection", "[all][TagMap-4]") {
    // It's important that this test use every feature of the TagMap interface.
    TagMap tag_map_in;
    tag_map_in.registerTag("Banana");
    tag_map_in.registerTag("Cantaloupe");
    tag_map_in.registerTag("Durian");
    tag_map_in.registerTag("Apple");
    tag_map_in.deleteTag("Durian");
    tag_map_in.registerTag("Dragonfruit");
    nlohmann::json j = tag_map_in.toJson();

    SECTION("JSON read/write") {
      auto from_json_ret = TagMap::fromJson(j);
      REQUIRE(from_json_ret.has_value());
      CHECK(*from_json_ret == tag_map_in);

      // Sanity check to make sure we didn't write/read empty file or something.
      CHECK(from_json_ret->numTags() == 4);
    }

    SECTION("File read/write") {
      std::ofstream o("io.json");
      o << std::setw(2) << j << std::endl;
      o.close();

      std::ifstream i("io.json");
      nlohmann::json parsed = nlohmann::json::parse(i);
      i.close();
      auto constructed_from_file = TagMap::fromJson(parsed);
      REQUIRE(constructed_from_file.has_value());
      CHECK(*constructed_from_file == tag_map_in);
      CHECK(constructed_from_file->numTags() == 4);

      std::remove("io.json");
    }
  }
}  // namespace ragtag
