#ifndef INCLUDE_TAG_MAP_H
#define INCLUDE_TAG_MAP_H

#include <map>
#include <optional>
#include <string>
#include <utility>  // std::pair
#include <vector>
#include <nlohmann/json.hpp>

class TagMap {
public:
  typedef int id_t;
  typedef std::string tag_t;

  static const int MAX_NUM_TAGS;

  TagMap();
  bool addTag(id_t id, tag_t tag);
  bool removeTag(id_t id);
  std::optional<tag_t> getTag(id_t id) const;
  std::vector<std::pair<id_t, tag_t>> getAllTags() const;
  int numTags() const;
  nlohmann::json toJson() const;

private:
  std::map<id_t, tag_t> id_to_tag_map_{};
};

#endif  // INCLUDE_TAG_MAP_H
