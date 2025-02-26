#ifndef INCLUDE_RAG_TAG_UTIL_H
#define INCLUDE_RAG_TAG_UTIL_H

#include <wx/string.h>

namespace RagTagUtil {
extern const wxString GLYPH_CHECKED;
extern const wxString GLYPH_UNCOMMITTED;
extern const wxString GLYPH_UNCHECKED;
extern const wxString GLYPH_RATING_FULL_STAR;
extern const wxString GLYPH_RATING_HALF_STAR;
extern const int MAX_STARS;

wxString getStarTextForRating(float rating);
}  // namespace RagTagUtil

#endif  // INCLUDE_RAG_TAG_UTIL_H
