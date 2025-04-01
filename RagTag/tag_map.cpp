// Copyright (C) 2025 by Edward Foley
//
// This file is part of RagTag.
//
// RagTag is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// RagTag is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with RagTag. If not, see
// <https://www.gnu.org/licenses/>.

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
          << file_it.first.wstring() << L"'.\n";
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
            << file_entry.first.wstring() << L"' during tag copy operation.\n";
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

  std::optional<std::map<tag_t, TagSetting>> TagMap::getAllTagSettings(const path_t& path) const
  {
    const auto file_it = file_map_.find(path);
    if (file_it == file_map_.end()) {
      // File is not in our list.
      return {};
    }

    std::map<tag_t, TagSetting> returning;
    for (const auto& tag : tag_registry_) {
      const auto tag_it_in_file = file_it->second.tags.find(tag.first);
      if (tag_it_in_file == file_it->second.tags.end()) {
        returning.emplace(tag.first, TagSetting::UNCOMMITTED);
      }
      else {
        returning.emplace(tag.first, tag_it_in_file->second);
      }
    }

    return returning;
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

  TagCoverage TagMap::getFileTagCoverage(const ragtag::path_t& file) const
  {
    if (tag_registry_.empty()) {
      return TagCoverage::NO_TAGS_DEFINED;
    }

    const auto file_it = file_map_.find(file);
    if (file_it == file_map_.end()) {
      // File is not registered.
      return TagCoverage::NONE;
    }

    const auto& file_tags = file_it->second.tags;
    if (file_tags.empty()) {
      // Since we explicitly store only YES and NO, any file that has no stored tag data is a file
      // for which all defined tags are UNCOMMITTED.
      return TagCoverage::NONE;
    }

    if (file_tags.size() == tag_registry_.size()) {
      // ...By the same token, if this file has data stored for every tag, the file must be
      // completely covered by YES and NO settings.
      return TagCoverage::ALL;
    }

    // ...Otherwise, we must have partial coverage.
    return TagCoverage::SOME;
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
      std::wcerr << "Can't find \"tags\" definition within JSON.\n";
      return {};
    }

    std::map<int, tag_t> id_to_tag_map;
    const nlohmann::json& tag_map_json = *tag_map_json_it;  // Alias for convenience
    for (const auto& tag_it : tag_map_json) {
      const auto id_json = tag_it.find("id");
      if (id_json == tag_it.end()) {
        // Tag doesn't have "id" attribute.
        std::wcerr << "Tag lacks \"id\" attribute.\n";
        continue;
      }
      const auto tag_json = tag_it.find("tag");
      if (tag_json == tag_it.end()) {
        // Tag doesn't have "tag" attribute.
        std::wcerr << "Tag lacks \"tag\" attribute.\n";
        continue;
      }

      const auto default_json = tag_it.find("default");
      if (default_json == tag_it.end()) {
        // Tag doesn't have "default" attribute.
        std::wcerr << "Tag lacks \"default\" attribute.\n";
        continue;
      }

      const auto hotkey_json = tag_it.find("hotkey");
      // Hotkey is optional; no problem if there's no setting.

      // If we've made it here, the tag entry has "id", "tag", and "default".
      if (id_to_tag_map.contains(*id_json)) {
        // Duplicate ID...
        std::wcerr << "Tag ID " << int(*id_json) << " is duplicated.\n";
        continue;
      }

      TagProperties properties_pending;
      auto default_setting = numberToTagSetting(*default_json);
      if (!default_setting.has_value()) {
        // The stated default setting isn't one that we know how to interpret.
        std::wcerr << "Tag has an unrecognized default value.\n";
        continue;
      }
      properties_pending.default_setting = *default_setting;

      if (hotkey_json == tag_it.end()) {
        properties_pending.hotkey = {};
      } else {
        properties_pending.hotkey = *hotkey_json;
      }

      const std::wstring tag = toWString(*tag_json);
      bool insertion_successful =
        id_to_tag_map.try_emplace(*id_json, tag).second;
      if (!insertion_successful) {
        // Memory allocation issue? We generally shouldn't see this.
        std::wcerr << "Couldn't insert tag ID " << tag << " into internal map.\n";
        continue;
      }

      // All is good! Add the tag to our fledgling TagMap.
      bool register_tag_success = tag_map.registerTag(tag, properties_pending);
      if (!register_tag_success) {
        // Unclear what would cause this error.
        std::wcerr << "Failed to register tag " << tag << " with TagMap object.\n";
        continue;
      }
    }

    const auto file_map_json_it = json.find("files");
    if (file_map_json_it == json.end()) {
      // Can't find "files" definition.
      std::wcerr << "Can't find \"files\" definition within JSON.\n";
      return {};
    }

    const nlohmann::json file_map_json = *file_map_json_it;
    for (const auto& file_it : file_map_json) {
      const auto path_json = file_it.find("path");
      if (path_json == file_it.end()) {
        // File doesn't have "path" attribute.
        std::wcerr << "File lacks \"path\" attribute.\n";
        continue;
      }
      std::filesystem::path path = toWString(*path_json);

      bool add_file_success = tag_map.addFile(path);
      if (!add_file_success) {
        std::wcerr << "Failed to add file '" << path.wstring() << "' to TagMap object.\n";
        continue;
      }

      const auto rating_json = file_it.find("rating");
      if (rating_json == file_it.end()) {
        // Not an issue, since "rating" is optional.
        if (!tag_map.clearRating(path)) {
          std::wcerr << "Couldn't clear rating for file '" << path.wstring() << "'\n";
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
        std::wcerr << "File '" << path.wstring() << "' lacks 'yes_tags' definition.\n";
        continue;
      }
      nlohmann::json yes_tags_json = *yes_tags_json_it;
      for (const auto& yes_tag_id_json : yes_tags_json) {
        const int yes_tag_id = yes_tag_id_json;
        const auto yes_tag_it = id_to_tag_map.find(yes_tag_id);
        if (yes_tag_it == id_to_tag_map.end()) {
          std::wcerr << "Couldn't find yes-tag ID " << yes_tag_id
            << " within internal map for file '" << path.wstring() << "'.\n";
          continue;
        }
        if (!tag_map.setTag(path, yes_tag_it->second, TagSetting::YES)) {
          std::wcerr << L"Couldn't set tag '" << yes_tag_it->second << L"' to YES for file '"
            << path.wstring() << L"'.\n";
          continue;
        }
      }

      const auto no_tags_json_it = file_it.find("no_tags");
      if (no_tags_json_it == file_it.end()) {
        // Can't find "no_tags" definition.
        std::wcerr << "File '" << path.wstring() << "' lacks 'no_tags' definition.\n";
        continue;
      }
      nlohmann::json no_tags_json = *no_tags_json_it;
      for (const auto& no_tag_id_json : no_tags_json) {
        const int no_tag_id = no_tag_id_json;
        const auto no_tag_it = id_to_tag_map.find(no_tag_id);
        if (no_tag_it == id_to_tag_map.end()) {
          std::wcerr << "Couldn't find no-tag ID " << no_tag_id
            << " within internal map for file '" << path.wstring() << "'.\n";
          continue;
        }
        if (!tag_map.setTag(path, no_tag_it->second, TagSetting::NO)) {
          std::wcerr << L"Couldn't set tag '" << no_tag_it->second << L"' to NO for file '"
            << path.wstring() << L"'.\n";
          continue;
        }
      }
    }

    return tag_map;
  }

  bool TagMap::toFile(const path_t& path) {
    // Strategy for this function is to write to a temporary string buffer at first instead of the
    // file on disk. The reason to double-buffer like this is because some exceptions are only
    // thrown during the writing process, and when this happens to a file, it corrupts the file with
    // potentially serious data loss.
    //
    // (Yes, I learned this the hard way.)

    try {
      const nlohmann::json tag_map_as_json = toJson();

      // Start by writing JSON to the temporary string buffer...
      std::stringstream temp_stream;
      temp_stream << tag_map_as_json;  // This can throw, e.g., with non-UTF-8 chars.

      if (!temp_stream.good()) {
        std::wcerr << L"Temporary string buffer is not good() after writing to it.\n";
        return false;
      }

      std::ofstream output_file(path);
      if (!output_file.good()) {
        std::wcerr << L"Output file buffer is not good() after opening it.\n";
        return false;
      }

      // ...Now, since things seem to be in order, actually write to the file.
      output_file << temp_stream.rdbuf();

      if (!output_file.good()) {
        // This is a bad situation since we may have already modified the file on disk...
        std::wcerr << L"Output file buffer is not good() after writing to it.\n";
        return false;
      }

      return true;
    }
    catch (...) {
      // Exception happened. Don't try to write the file at all since it could get corrupted.
      std::wcerr << L"Exception thrown when writing JSON to file.\n";
      return false;
    }
  }

  std::optional<ragtag::TagMap> TagMap::fromFile(const path_t& path) {
    std::ifstream input_file(path);
    if (!input_file.good()) {
      return {};
    }

    nlohmann::json tag_map_as_json;

    try {
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
    catch (...) {
      std::wcerr << L"Exception thrown when converting file to JSON.\n";
      return {};
    }
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
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.from_bytes(string);
  }
}  // namespace ragtag
