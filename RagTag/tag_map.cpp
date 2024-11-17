#include "tag_map.h"

// Ensure this is no larger than max int so that we can safely cast size_t to int in numTags().
const int TagMap::MAX_NUM_TAGS = std::numeric_limits<int>::max();

TagMap::TagMap() {}

bool TagMap::addTag(id_t id, tag_t tag)
{
  if (numTags() >= MAX_NUM_TAGS) {
    return false;
  }

  return id_to_tag_map_.emplace(id, tag).second;
}

bool TagMap::removeTag(id_t id) {
  // erase returns number of elements removed
  return id_to_tag_map_.erase(id) > 0;
}

std::optional<TagMap::tag_t> TagMap::getTag(id_t id) const
{
  const auto tag_it = id_to_tag_map_.find(id);
  if (tag_it == id_to_tag_map_.end()) {
    return {};
  }
  return tag_it->second;
}

int TagMap::numTags() const
{
  // Safe conversion provided MAX_NUM_TAGS is enforced.
  return static_cast<int>(id_to_tag_map_.size());
}