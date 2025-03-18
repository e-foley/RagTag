#ifndef INCLUDE_RAG_TAG_UTIL_H
#define INCLUDE_RAG_TAG_UTIL_H

#include "tag_map.h"
#include <wx/string.h>
#include <string>
#include <vector>

namespace RagTagUtil {
extern const wxString GLYPH_CHECKED;
extern const wxString GLYPH_UNCOMMITTED;
extern const wxString GLYPH_UNCHECKED;
extern const wxString GLYPH_RATING_FULL_STAR;
extern const wxString GLYPH_RATING_HALF_STAR;
extern const int MAX_STARS;
extern const ragtag::path_t DEFAULT_TAG_MAP_FILE_EXTENSION;
extern const ragtag::path_t BACKUP_TAG_MAP_FILE_EXTENSION;
extern const int RAGTAG_APP_VERSION_MAJOR;
extern const int RAGTAG_APP_VERSION_MINOR;
extern const int RAGTAG_APP_VERSION_PATCH;

wxString getStarTextForRating(float rating);
bool isStaticMedia(const ragtag::path_t& path);
std::wstring getPathsAsNewlineDelineatedString(const std::vector<ragtag::path_t>& paths);
bool deleteFile(const ragtag::path_t& path);
std::wstring getRagTagAppVersionString();
void stamp(const std::string& label);
}  // namespace RagTagUtil

#endif  // INCLUDE_RAG_TAG_UTIL_H
