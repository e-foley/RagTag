#include "tag_map.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <wx/stdpaths.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

namespace ragtag {
  // Ensure these are no larger than max int so that we can safely cast size_t to int.
  const int TagMap::MAX_NUM_TAGS = std::numeric_limits<int>::max();
  const int TagMap::MAX_NUM_FILES = std::numeric_limits<int>::max();

  TagMap::TagMap() {}

  bool TagMap::operator==(const TagMap& rhs) const noexcept {
    return tag_registry_ == rhs.tag_registry_ &&
      file_map_ == rhs.file_map_;
  }

  bool TagMap::registerTag(const tag_t tag) {
    return registerTag(tag, TagProperties{});
  }

  bool TagMap::registerTag(const tag_t tag, const TagProperties& properties) {
    if (numTags() >= MAX_NUM_TAGS) {
      return false;
    }

    return tag_registry_.emplace(tag, properties).second;
  }

  bool TagMap::deleteTag(const tag_t tag) {
    // erase() returns number of elements removed.
    return tag_registry_.erase(tag) > 0;
  }

  std::optional<TagProperties> TagMap::getTagProperties(const tag_t tag) const {
    const auto tag_it = tag_registry_.find(tag);
    if (tag_it == tag_registry_.end()) {
      return {};
    }
    return tag_it->second;
  }

  bool TagMap::isTagRegistered(const tag_t tag) const {
    return tag_registry_.contains(tag);
  }

  std::vector<std::pair<tag_t, TagProperties>> TagMap::getAllTags() const {
    std::vector<std::pair<tag_t, TagProperties>> tag_vector;
    tag_vector.reserve(tag_registry_.size());
    for (const auto map_it : tag_registry_) {
      tag_vector.emplace_back(map_it);
    }
    return tag_vector;
  }

  int TagMap::numTags() const {
    // Safe conversion provided MAX_NUM_TAGS is enforced.
    return static_cast<int>(tag_registry_.size());
  }

  bool TagMap::addFile(const path_t& path) {
    return addFile(path, FileProperties{});
  }

  bool TagMap::addFile(const path_t& path, const FileProperties& properties) {
    if (numFiles() >= MAX_NUM_FILES) {
      return false;
    }

    // TODO: Enforce valid FileProperties, which means confirming tags therein are registered.

    return file_map_.emplace(path, properties).second;
  }

  bool TagMap::removeFile(const path_t& path) {
    // erase() returns number of elements removed.
    return file_map_.erase(path) > 0;
  }

  bool TagMap::setTag(const path_t& path, const tag_t tag, const TagSetting setting) {
    const auto file_it = file_map_.find(path);
    if (file_it == file_map_.end()) {
      // File is not in our list.
      return false;
    }

    const auto tag_it = tag_registry_.find(tag);
    if (tag_it == tag_registry_.end()) {
      // Tag is not registered.
      return false;
    }

    // TODO: Perform validation on the setting that's appropriate for the configuration of the tag.
    if (setting < TagSetting::NO || setting > TagSetting::UNCOMMITTED) {
      return false;
    }

    // The .second refers to the success of the insertion-or-assignment operation.
    return file_it->second.tags.insert_or_assign(tag, setting).second;
  }

  bool TagMap::setRating(const path_t& path, const rating_t rating) {
    const auto file_it = file_map_.find(path);
    if (file_it == file_map_.end()) {
      // File is not in our list.
      return false;
    }

    file_it->second.rating = rating;
    return true;
  }

  bool TagMap::hasFile(const path_t& path) const {
    return file_map_.contains(path);
  }

  std::optional<FileProperties> TagMap::getFileProperties(const path_t& path) const {
    const auto file_it = file_map_.find(path);
    if (file_it == file_map_.end()) {
      return {};
    }
    return file_it->second;
  }

  std::vector<std::pair<path_t, FileProperties>> TagMap::getAllFiles() const {
    std::vector<std::pair<path_t, FileProperties>> file_vector;
    file_vector.reserve(file_map_.size());
    for (const auto map_it : file_map_) {
      file_vector.emplace_back(map_it);
    }
    return file_vector;
  }

  std::vector<std::pair<path_t, FileProperties>> TagMap::selectFiles(const file_qualifier_t& fn) const {
    std::vector<std::pair<path_t, FileProperties>> qualified_file_vector;
    std::copy_if(file_map_.begin(), file_map_.end(), std::back_inserter(qualified_file_vector),
      [&fn](const auto& p) {return std::invoke(fn, p.second);});
    return qualified_file_vector;
  }

  int TagMap::numFiles() const {
    // Safe conversion provided MAX_NUM_FILES is enforced.
    return static_cast<int>(file_map_.size());
  }

  nlohmann::json TagMap::toJson() const {
    nlohmann::json json;

    // To allow a (relatively) compact representation of our table, assign each tag an ID.
    std::map<tag_t, int> tag_to_id;
    nlohmann::json id_tag_array_json;
    int id = 1;  // Start at 1 so that we can use 0 as some kind of default value if we want.
    for (auto tag_it : tag_registry_) {
      // TODO: Implement error handling.
      tag_to_id.try_emplace(tag_it.first, id);
      nlohmann::json adding;
      adding["id"] = id;
      adding["tag"] = tag_it.first;
      // TODO: Another place that needs error handling attention.
      auto default_setting_num = tagSettingToNumber(tag_it.second.default_setting);
      if (default_setting_num.has_value()) {
        adding["default"] = *default_setting_num;
      }
      id_tag_array_json.push_back(adding);
      ++id;
    }

    json["tags"] = id_tag_array_json;

    nlohmann::json file_array_json;
    for (auto file_it : file_map_) {
      nlohmann::json adding;
      adding["path"] = file_it.first;
      if (file_it.second.rating.has_value()) {
        adding["rating"] = *file_it.second.rating;
      }
      nlohmann::json yes_tags;
      nlohmann::json no_tags;
      // Anything neither yes nor no is uncommitted.
      for (auto tag_it : file_it.second.tags) {
        auto tag_id_it = tag_to_id.find(tag_it.first);
        if (tag_id_it == tag_to_id.end()) {
          // TODO: Invoke global log here.
          //log << "Tag " << tag_it.first << " does not appear in internal tag-to-id map.\n";
          continue;
        }

        const int& tag_id = tag_id_it->second;  // Alias for convenience and readability

        switch (tag_it.second) {
        case ragtag::TagSetting::YES:
          yes_tags.push_back(tag_id);
          break;
        case ragtag::TagSetting::NO:
          no_tags.push_back(tag_id);
          break;
        case ragtag::TagSetting::UNCOMMITTED:
          break;
        default:
          // TODO: Record error.
          break;
        }
      }

      adding["yes_tags"] = yes_tags;
      adding["no_tags"] = no_tags;
      file_array_json.push_back(adding);
    }

    json["files"] = file_array_json;

    return json;
  }

  std::optional<TagMap> TagMap::fromJson(const nlohmann::json& json) {
    // TODO: DEBUG
    const wxString debug_media_dir = wxStandardPaths::Get().GetDocumentsDir() + "/ragtag-debug/";
    wxFFileOutputStream output(debug_media_dir + "debug.txt");
    wxTextOutputStream log(output);

    TagMap tag_map;  // Empty TagMap that we will populate with JSON-specified contents.

    const auto tag_map_json_it = json.find("tags");
    if (tag_map_json_it == json.end()) {
      // Can't find "tags" definition.
      log << "Can't find \"tags\" definition within JSON.\n";
      return {};
    }

    std::map<int, tag_t> id_to_tag_map;
    const nlohmann::json& tag_map_json = *tag_map_json_it;  // Alias for convenience
    for (const auto& tag_it : tag_map_json) {
      const auto id_json = tag_it.find("id");
      if (id_json == tag_it.end()) {
        // Tag doesn't have "id" attribute.
        log << "Tag lacks \"id\" attribute.\n";
        continue;
      }
      const auto tag_json = tag_it.find("tag");
      if (tag_json == tag_it.end()) {
        // Tag doesn't have "tag" attribute.
        log << "Tag lacks \"tag\" attribute.\n";
        continue;
      }

      const auto default_json = tag_it.find("default");
      if (default_json == tag_it.end()) {
        // Tag doesn't have "default" attribute.
        log << "Tag lacks \"default\" attribute.\n";
        continue;
      }

      // If we've made it here, the tag entry has "id", "tag", and "default".
      if (id_to_tag_map.contains(*id_json)) {
        // Duplicate ID...
        log << "Tag ID " << int(*id_json) << " is duplicated.\n";
        continue;
      }

      TagProperties properties_pending;
      auto default_setting = numberToTagSetting(*default_json);
      if (!default_setting.has_value()) {
        // The stated default setting isn't one that we know how to interpret.
        log << "Tag has an unrecognized default value.\n";
        continue;
      }
      properties_pending.default_setting = *default_setting;

      bool insertion_successful =
        id_to_tag_map.try_emplace(*id_json, *tag_json).second;
      if (!insertion_successful) {
        // Memory allocation issue? We generally shouldn't see this.
        log << "Couldn't insert tag ID " << std::string(*id_json) << " into internal map.\n";
        continue;
      }

      // All is good! Add the tag to our fledgling TagMap.
      bool register_tag_result = tag_map.registerTag(*tag_json, properties_pending);
      if (!register_tag_result) {
        // Unclear what would cause this error.
        log << "Failed to register tag " << std::string(*tag_json) << " with TagMap object.\n";
        continue;
      }
    }

    return tag_map;
  }

  bool TagMap::toFile(const path_t& path) {
    const nlohmann::json tag_map_as_json = toJson();
    std::ofstream output_file(path);
    if (!output_file.good()) {
      return false;
    }

    output_file << tag_map_as_json;

    if (!output_file.good()) {
      return false;
    }

    return true;
  }

  std::optional<ragtag::TagMap> TagMap::fromFile(const path_t& path) {
    std::ifstream input_file(path);
    if (!input_file.good()) {
      return {};
    }

    nlohmann::json tag_map_as_json;
    input_file >> tag_map_as_json;

    if (!input_file.good()) {
      return {};
    }

    const auto tag_map_result = fromJson(tag_map_as_json);
    if (!tag_map_result) {
      return {};
    }

    return *tag_map_result;
  }

  // These numbers don't have to match the enumerator mapping so long as they form a one-to-one
  // mapping exactly reversed by numberToTagSetting().
  std::optional<int> TagMap::tagSettingToNumber(TagSetting setting) {
    switch (setting) {
    case TagSetting::NO:
      return 0;
    case TagSetting::YES:
      return 1;
    case TagSetting::UNCOMMITTED:
      return 2;
    default:
      return {};
    }
  }

  // See note on tagSettingToNumber().
  std::optional<TagSetting> TagMap::numberToTagSetting(int number) {
    switch (number) {
    case 0:
      return TagSetting::NO;
    case 1:
      return TagSetting::YES;
    case 2:
      return TagSetting::UNCOMMITTED;
    default:
      return {};
    }
  }
}  // namespace ragtag
