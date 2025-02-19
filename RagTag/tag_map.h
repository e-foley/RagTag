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
  typedef wchar_t rtchar_t;

  enum class TagSetting {
    NO = 0,
    YES = 1,
    UNCOMMITTED = 2
  };

  struct TagProperties {
    TagSetting default_setting{TagSetting::NO};
    std::optional<rtchar_t> hotkey{};

    bool operator==(const TagProperties& rhs) const noexcept {
      return default_setting == rhs.default_setting &&
        hotkey == rhs.hotkey;
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
    bool copyTag(tag_t tag, tag_t copy_name);
    bool renameTag(tag_t old_name, tag_t new_name);
    bool isTagRegistered(tag_t tag) const;
    std::optional<TagProperties> getTagProperties(tag_t tag) const;
    bool setTagProperties(tag_t tag, const TagProperties& properties);
    std::vector<std::pair<tag_t, TagProperties>> getAllTags() const;
    int numTags() const;

    // File-centric operations
    bool addFile(const path_t& path);
    bool removeFile(const path_t& path);
    bool setTag(const path_t& path, tag_t tag, TagSetting setting);
    bool clearTag(const path_t& path, tag_t tag);
    std::optional<TagSetting> getTagSetting(const path_t& path, tag_t tag) const;
    bool setRating(const path_t& path, rating_t rating);
    bool clearRating(const path_t& path);
    std::optional<rating_t> getRating(const path_t& path) const;
    bool hasFile(const path_t& path) const;
    std::optional<std::vector<tag_t>> getFileTags(const path_t& path) const;
    std::vector<path_t> getAllFiles() const;

    struct FileInfo {
    public:
      path_t path{};
      std::optional<rating_t> rating{};
      std::function<TagSetting(tag_t)> f_tag_setting{};
    };

    typedef std::function<bool(const FileInfo&)> file_qualifier_t;
    std::vector<path_t> selectFiles(const file_qualifier_t& fn) const;
    int numFiles() const;

    // Reading and writing
    nlohmann::json toJson() const;
    static std::optional<TagMap> fromJson(const nlohmann::json& json);
    bool toFile(const path_t& path);
    static std::optional<TagMap> fromFile(const path_t& path);

  private:
    struct FileProperties {
      std::optional<rating_t> rating;
      std::map<tag_t, TagSetting> tags;

      bool operator==(const FileProperties& rhs) const noexcept {
        return rating == rhs.rating &&
          tags == rhs.tags;
      }
    };

    static std::optional<int> tagSettingToNumber(TagSetting setting);
    static std::optional<TagSetting> numberToTagSetting(int number);
    std::map<tag_t, TagProperties> tag_registry_{};
    std::map<std::filesystem::path, FileProperties> file_map_{};
  };
}  // namespace ragtag

#endif  // INCLUDE_TAG_MAP_H
