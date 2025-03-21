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

//! Namespace for the low-level RagTag library interface.
namespace ragtag {
  //! Type used to describe tags.
  typedef std::wstring tag_t;
  //! Type used to describe paths.
  typedef std::filesystem::path path_t;
  //! Type used to describe file ratings.
  typedef float rating_t;
  //! Type used to describe hotkeys.
  typedef wchar_t rtchar_t;

  //! Way to describe a tag's applicability to a file.
  enum class TagSetting {
    NO = 0,          //!< Tag does not apply to this file.
    YES = 1,         //!< Tag applies to this file.
    UNCOMMITTED = 2  //!< The tag's applicability to this file is not specified.
  };

  //! Categories of "tag coverage," which simply describes the proportion of defined tags that have
  //! been attached to a file.
  enum class TagCoverage {
    NONE,  //!< All tags defined in the project are unspecified for this file.
    SOME,  //!< Some tags defined in the project are committed to yes or no; others are unspecified.
    ALL,  //!< All tags defined in the project have been committed either yes or no for this file.
    NO_TAGS_DEFINED  //!< No tags are defined in the project; coverage is indeterminate.
  };

  //! Structure describing tag traits.
  struct TagProperties {
    //! The default setting of the tag.
    //! 
    //! This has no function within the TagMap interface but is encoded in the representation of the
    //! TagMap for use by other interfaces.
    TagSetting default_setting{TagSetting::NO};
    //! Hotkey associated with this tag or an empty optional if no hotkey.
    //! 
    //! This has no function within the TagMap interface but is encoded in the JSON representation
    //! of the TagMap for use by other interfaces.
    std::optional<rtchar_t> hotkey{};

    //! Tests equality of this TagProperties instance and another TagProperties instance.
    //! 
    //! Equality is defined by matching default settings and matching hotkeys.
    //! 
    //! @param rhs TagProperties instance to compare this instance with.
    //! @returns True if this TagProperties instance is equal to another TagProperties instance.
    bool operator==(const TagProperties& rhs) const noexcept {
      return default_setting == rhs.default_setting
          && hotkey == rhs.hotkey;
    }
  };

  //! Database of files, descriptors, and the relationship between the two.
  //!  
  //! Descriptors take the form of tags and ratings. Files must be added to the TagMap via addFile()
  //! before descriptors can be applied to files.
  //! 
  //! Tags must be registered via registerTag() before they can be applied to registered files via
  //! setTag().
  //! 
  //! All tags are considered to have a setting of TagSetting::UNCOMMITTED on files by default. Use
  //! setTag() to change the tag's setting to something else.
  //! 
  //! Each registered file may have at most one rating, which can be assigned via setRating().
  class TagMap {
  public:
    //! Maximum number of tags that can be actively registered with the TagMap.
    static const int MAX_NUM_TAGS;
    //! Maximum number of files that can be present within the TagMap.
    static const int MAX_NUM_FILES;

    //! Default constructor.
    //! 
    //! Produces a TagMap with no files and no registered tags.
    TagMap();

    //! Tests equality of this TagMap and another.
    //! 
    //! TagMaps are equal if they describe identical files, tags, and file descriptors.
    //! 
    //! @param rhs The TagMap to compare this object against.
    //! @returns True if the two TagMaps are equal.
    bool operator==(const TagMap& rhs) const noexcept;

    // TAG-CENTRIC OPERATIONS ======================================================================
    //! Registers a tag with the TagMap, thus allowing it to be assigned to files in the TagMap.
    //! 
    //! The tag is assigned default TagProperties.
    //! 
    //! @param tag The name of the tag.
    //! @returns True if the tag is successfully registered.
    bool registerTag(tag_t tag);

    //! Registers a tag and associated properties with the TagMap, thus allowing the tag to be
    //! assigned to files in the TagMap.
    //! 
    //! @param tag The name of the tag.
    //! @param properties Properties to associate with the tag.
    //! @returns True if the tag is successfully registered.
    bool registerTag(tag_t tag, const TagProperties& properties);

    //! Deletes a tag and removes its descriptor from all files in the TagMap.
    //! 
    //! @param tag The tag to delete.
    //! @returns True if the tag is deleted and removed from all files in the TagMap.
    bool deleteTag(tag_t tag);

    //! Duplicates a tag and its existing associations with files in the TagMap.
    //! 
    //! Duplicating a tag leads to a new tag being registered as if by registerTag().
    //! 
    //! @param tag The name of the tag to copy.
    //! @param copy_name The name to assign the duplicate tag.
    //! @returns True if the tag and its associations with files are successfully duplicated.
    bool copyTag(tag_t tag, tag_t copy_name);

    //! Renames a tag.
    //! 
    //! Files that described the tag with the old name now describe the tag with the new name (and
    //! maintain the same settings).
    //! 
    //! @param old_name The original name of the tag.
    //! @param new_name The new name of the tag.
    //! @returns True if the renaming is successful and the associations of this tag with files in
    //!     the TagMap are updated accordingly.
    bool renameTag(tag_t old_name, tag_t new_name);

    //! Tests whether a tag is registered in the TagMap.
    //! 
    //! @param tag The tag to test the registration status of.
    //! @returns True if the tag is currently registered.
    bool isTagRegistered(tag_t tag) const;

    //! Retrieves TagProperties of a tag.
    //! 
    //! @param tag The name of the tag to retrieve properties of.
    //! @returns The TagProperties assigned to the tag or an empty optional if the properties cannot
    //!     be retrieved.
    std::optional<TagProperties> getTagProperties(tag_t tag) const;

    //! Modifies properties of an existing tag.
    //! 
    //! @param tag The tag to modify the properties of.
    //! @param properties The new properties to assign to the tag.
    //! @returns True if the properties are successfully updated.
    bool setTagProperties(tag_t tag, const TagProperties& properties);

    //! Retrieves a list of all tags and related settings.
    //! 
    //! @returns A list of all tags and related settings grouped as pairs.
    std::vector<std::pair<tag_t, TagProperties>> getAllTags() const;

    //! Obtains the count of all registered tags in this TagMap.
    //! 
    //! @returns The number of tags registered in this TagMap.
    int numTags() const;

    // FILE-CENTRIC OPERATIONS =====================================================================
    //! Adds a file to the TagMap.
    //! 
    //! The file is added with no descriptors.
    //! 
    //! @param path The path of the file to add.
    //! @returns True if the file is added successfully.
    bool addFile(const path_t& path);

    //! Removes a file from the TagMap.
    //! 
    //! @param path The path of the file to remove.
    //! @returns True if the file is removed successfully.
    bool removeFile(const path_t& path);

    //! Sets or modifies a tag on a file.
    //! 
    //! @param path The path of the file to set a tag on.
    //! @param tag The name of the tag to set.
    //! @param setting The setting to associate with the tag on the file.
    //! @returns True if the tag is successfully set to the desired value on the file.
    bool setTag(const path_t& path, tag_t tag, TagSetting setting);

    //! Removes a tag from a file.
    //! 
    //! Removing a tag from a file is functionally the same as setting it to TagSetting::UNCOMMITTED
    //! via setTag().
    //! 
    //! @param path The path of the file to clear a tag from.
    //! @param tag The tag to remove from the file.
    //! @returns True if the tag is successfully removed from the file.
    bool clearTag(const path_t& path, tag_t tag);

    //! Retrieves a tag setting for a file.
    //! 
    //! The function returns TagSetting::UNCOMMITTED for tags that are registered but haven't been
    //! explicitly set on the file.
    //! 
    //! @param path The path of the file to retrieve a tag setting for.
    //! @param tag The tag of interest.
    //! @returns The TagSetting for this tag on this file or an empty optional if the retrieval
    //!     of this information fails.
    std::optional<TagSetting> getTagSetting(const path_t& path, tag_t tag) const;

    //! Retrieves all tag settings for a file.
    //! 
    //! Tags will appear with a TagSetting of TagSetting::UNCOMMITTED if they have been registered
    //! but not otherwise explicitly assigned to the file.
    //! 
    //! @param path The path of the file to retrieve tag settings for.
    //! @returns A map of tags and associated TagSettings for the file, or an empty optional if the
    //!     retrieval fails.
    std::optional<std::map<tag_t, TagSetting>> getAllTagSettings(const path_t& path) const;

    //! Sets a rating on a file.
    //! 
    //! @param path The path of the file to set a rating on.
    //! @param rating The rating to assign the file.
    //! @returns True if the rating assignment is successful.
    bool setRating(const path_t& path, rating_t rating);

    //! Removes a rating from a file.
    //! 
    //! Removing a rating is not the same as setting a rating to 0. Instead, this function removes
    //! the association of any rating from this file such that future calls to getRating() on this
    //! file will not produce any number (until setRating() is called).
    //! 
    //! @param path The path of the file to remove a rating from.
    //! @returns True if the rating is successfully removed from the file.
    bool clearRating(const path_t& path);

    //! Retrieves the rating for a file.
    //! 
    //! @note There are two circumstances in which this function returns its default value: the file
    //! may not exist or there may be no rating assigned to the existing file.
    //! 
    //! @param path The path of the file to retrieve the rating of.
    //! @returns The rating assigned to this file or an empty optional in the case that either the
    //!     lookup fails or the file has no rating assigned.
    std::optional<rating_t> getRating(const path_t& path) const;

    //! Checks whether the TagMap has a specified file.
    //! 
    //! @param path The path of the file to test the presence of.
    //! @returns True if the file is present in the TagMap.
    bool hasFile(const path_t& path) const;

    //! Procures a list of tags that have been explicitly set to yes or no on this file.
    //! 
    //! @param path The file to retrieve a list of tags for.
    //! @returns The list of tags that have been set to either yes or no on this file, or an empty
    //!     optional if the lookup fails.
    std::optional<std::vector<tag_t>> getFileTags(const path_t& path) const;

    //! Retrieves a list of all files in the TagMap.
    //! 
    //! @returns A list of paths of all files in the TagMap.
    std::vector<path_t> getAllFiles() const;

    //! Determines and provides the tag coverage for a specified file.
    //! 
    //! If the file is not in the TagMap, returns TagCoverage::NONE.
    //! 
    //! See the TagCoverage enumeration for more information on the return value.
    //! 
    //! @param file The file to determine the tag coverage of.
    //! @returns The TagCoverage of the specified file.
    TagCoverage getFileTagCoverage(const ragtag::path_t& file) const;

    //! Container of information about files and associated tags that is provided to file selection
    //! functions.
    //! 
    //! See file_qualifier_t and selectFiles() for more information.
    struct FileInfo {
    public:
      //! Path of the file.
      path_t path{};
      //! The file's rating or an empty optional if the file has no rating.
      std::optional<rating_t> rating{};
      //! Function that produces the TagSetting for a given tag on this file.
      std::function<TagSetting(tag_t)> f_tag_setting{};
    };

    //! Function that qualifies a file for inclusion in a group based on properties of the file
    //! shared via FileInfo.
    //! 
    //! A return value of true indicates that the file qualifies.
    typedef std::function<bool(const FileInfo&)> file_qualifier_t;

    //! Selects files in the TagMap based on specified criteria.
    //! 
    //! @param fn File selection criteria.
    //! @returns The paths of files that satisfy the criteria.
    std::vector<path_t> selectFiles(const file_qualifier_t& fn) const;

    //! Obtains the count of all files in the TagMap.
    //! 
    //! @returns The count of all files in the TagMap.
    int numFiles() const;

    // READING AND WRITING =========================================================================
    //! Generate and retrieve a JSON representation of this TagMap.
    //!
    //! @returns A JSON representation of this TagMap.
    nlohmann::json toJson() const;

    //! Produces a TagMap from JSON data.
    //! 
    //! @param json The JSON representation of a TagMap.
    //! @returns The TagMap produced from the JSON data or an empty optional if the conversion
    //!     fails.
    static std::optional<TagMap> fromJson(const nlohmann::json& json);

    //! Saves a JSON representation of this TagMap to disk.
    //! 
    //! @param path The path of the JSON file to save.
    //! @returns True if the save operation is successful.
    bool toFile(const path_t& path);

    //! Produces a TagMap from a JSON file on disk.
    //! 
    //! @param path Path to the JSON file.
    //! @returns The TagMap produced from the JSON file or an empty optional if the conversion
    //!     fails.
    static std::optional<TagMap> fromFile(const path_t& path);

  private:
    //! Internal helper struct to collect properties associated with files.
    struct FileProperties {
      //! File rating or an empty optional if no rating.
      std::optional<rating_t> rating;

      //! All tags with settings other than TagSetting::UNCOMMITTED.
      std::map<tag_t, TagSetting> tags;

      //! Test equality of this FileProperties instance with another.
      //! 
      //! @param rhs The FileProperties instance to test equality with.
      //! @returns True if the two FileProperties instances are equal.
      bool operator==(const FileProperties& rhs) const noexcept {
        return rating == rhs.rating &&
          tags == rhs.tags;
      }
    };

    //! Converts a TagSetting to a number for use in encoding the setting into JSON.
    //! 
    //! @param setting The TagSetting.
    //! @returns A number uniquely identifying the TagSetting or an empty optional if there is no
    //!     such number.
    static std::optional<int> tagSettingToNumber(TagSetting setting);

    //! Converts a JSON-encoded number into a TagSetting.
    //! 
    //! @param number The number to convert to a TagSetting.
    //! @returns The TagSetting corresponding to this number or an empty optional if there is no
    //!     such TagSetting.
    static std::optional<TagSetting> numberToTagSetting(int number);

    //! Converts a wide string to a UTF-8-encoded representation.
    //! 
    //! @param wide_string The string to convert.
    //! @returns The UTF-8-encoded string.
    static std::string toUtf8(const std::wstring& wide_string);

    //! Converts a string to a wide string.
    //! 
    //! @param strsing The string to convert.
    //! @returns The wide-string equivalent of the input string.
    static std::wstring toWString(const std::string& string);

    //! Map of all tags and associated properties.
    std::map<tag_t, TagProperties> tag_registry_{};

    //! Map of all files and associated properties.
    std::map<std::filesystem::path, FileProperties> file_map_{};
  };
}  // namespace ragtag

#endif  // INCLUDE_TAG_MAP_H
