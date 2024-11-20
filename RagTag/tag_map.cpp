#include "tag_map.h"

namespace ragtag {
  // Ensure this is no larger than max int so that we can safely cast size_t to int in numTags().
  const int TagMap::MAX_NUM_TAGS = std::numeric_limits<int>::max();

  TagMap::TagMap() {}

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

  nlohmann::json TagMap::toJson() const {
    nlohmann::json id_tag_array_json;
    // TODO: Investigate a more compact, specialized way of handling key-value pairs.
    //for (const auto& map_it : tag_registry_) {
    //  nlohmann::json adding;
    //  adding["id"] = map_it.first;
    //  adding["tag"] = map_it.second;
    //  id_tag_array_json.push_back(adding);
    //}
    return id_tag_array_json;
  }
}  // namespace ragtag
