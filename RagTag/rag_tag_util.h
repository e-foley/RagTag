#ifndef INCLUDE_RAG_TAG_UTIL_H
#define INCLUDE_RAG_TAG_UTIL_H

#include "tag_map.h"
#include <wx/string.h>

namespace RagTagUtil {
extern const wxString GLYPH_CHECKED;
extern const wxString GLYPH_UNCOMMITTED;
extern const wxString GLYPH_UNCHECKED;
extern const wxString GLYPH_RATING_FULL_STAR;
extern const wxString GLYPH_RATING_HALF_STAR;
extern const int MAX_STARS;
extern const ragtag::path_t DEFAULT_TAG_MAP_FILE_EXTENSION;
extern const ragtag::path_t BACKUP_TAG_MAP_FILE_EXTENSION;

wxString getStarTextForRating(float rating);
bool isStaticMedia(const ragtag::path_t& path);
}  // namespace RagTagUtil

#endif  // INCLUDE_RAG_TAG_UTIL_H
