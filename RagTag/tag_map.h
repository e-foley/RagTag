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

  struct TagProperties {
    bool foo{false};  // Temp placeholder
  };

  struct FileProperties {
    std::optional<float> rating;
    std::map<tag_t, std::optional<bool>> tags;
  };

  class TagMap {
  public:
    static const int MAX_NUM_TAGS;

    TagMap();
    bool registerTag(tag_t tag);
    bool registerTag(tag_t tag, const TagProperties& properties);
    bool deleteTag(tag_t tag);
    std::optional<TagProperties> getTagProperties(tag_t tag) const;
    std::vector<std::pair<tag_t, TagProperties>> getAllTags() const;
    int numTags() const;
    nlohmann::json toJson() const;

  private:
    std::map<tag_t, TagProperties> tag_registry_{};
    std::map<std::filesystem::path, FileProperties> file_map_{};
  };
}  // namespace ragtag

#endif  // INCLUDE_TAG_MAP_H
