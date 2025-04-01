using namespace std;

#include "tag_map.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <iostream>

namespace ragtag {
  TEST_CASE("TagMap registerTag(), deleteTag(), isTagRegistered(), numTags()", "[all][TagMap-1]") {
    TagMap map;

    CHECK(map.numTags() == 0);
    CHECK_FALSE(map.isTagRegistered(L"Benny"));
    REQUIRE(map.registerTag(L"Benny"));
    CHECK(map.numTags() == 1);
    CHECK(map.isTagRegistered(L"Benny"));

    // Attempting to add already-added tag should fail.
    CHECK_FALSE(map.registerTag(L"Benny"));
    CHECK(map.numTags() == 1);

    // Can't remove tag that doesn't exist yet.
    CHECK_FALSE(map.deleteTag(L"Charlie"));
    CHECK(map.numTags() == 1);

    // Removing a tag that exists should succeed.
    REQUIRE(map.deleteTag(L"Benny"));
    CHECK(map.numTags() == 0);
    CHECK_FALSE(map.isTagRegistered(L"Benny"));
  }

  TEST_CASE("TagMap registerTag() with non-default TagProperties", "[all][TagMap-2]") {
    TagMap map;

    TagProperties props1;
    props1.default_setting = TagSetting::NO;
    REQUIRE(map.registerTag(L"Defaults to NO", props1));
    auto props_get = map.getTagProperties(L"Defaults to NO");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::NO);

    TagProperties props2;
    props2.default_setting = TagSetting::YES;
    REQUIRE(map.registerTag(L"Defaults to YES", props2));
    props_get = map.getTagProperties(L"Defaults to YES");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::YES);

    TagProperties props3;
    props3.default_setting = TagSetting::UNCOMMITTED;
    REQUIRE(map.registerTag(L"Defaults to UNCOMMITTED", props3));
    props_get = map.getTagProperties(L"Defaults to UNCOMMITTED");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::UNCOMMITTED);
  }

  TEST_CASE("TagMap tag re-registration with new properties", "[all][TagMap-3]") {
    TagMap map;
    REQUIRE_FALSE(map.getTagProperties(L"tag").has_value());

    TagProperties props;
    props.default_setting = TagSetting::YES;
    REQUIRE(map.registerTag(L"tag", props));
    CHECK(map.numTags() == 1);
    auto props_get = map.getTagProperties(L"tag");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::YES);

    REQUIRE(map.deleteTag(L"tag"));
    CHECK(map.numTags() == 0);

    props.default_setting = TagSetting::NO;
    REQUIRE(map.registerTag(L"tag", props));
    CHECK(map.numTags() == 1);
    props_get = map.getTagProperties(L"tag");
    REQUIRE(props_get.has_value());
    CHECK(props_get->default_setting == TagSetting::NO);
  }

  TEST_CASE("TagMap getAllTags()", "[all][TagMap-4]") {
    TagMap map;
    CHECK(map.getAllTags().size() == 0);

    // Intentionally add tag numbers out of order to ensure later that getAllTags() sorts by value.
    REQUIRE(map.registerTag(L"300"));
    REQUIRE(map.registerTag(L"100"));
    REQUIRE(map.registerTag(L"400"));
    REQUIRE(map.registerTag(L"500"));
    REQUIRE(map.registerTag(L"200"));

    auto all_tags = map.getAllTags();
    REQUIRE(all_tags.size() == 5);

    // Confirm that the tags are sorted numerically by ID.
    CHECK(all_tags.at(0) == std::pair<tag_t, TagProperties>(L"100", TagProperties{}));
    CHECK(all_tags.at(1) == std::pair<tag_t, TagProperties>(L"200", TagProperties{}));
    CHECK(all_tags.at(2) == std::pair<tag_t, TagProperties>(L"300", TagProperties{}));
    CHECK(all_tags.at(3) == std::pair<tag_t, TagProperties>(L"400", TagProperties{}));
    CHECK(all_tags.at(4) == std::pair<tag_t, TagProperties>(L"500", TagProperties{}));

    // Confirm that removing elements refreshes the results from getAllTags().
    REQUIRE(map.deleteTag(L"300"));
    REQUIRE(map.deleteTag(L"500"));
    REQUIRE(map.deleteTag(L"100"));
    all_tags = map.getAllTags();
    REQUIRE(all_tags.size() == 2);
    CHECK(all_tags.at(0) == std::pair<tag_t, TagProperties>(L"200", TagProperties{}));
    CHECK(all_tags.at(1) == std::pair<tag_t, TagProperties>(L"400", TagProperties{}));
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
    CHECK(tag_map_in.registerTag(L"Banana", default_no));
    CHECK(tag_map_in.registerTag(L"Cantaloupe", default_uncommitted));
    CHECK(tag_map_in.registerTag(L"Durian", default_no));
    CHECK(tag_map_in.registerTag(L"Apple", default_yes));
    CHECK(tag_map_in.deleteTag(L"Durian"));  // Testing that deletion leaves no lingering artifacts
    CHECK(tag_map_in.registerTag(L"Dragonfruit"));
    CHECK(tag_map_in.registerTag(L"ÀÈÌÒÙ"));  // Non-ASCII tag

    // Registering files...
    ragtag::path_t path = "test1.madeup";
    CHECK(tag_map_in.addFile(path));
    
    path = "test2.madeup";
    CHECK(tag_map_in.addFile(path));
    CHECK(tag_map_in.setRating(path, 3.1f));
    CHECK(tag_map_in.setTag(path, L"Dragonfruit", TagSetting::YES));
    CHECK(tag_map_in.setTag(path, L"Cantaloupe", TagSetting::NO));
    CHECK(tag_map_in.setTag(path, L"Banana", TagSetting::UNCOMMITTED));
    CHECK(tag_map_in.setTag(path, L"ÀÈÌÒÙ", TagSetting::NO));
    // Leave "Apple" tag unspecified in test2.

    path = "test3.madeup";
    CHECK(tag_map_in.addFile(path));
    // Intentionally don't assign rating here.
    CHECK(tag_map_in.setTag(path, L"Banana", TagSetting::NO));
    CHECK(tag_map_in.setTag(path, L"Cantaloupe", TagSetting::YES));
    CHECK(tag_map_in.setTag(path, L"Dragonfruit", TagSetting::NO));
    CHECK(tag_map_in.setTag(path, L"Apple", TagSetting::YES));
    CHECK(tag_map_in.setTag(path, L"ÀÈÌÒÙ", TagSetting::YES));

    path = "test4.madeup";
    CHECK(tag_map_in.addFile(path));
    CHECK(tag_map_in.setRating(path, 3.1f));
    CHECK(tag_map_in.setTag(path, L"Apple", TagSetting::NO));
    // Testing that deletion leaves no lingering artifacts
    CHECK(tag_map_in.removeFile(L"test4.madeup"));

    path = "test5_ö.madeup";  // Non-ASCII filename
    CHECK(tag_map_in.addFile(path));
    CHECK(tag_map_in.setRating(path, -123.4f));
    CHECK(tag_map_in.setTag(path, L"Apple", TagSetting::YES));
    CHECK(tag_map_in.setTag(path, L"ÀÈÌÒÙ", TagSetting::NO));  // Non-ASCII tag in non-ASCII file
    
    // Convert to JSON
    const nlohmann::json j = tag_map_in.toJson();

    SECTION("JSON read/write") {
      auto from_json_ret = TagMap::fromJson(j);
      REQUIRE(from_json_ret.has_value());
      CHECK(*from_json_ret == tag_map_in);

      // Sanity checks to make sure we didn't write/read empty stream or something.
      CHECK(from_json_ret->numTags() == 5);
      const auto test2_rating_ret = from_json_ret->getRating(L"test2.madeup");
      REQUIRE(test2_rating_ret.has_value());
      CHECK(*test2_rating_ret == 3.1f);
      const auto test2_tag_ret = from_json_ret->getTagSetting(L"test2.madeup", L"Dragonfruit");
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
      CHECK(constructed_from_file->numTags() == 5);
      const auto test3_rating_ret = constructed_from_file->getRating(L"test3.madeup");
      CHECK_FALSE(test3_rating_ret.has_value());
      const auto test3_tag_ret = constructed_from_file->getTagSetting(L"test3.madeup", L"Banana");
      REQUIRE(test3_tag_ret.has_value());
      CHECK(*test3_tag_ret == TagSetting::NO);

      std::remove("io.json");
    }
  }

  TEST_CASE("TagMap selectFiles()", "[all][TagMap-6]") {
    TagMap tag_map;
    REQUIRE(tag_map.registerTag(L"2 legs"));
    REQUIRE(tag_map.registerTag(L"4 legs"));
    REQUIRE(tag_map.registerTag(L"6 legs"));
    REQUIRE(tag_map.registerTag(L"flies"));

    REQUIRE(tag_map.addFile(L"eagle"));
    REQUIRE(tag_map.setTag(L"eagle", L"2 legs", TagSetting::YES));
    REQUIRE(tag_map.setTag(L"eagle", L"4 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"eagle", L"6 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"eagle", L"flies", TagSetting::YES));
    REQUIRE(tag_map.setRating(L"eagle", 4.5f));

    REQUIRE(tag_map.addFile(L"human"));
    REQUIRE(tag_map.setTag(L"human", L"2 legs", TagSetting::YES));
    REQUIRE(tag_map.setTag(L"human", L"4 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"human", L"6 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"human", L"flies", TagSetting::NO));
    REQUIRE(tag_map.setRating(L"human", 4.0f));

    REQUIRE(tag_map.addFile(L"spider"));
    REQUIRE(tag_map.setTag(L"spider", L"2 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"spider", L"4 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"spider", L"6 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"spider", L"flies", TagSetting::NO));
    REQUIRE(tag_map.setRating(L"spider", 2.0f));

    REQUIRE(tag_map.addFile(L"dog"));
    REQUIRE(tag_map.setTag(L"dog", L"2 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"dog", L"4 legs", TagSetting::YES));
    REQUIRE(tag_map.setTag(L"dog", L"6 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"dog", L"flies", TagSetting::NO));
    REQUIRE(tag_map.setRating(L"dog", 5.0f));

    REQUIRE(tag_map.addFile(L"dragonfly"));
    REQUIRE(tag_map.setTag(L"dragonfly", L"2 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"dragonfly", L"4 legs", TagSetting::NO));
    REQUIRE(tag_map.setTag(L"dragonfly", L"6 legs", TagSetting::YES));
    REQUIRE(tag_map.setTag(L"dragonfly", L"flies", TagSetting::YES));
    REQUIRE(tag_map.setRating(L"dragonfly", 3.5f));

    REQUIRE(tag_map.addFile(L"ditto"));
    REQUIRE(tag_map.setTag(L"ditto", L"2 legs", TagSetting::UNCOMMITTED));
    REQUIRE(tag_map.setTag(L"ditto", L"4 legs", TagSetting::UNCOMMITTED));
    REQUIRE(tag_map.setTag(L"ditto", L"6 legs", TagSetting::UNCOMMITTED));
    REQUIRE(tag_map.setTag(L"ditto", L"flies", TagSetting::UNCOMMITTED));
    REQUIRE(tag_map.setRating(L"ditto", 123.4f));

    // Note: The predicates don't have to be assigned a variable, but doing so makes it easier to
    // compose them into more complex predicates.

    const TagMap::file_qualifier_t has_two_legs = [](const TagMap::FileInfo& p) {
      return p.f_tag_setting(L"2 legs") == TagSetting::YES;
    };
    const auto two_legged_creatures = tag_map.selectFiles(has_two_legs);
    CHECK(two_legged_creatures.size() == 2);

    const TagMap::file_qualifier_t is_flightless = [](const TagMap::FileInfo& p) {
      return p.f_tag_setting(L"flies") == TagSetting::NO;
    };
    const auto flightless_creatures = tag_map.selectFiles(is_flightless);
    CHECK(flightless_creatures.size() == 3);

    const TagMap::file_qualifier_t is_rated_at_least_four = [](const TagMap::FileInfo& p) {
      return p.rating.has_value() && *p.rating >= 4.0f;
    };
    const auto creatures_rated_at_least_four = tag_map.selectFiles(is_rated_at_least_four);
    CHECK(creatures_rated_at_least_four.size() == 4);

    const TagMap::file_qualifier_t plausibly_has_four_legs = [](const TagMap::FileInfo& p) {
      auto four_legs_state = p.f_tag_setting(L"4 legs");
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
