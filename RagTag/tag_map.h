#ifndef INCLUDE_TAG_MAP_H
#define INCLUDE_TAG_MAP_H

#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <utility>  // std::pair
#include <vector>
#include <nlohmann/json.hpp>

namespace ragtag {
  typedef std::string tag_t;
  typedef std::filesystem::path path_t;
  typedef float rating_t;

  enum class TagSetting {
    NO = 0,
    YES = 1,
    UNCOMMITTED = 2
  };

  struct TagProperties {
    TagSetting default_setting{TagSetting::NO};

    bool operator==(const TagProperties& rhs) const noexcept {
      return default_setting == rhs.default_setting;
    }
  };

  struct FileProperties {
    std::optional<rating_t> rating;
    std::map<tag_t, TagSetting> tags;

    bool operator==(const FileProperties& rhs) const noexcept {
      return rating == rhs.rating &&
        tags == rhs.tags;
    }
  };

  class TagMap {
  public:
    static const int MAX_NUM_TAGS;
    static const int MAX_NUM_FILES;

    TagMap();
    bool operator==(const TagMap& rhs) const noexcept;

    // Tag-centric operations
    bool registerTag(tag_t tag);
    bool registerTag(tag_t tag, const TagProperties& properties);
    bool deleteTag(tag_t tag);
    bool isTagRegistered(tag_t tag) const;
    std::optional<TagProperties> getTagProperties(tag_t tag) const;
    std::vector<std::pair<tag_t, TagProperties>> getAllTags() const;
    int numTags() const;

    // File-centric operations
    bool addFile(const path_t& path);
    bool addFile(const path_t& path, const FileProperties& properties);
    bool removeFile(const path_t& path);
    bool setTag(const path_t& path, tag_t tag, TagSetting setting);
    bool setRating(const path_t& path, rating_t rating);
    bool hasFile(const path_t& path) const;
    // TODO: Implement getTags(), getRating()
    std::optional<FileProperties> getFileProperties(const path_t& path) const;
    std::vector<std::pair<path_t, FileProperties>> getAllFiles() const;

    typedef std::function<bool(const FileProperties&)> file_qualifier_t;
    std::vector<std::pair<path_t, FileProperties>> selectFiles(const file_qualifier_t& fn) const;
    int numFiles() const;

    // Reading and writing
    nlohmann::json toJson() const;
    static std::optional<TagMap> fromJson(const nlohmann::json& json);

  private:
    std::map<tag_t, TagProperties> tag_registry_{};
    std::map<std::filesystem::path, FileProperties> file_map_{};
  };
}  // namespace ragtag

#endif  // INCLUDE_TAG_MAP_H
