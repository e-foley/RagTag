#include "tag_map.h"

namespace ragtag {
  // Ensure this is no larger than max int so that we can safely cast size_t to int in numTags().
  const int TagMap::MAX_NUM_TAGS = std::numeric_limits<int>::max();

  TagMap::TagMap() {}

  bool TagMap::addTag(id_t id, tag_t tag) {
    if (numTags() >= MAX_NUM_TAGS) {
      return false;
    }

    return id_to_tag_map_.emplace(id, tag).second;
  }

  bool TagMap::removeTag(id_t id) {
    // erase returns number of elements removed
    return id_to_tag_map_.erase(id) > 0;
  }

  std::optional<tag_t> TagMap::getTag(id_t id) const {
    const auto tag_it = id_to_tag_map_.find(id);
    if (tag_it == id_to_tag_map_.end()) {
      return {};
    }
    return tag_it->second;
  }

  std::vector<std::pair<id_t, tag_t>> TagMap::getAllTags() const {
    std::vector<std::pair<id_t, tag_t>> tag_vector;
    tag_vector.reserve(id_to_tag_map_.size());
    for (const auto map_it : id_to_tag_map_) {
      tag_vector.emplace_back(map_it);
    }
    return tag_vector;
  }

  int TagMap::numTags() const {
    // Safe conversion provided MAX_NUM_TAGS is enforced.
    return static_cast<int>(id_to_tag_map_.size());
  }

  nlohmann::json TagMap::toJson() const {
    nlohmann::json id_tag_array_json;
    // TODO: Investigate a more compact, specialized way of handling key-value pairs.
    for (const auto& map_it : id_to_tag_map_) {
      nlohmann::json adding;
      adding["id"] = map_it.first;
      adding["tag"] = map_it.second;
      id_tag_array_json.push_back(adding);
    }
    return id_tag_array_json;
  }
}  // namespace ragtag
