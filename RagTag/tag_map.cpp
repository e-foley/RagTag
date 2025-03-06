#include "tag_map.h"
#include <algorithm>
#include <codecvt>
#include <fstream>
#include <iostream>

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
    const auto tag_it = tag_registry_.find(tag);
    if (tag_it == tag_registry_.end()) {
      // This tag already doesn't exist, so we can't remove it.
      return false;
    }

    bool was_tag_cleared_from_all_files = true;
    for (const auto& file_it : file_map_) {
      if (!clearTag(file_it.first, tag)) {
        std::wcerr << L"Couldn't clear tag '" << tag << L"' from file '"
          << file_it.first.generic_wstring() << L"'.\n";
        was_tag_cleared_from_all_files = false;
      }
    }

    // erase() returns number of elements removed.
    return was_tag_cleared_from_all_files && tag_registry_.erase(tag) > 0;
  }

  bool TagMap::copyTag(tag_t tag, tag_t copy_name)
  {
    const auto original_tag_it = tag_registry_.find(tag);
    if (original_tag_it == tag_registry_.end()) {
      // Can't find tag we're supposed to copy.
      return false;
    }

    // Assert same tag properties.
    if (!registerTag(copy_name, original_tag_it->second)) {
      // Can't create copy, possibly due to naming conflict.
      return false;
    }

    for (auto file_entry : file_map_) {
      const auto setting = getTagSetting(file_entry.first, tag);
      if (setting.has_value() && *setting != TagSetting::UNCOMMITTED) {
        if (!setTag(file_entry.first, copy_name, *setting)) {
          std::wcerr << "Could not set tag '" << copy_name << L"' on file '"
            << file_entry.first.generic_wstring() << L"' during tag copy operation.\n";
        }
      }
    }

    return true;
  }

  bool TagMap::renameTag(tag_t old_name, tag_t new_name)
  {
    // Strategy is to make a copy and delete the original. Easy-peasy!
    return copyTag(old_name, new_name) && deleteTag(old_name);
  }

  std::optional<TagProperties> TagMap::getTagProperties(const tag_t tag) const {
    const auto tag_it = tag_registry_.find(tag);
    if (tag_it == tag_registry_.end()) {
      return {};
    }
    return tag_it->second;
  }

  bool TagMap::setTagProperties(tag_t tag, const TagProperties& properties)
  {
    auto tag_it = tag_registry_.find(tag);
    if (tag_it == tag_registry_.end()) {
      return false;
    }

    tag_it->second = properties;
    return true;
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
    return file_map_.emplace(path, FileProperties{}).second;
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

    // If UNCOMMITTED, remove any existing mention of tag from the file.
    if (setting == TagSetting::UNCOMMITTED) {
      const auto current_tag_it = file_it->second.tags.find(tag);
      if (current_tag_it == file_it->second.tags.end()) {
        // Already the tag is not described by the file.
        return true;
      }
      file_it->second.tags.erase(current_tag_it);
      return true;
    }

    // Returns bool denoting whether insertion took place (true) or assignment took place (false),
    // but we don't really mind one way or the other, so we can ignore the result.
    file_it->second.tags.insert_or_assign(tag, setting).second;
    return true;
  }

  bool TagMap::clearTag(const path_t& path, const tag_t tag) {
    // For now, clearing a tag is the same as setting it to uncommitted.
    return setTag(path, tag, TagSetting::UNCOMMITTED);
  }

  std::optional<TagSetting> TagMap::getTagSetting(const path_t& path, tag_t tag) const
  {
    const auto file_it = file_map_.find(path);
    if (file_it == file_map_.end()) {
      // File is not in our list.
      return {};
    }

    const auto tag_it = file_it->second.tags.find(tag);
    if (tag_it == file_it->second.tags.end()) {
      // If tag isn't explicitly declared, it's uncommitted.
      return TagSetting::UNCOMMITTED;
    }

    return tag_it->second;
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

  std::optional<rating_t> TagMap::getRating(const path_t& path) const
  {
    const auto file_it = file_map_.find(path);
    if (file_it == file_map_.end()) {
      // File is not in our list.
      return {};
    }

    return file_it->second.rating;
  }

  bool TagMap::clearRating(const path_t& path)
  {
    const auto file_it = file_map_.find(path);
    if (file_it == file_map_.end()) {
      // File is not in our list.
      return false;
    }

    file_it->second.rating = {};
    return true;
  }

  bool TagMap::hasFile(const path_t& path) const {
    return file_map_.contains(path);
  }

  std::optional<std::vector<tag_t>> TagMap::getFileTags(const path_t& path) const
  {
    const auto file_it = file_map_.find(path);
    if (file_it == file_map_.end()) {
      // File is not in our list.
      return {};
    }

    std::vector<tag_t> tags_returning;
    for (const auto& tag_it : file_it->second.tags) {
      tags_returning.emplace_back(tag_it.first);
    }
    return tags_returning;
  }

  std::vector<path_t> TagMap::getAllFiles() const {
    std::vector<path_t> file_vector;
    file_vector.reserve(file_map_.size());
    for (const auto map_it : file_map_) {
      file_vector.emplace_back(map_it.first);
    }
    return file_vector;
  }

  std::vector<path_t> TagMap::selectFiles(const file_qualifier_t& fn) const {
    std::vector<path_t> qualified_file_vector;
    for (const auto& file : file_map_) {
      // Construct relevant FileInfo object...
      FileInfo info;
      info.path = file.first;
      info.rating = file.second.rating;
      // In effect, this function allows the invoking of getTagSetting() without an explicit path.
      // This allows the developer to focus on the traits of the tags.
      info.f_tag_setting = [this, file](tag_t tag) {
        const auto tag_setting_ret = getTagSetting(file.first, tag);
        if (!tag_setting_ret.has_value()) {
          return TagSetting::UNCOMMITTED;
        }
        else {
          return *tag_setting_ret;
        }
        };

      if (std::invoke(fn, info)) {
        qualified_file_vector.push_back(file.first);
      }
    }

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
      adding["tag"] = toUtf8(tag_it.first);
      // TODO: Another place that needs error handling attention.
      auto default_setting_num = tagSettingToNumber(tag_it.second.default_setting);
      if (default_setting_num.has_value()) {
        adding["default"] = *default_setting_num;
      }
      if (tag_it.second.hotkey.has_value()) {
        adding["hotkey"] = *tag_it.second.hotkey;
      }
      id_tag_array_json.push_back(adding);
      ++id;
    }

    json["tags"] = id_tag_array_json;

    nlohmann::json file_array_json;
    for (auto file_it : file_map_) {
      nlohmann::json adding;
      adding["path"] = toUtf8(file_it.first);
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
          std::wcerr << L"Tag " << tag_it.first << L" does not appear in internal tag-to-id map.\n";
          continue;
        }

        const int& tag_id = tag_id_it->second;  // Alias for convenience and readability

        switch (tag_it.second) {
        case TagSetting::YES:
          yes_tags.push_back(tag_id);
          break;
        case TagSetting::NO:
          no_tags.push_back(tag_id);
          break;
        case TagSetting::UNCOMMITTED:
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
    TagMap tag_map;  // Empty TagMap that we will populate with JSON-specified contents.

    const auto tag_map_json_it = json.find("tags");
    if (tag_map_json_it == json.end()) {
      // Can't find "tags" definition.
      std::cerr << "Can't find \"tags\" definition within JSON.\n";
      return {};
    }

    std::map<int, tag_t> id_to_tag_map;
    const nlohmann::json& tag_map_json = *tag_map_json_it;  // Alias for convenience
    for (const auto& tag_it : tag_map_json) {
      const auto id_json = tag_it.find("id");
      if (id_json == tag_it.end()) {
        // Tag doesn't have "id" attribute.
        std::cerr << "Tag lacks \"id\" attribute.\n";
        continue;
      }
      const auto tag_json = tag_it.find("tag");
      if (tag_json == tag_it.end()) {
        // Tag doesn't have "tag" attribute.
        std::cerr << "Tag lacks \"tag\" attribute.\n";
        continue;
      }

      const auto default_json = tag_it.find("default");
      if (default_json == tag_it.end()) {
        // Tag doesn't have "default" attribute.
        std::cerr << "Tag lacks \"default\" attribute.\n";
        continue;
      }

      const auto hotkey_json = tag_it.find("hotkey");
      // Hotkey is optional; no problem if there's no setting.

      // If we've made it here, the tag entry has "id", "tag", and "default".
      if (id_to_tag_map.contains(*id_json)) {
        // Duplicate ID...
        std::cerr << "Tag ID " << int(*id_json) << " is duplicated.\n";
        continue;
      }

      TagProperties properties_pending;
      auto default_setting = numberToTagSetting(*default_json);
      if (!default_setting.has_value()) {
        // The stated default setting isn't one that we know how to interpret.
        std::cerr << "Tag has an unrecognized default value.\n";
        continue;
      }
      properties_pending.default_setting = *default_setting;

      if (hotkey_json == tag_it.end()) {
        properties_pending.hotkey = {};
      } else {
        properties_pending.hotkey = *hotkey_json;
      }

      bool insertion_successful =
        id_to_tag_map.try_emplace(*id_json, toWString(*tag_json)).second;
      if (!insertion_successful) {
        // Memory allocation issue? We generally shouldn't see this.
        std::cerr << "Couldn't insert tag ID " << std::string(*id_json) << " into internal map.\n";
        continue;
      }

      // All is good! Add the tag to our fledgling TagMap.
      bool register_tag_success = tag_map.registerTag(toWString(*tag_json), properties_pending);
      if (!register_tag_success) {
        // Unclear what would cause this error.
        std::cerr << "Failed to register tag " << std::string(*tag_json) << " with TagMap object.\n";
        continue;
      }
    }

    const auto file_map_json_it = json.find("files");
    if (file_map_json_it == json.end()) {
      // Can't find "files" definition.
      std::cerr << "Can't find \"files\" definition within JSON.\n";
      return {};
    }

    const nlohmann::json file_map_json = *file_map_json_it;
    for (const auto& file_it : file_map_json) {
      const auto path_json = file_it.find("path");
      if (path_json == file_it.end()) {
        // File doesn't have "path" attribute.
        std::cerr << "File lacks \"path\" attribute.\n";
        continue;
      }
      std::filesystem::path path = *path_json;

      bool add_file_success = tag_map.addFile(path);
      if (!add_file_success) {
        std::cerr << "Failed to add file '" << path.generic_string() << "' to TagMap object.\n";
        continue;
      }

      const auto rating_json = file_it.find("rating");
      if (rating_json == file_it.end()) {
        // Not an issue, since "rating" is optional.
        if (!tag_map.clearRating(path)) {
          std::cerr << "Couldn't clear rating for file '" << path.generic_string() << "'\n";
          continue;
        }
      }
      else {
        if (!tag_map.setRating(path, *rating_json)) {
          // TODO: Shouldn't happen; log error.
          continue;
        }
      }

      const auto yes_tags_json_it = file_it.find("yes_tags");
      if (yes_tags_json_it == file_it.end()) {
        // Can't find "yes_tags" definition.
        std::cerr << "File '" << path.generic_string() << "' lacks 'yes_tags' definition.\n";
        continue;
      }
      nlohmann::json yes_tags_json = *yes_tags_json_it;
      for (const auto& yes_tag_id_json : yes_tags_json) {
        const int yes_tag_id = yes_tag_id_json;
        const auto yes_tag_it = id_to_tag_map.find(yes_tag_id);
        if (yes_tag_it == id_to_tag_map.end()) {
          std::cerr << "Couldn't find yes-tag ID " << yes_tag_id
            << " within internal map for file '" << path.generic_string() << "'.\n";
          continue;
        }
        if (!tag_map.setTag(path, yes_tag_it->second, TagSetting::YES)) {
          std::wcerr << L"Couldn't set tag '" << yes_tag_it->second << L"' to YES for file '"
            << path.generic_wstring() << L"'.\n";
          continue;
        }
      }

      const auto no_tags_json_it = file_it.find("no_tags");
      if (no_tags_json_it == file_it.end()) {
        // Can't find "no_tags" definition.
        std::cerr << "File '" << path.generic_string() << "' lacks 'no_tags' definition.\n";
        continue;
      }
      nlohmann::json no_tags_json = *no_tags_json_it;
      for (const auto& no_tag_id_json : no_tags_json) {
        const int no_tag_id = no_tag_id_json;
        const auto no_tag_it = id_to_tag_map.find(no_tag_id);
        if (no_tag_it == id_to_tag_map.end()) {
          std::wcerr << "Couldn't find no-tag ID " << no_tag_id
            << " within internal map for file '" << path.generic_wstring() << "'.\n";
          continue;
        }
        if (!tag_map.setTag(path, no_tag_it->second, TagSetting::NO)) {
          std::wcerr << L"Couldn't set tag '" << no_tag_it->second << L"' to NO for file '"
            << path.generic_wstring() << L"'.\n";
          continue;
        }
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

  // Implementation copied from from https://json.nlohmann.me/home/faq/#wide-string-handling.
  std::string TagMap::toUtf8(const std::wstring& wide_string)
  {
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(wide_string);
  }

  std::wstring TagMap::toWString(const std::string& string) {
    return std::wstring(string.begin(), string.end());
  }
}  // namespace ragtag
