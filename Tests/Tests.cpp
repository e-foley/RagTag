using namespace std;

#include "tag_map.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iostream>

namespace ragtag {
  TEST_CASE("TagMap registerTag(), deleteTag(), isTagRegistered(), numTags()", "[all][TagMap-1]") {
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

  TEST_CASE("TagMap registerTag() with non-default TagProperties", "[all][TagMap-2]") {
    TagMap map;

    TagProperties props1;
    props1.default_setting = TagSetting::NO;
    REQUIRE(map.registerTag("Defaults to NO", props1));
    auto props_get = map.getTagProperties("Defaults to NO");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::NO);

    TagProperties props2;
    props2.default_setting = TagSetting::YES;
    REQUIRE(map.registerTag("Defaults to YES", props2));
    props_get = map.getTagProperties("Defaults to YES");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::YES);

    TagProperties props3;
    props3.default_setting = TagSetting::UNCOMMITTED;
    REQUIRE(map.registerTag("Defaults to UNCOMMITTED", props3));
    props_get = map.getTagProperties("Defaults to UNCOMMITTED");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::UNCOMMITTED);
  }

  TEST_CASE("TagMap tag re-registration with new properties", "[all][TagMap-3]") {
    TagMap map;
    REQUIRE_FALSE(map.getTagProperties("tag").has_value());

    TagProperties props;
    props.default_setting = TagSetting::YES;
    REQUIRE(map.registerTag("tag", props));
    CHECK(map.numTags() == 1);
    auto props_get = map.getTagProperties("tag");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::YES);

    REQUIRE(map.deleteTag("tag"));
    CHECK(map.numTags() == 0);

    props.default_setting = TagSetting::NO;
    REQUIRE(map.registerTag("tag", props));
    CHECK(map.numTags() == 1);
    props_get = map.getTagProperties("tag");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::NO);
  }

  TEST_CASE("TagMap getAllTags()", "[all][TagMap-4]") {
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

  TEST_CASE("TagMap JSON read/write reflection", "[all][TagMap-5]") {
    // It's important that this test use every feature of the TagMap interface.
    TagMap tag_map_in;

    TagProperties default_no;
    default_no.default_setting = TagSetting::NO;
    TagProperties default_yes;
    default_yes.default_setting = TagSetting::YES;
    TagProperties default_uncommitted;
    default_uncommitted.default_setting = TagSetting::UNCOMMITTED;

    // Registering tags...
    CHECK(tag_map_in.registerTag("Banana", default_no));
    CHECK(tag_map_in.registerTag("Cantaloupe", default_uncommitted));
    CHECK(tag_map_in.registerTag("Durian", default_no));
    CHECK(tag_map_in.registerTag("Apple", default_yes));
    CHECK(tag_map_in.deleteTag("Durian"));  // Testing that deletion leaves no lingering artifacts
    CHECK(tag_map_in.registerTag("Dragonfruit"));

    // Registering files...
    ragtag::path_t path = "test1.madeup";
    CHECK(tag_map_in.addFile(path));

    path = "test2.madeup";
    CHECK(tag_map_in.addFile(path));
    CHECK(tag_map_in.setRating(path, 3.1f));
    CHECK(tag_map_in.setTag(path, "Dragonfruit", TagSetting::YES));
    CHECK(tag_map_in.setTag(path, "Cantaloupe", TagSetting::NO));
    CHECK(tag_map_in.setTag(path, "Banana", TagSetting::UNCOMMITTED));
    // Leave "Apple" unspecified in test2.

    path = "test3.madeup";
    CHECK(tag_map_in.addFile(path));
    // Intentionally don't assign rating here.
    CHECK(tag_map_in.setTag(path, "Banana", TagSetting::NO));
    CHECK(tag_map_in.setTag(path, "Cantaloupe", TagSetting::YES));
    CHECK(tag_map_in.setTag(path, "Dragonfruit", TagSetting::NO));
    CHECK(tag_map_in.setTag(path, "Apple", TagSetting::YES));

    path = "test4.madeup";
    CHECK(tag_map_in.addFile(path));
    CHECK(tag_map_in.setRating(path, 3.1f));
    CHECK(tag_map_in.setTag(path, "Apple", TagSetting::NO));
    // Testing that deletion leaves no lingering artifacts
    CHECK(tag_map_in.removeFile("test4.madeup"));
    
    // Convert to JSON
    nlohmann::json j = tag_map_in.toJson();

    SECTION("JSON read/write") {
      auto from_json_ret = TagMap::fromJson(j);
      REQUIRE(from_json_ret.has_value());
      CHECK(*from_json_ret == tag_map_in);

      // Sanity checks to make sure we didn't write/read empty stream or something.
      CHECK(from_json_ret->numTags() == 4);
      const auto test2_rating_ret = from_json_ret->getRating("test2.madeup");
      REQUIRE(test2_rating_ret.has_value());
      CHECK(*test2_rating_ret == 3.1f);
      const auto test2_tag_ret = from_json_ret->getTagSetting("test2.madeup", "Dragonfruit");
      REQUIRE(test2_tag_ret.has_value());
      CHECK(*test2_tag_ret == TagSetting::YES);
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

      // Sanity checks to make sure we didn't write/read empty file or something.
      CHECK(constructed_from_file->numTags() == 4);
      const auto test3_rating_ret = constructed_from_file->getRating("test3.madeup");
      CHECK_FALSE(test3_rating_ret.has_value());
      const auto test3_tag_ret = constructed_from_file->getTagSetting("test3.madeup", "Banana");
      REQUIRE(test3_tag_ret.has_value());
      CHECK(*test3_tag_ret == TagSetting::NO);

      //std::remove("io.json");
    }
  }

  TEST_CASE("TagMap selectFiles()", "[all][TagMap-6]") {
    TagMap tag_map;
    REQUIRE(tag_map.registerTag("2 legs"));
    REQUIRE(tag_map.registerTag("4 legs"));
    REQUIRE(tag_map.registerTag("6 legs"));
    REQUIRE(tag_map.registerTag("flies"));

    REQUIRE(tag_map.addFile("eagle"));
    REQUIRE(tag_map.setTag("eagle", "2 legs", TagSetting::YES));
    REQUIRE(tag_map.setTag("eagle", "4 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("eagle", "6 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("eagle", "flies", TagSetting::YES));
    REQUIRE(tag_map.setRating("eagle", 4.5f));

    REQUIRE(tag_map.addFile("human"));
    REQUIRE(tag_map.setTag("human", "2 legs", TagSetting::YES));
    REQUIRE(tag_map.setTag("human", "4 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("human", "6 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("human", "flies", TagSetting::NO));
    REQUIRE(tag_map.setRating("human", 4.0f));

    REQUIRE(tag_map.addFile("spider"));
    REQUIRE(tag_map.setTag("spider", "2 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("spider", "4 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("spider", "6 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("spider", "flies", TagSetting::NO));
    REQUIRE(tag_map.setRating("spider", 2.0f));

    REQUIRE(tag_map.addFile("dog"));
    REQUIRE(tag_map.setTag("dog", "2 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("dog", "4 legs", TagSetting::YES));
    REQUIRE(tag_map.setTag("dog", "6 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("dog", "flies", TagSetting::NO));
    REQUIRE(tag_map.setRating("dog", 5.0f));

    REQUIRE(tag_map.addFile("dragonfly"));
    REQUIRE(tag_map.setTag("dragonfly", "2 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("dragonfly", "4 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag("dragonfly", "6 legs", TagSetting::YES));
    REQUIRE(tag_map.setTag("dragonfly", "flies", TagSetting::YES));
    REQUIRE(tag_map.setRating("dragonfly", 3.5f));

    REQUIRE(tag_map.addFile("ditto"));
    REQUIRE(tag_map.setTag("ditto", "2 legs", TagSetting::UNCOMMITTED));
    REQUIRE(tag_map.setTag("ditto", "4 legs", TagSetting::UNCOMMITTED));
    REQUIRE(tag_map.setTag("ditto", "6 legs", TagSetting::UNCOMMITTED));
    REQUIRE(tag_map.setTag("ditto", "flies", TagSetting::UNCOMMITTED));
    REQUIRE(tag_map.setRating("ditto", 123.4f));

    // Note: The predicates don't have to be assigned a variable, but doing so makes it easier to
    // compose them into more complex predicates.

    const TagMap::file_qualifier_t has_two_legs = [](const TagMap::FileInfo& p) {
      return p.f_tag_setting("2 legs") == TagSetting::YES;
    };
    const auto two_legged_creatures = tag_map.selectFiles(has_two_legs);
    CHECK(two_legged_creatures.size() == 2);

    const TagMap::file_qualifier_t is_flightless = [](const TagMap::FileInfo& p) {
      return p.f_tag_setting("flies") == TagSetting::NO;
    };
    const auto flightless_creatures = tag_map.selectFiles(is_flightless);
    CHECK(flightless_creatures.size() == 3);

    const TagMap::file_qualifier_t is_rated_at_least_four = [](const TagMap::FileInfo& p) {
      return p.rating.has_value() && *p.rating >= 4.0f;
    };
    const auto creatures_rated_at_least_four = tag_map.selectFiles(is_rated_at_least_four);
    CHECK(creatures_rated_at_least_four.size() == 4);

    const TagMap::file_qualifier_t plausibly_has_four_legs = [](const TagMap::FileInfo& p) {
      auto four_legs_state = p.f_tag_setting("4 legs");
      return four_legs_state == TagSetting::YES || four_legs_state == TagSetting::UNCOMMITTED;
    };
    const auto creatures_plausibly_with_four_legs = tag_map.selectFiles(plausibly_has_four_legs);
    CHECK(creatures_plausibly_with_four_legs.size() == 2);

    // Meta: There's probably an easier way than this to compose function predicates.
    const auto flightless_or_high_rated_creatures =
      tag_map.selectFiles([&is_flightless, &is_rated_at_least_four](const TagMap::FileInfo& p) {
        return is_flightless(p) || is_rated_at_least_four(p);
      });
    CHECK(flightless_or_high_rated_creatures.size() == 5);
  }
}  // namespace ragtag
