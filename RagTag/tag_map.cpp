#include "tag_map.h"

TagMap::TagMap() {}

bool TagMap::addTag(id_t id, tag_t tag)
{
  return id_to_tag_map_.emplace(id, tag).second;
}

std::optional<TagMap::tag_t> TagMap::getTag(id_t id) const
{
  const auto tag_it = id_to_tag_map_.find(id);
  if (tag_it == id_to_tag_map_.end()) {
    return {};
  }
  return tag_it->second;
}
