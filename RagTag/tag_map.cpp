#include "tag_map.h"

namespace ragtag {
  // Ensure these are no larger than max int so that we can safely cast size_t to int.
  const int TagMap::MAX_NUM_TAGS = std::numeric_limits<int>::max();
  const int TagMap::MAX_NUM_FILES = std::numeric_limits<int>::max();

  TagMap::TagMap() {}

  bool TagMap::operator==(const TagMap& rhs) const noexcept {
    return tag_registry_ == rhs.tag_registry_ &&
      file_map_ == rhs.file_map_;
  }

  bool TagMap::registerTag(tag_t tag) {
    return registerTag(tag, TagProperties{});
  }

  bool TagMap::registerTag(tag_t tag, const TagProperties& properties) {
    if (numTags() >= MAX_NUM_TAGS) {
      return false;
    }

    return tag_registry_.emplace(tag, properties).second;
  }

  bool TagMap::deleteTag(tag_t tag) {
    // erase() returns number of elements removed.
    return tag_registry_.erase(tag) > 0;
  }

  std::optional<TagProperties> TagMap::getTagProperties(tag_t tag) const {
    const auto tag_it = tag_registry_.find(tag);
    if (tag_it == tag_registry_.end()) {
      return {};
    }
    return tag_it->second;
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

  int TagMap::numFiles() const {
    // Safe conversion provided MAX_NUM_FILES is enforced.
    return static_cast<int>(file_map_.size());
  }

  nlohmann::json TagMap::toJson() const {
    // To allow a (relatively) compact representation of our table, assign each tag an ID.
    std::map<tag_t, int> tag_to_id;
    int id = 1;  // Start at 1 so that we can use 0 as some kind of default value if we want.
    for (auto map_it : tag_registry_) {
      // TODO: Implement error handling.
      tag_to_id.try_emplace(map_it.first, id);
      ++id;
    }

    nlohmann::json json;
    nlohmann::json id_tag_array_json;
    // TODO: Investigate a more compact, specialized way of handling key-value pairs.
    for (const auto& tag_it : tag_to_id) {
      nlohmann::json adding;
      adding["id"] = tag_it.second;
      adding["tag"] = tag_it.first;
      id_tag_array_json.push_back(adding);
    }

    json["tags"] = id_tag_array_json;
    return json;
  }

  std::optional<TagMap> TagMap::fromJson(const nlohmann::json& json) {
    return TagMap{};
  }
}  // namespace ragtag
