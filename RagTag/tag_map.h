#ifndef INCLUDE_TAG_MAP_H
#define INCLUDE_TAG_MAP_H

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <utility>  // std::pair
#include <vector>
#include <nlohmann/json.hpp>

namespace ragtag {
  typedef int id_t;
  typedef std::string tag_t;

  struct FileProperties {
    std::optional<float> rating;
    std::map<tag_t, std::optional<bool>> tags;
  };

  class TagMap {
  public:
    static const int MAX_NUM_TAGS;

    TagMap();
    bool registerTag(id_t id, tag_t tag);
    bool deleteTag(id_t id);
    std::optional<tag_t> getTag(id_t id) const;
    std::vector<std::pair<id_t, tag_t>> getAllTags() const;
    int numTags() const;
    nlohmann::json toJson() const;

  private:
    std::map<id_t, tag_t> id_to_tag_map_{};
    std::map<std::filesystem::path, FileProperties> file_map_{};
  };
}  // namespace ragtag

#endif  // INCLUDE_TAG_MAP_H
