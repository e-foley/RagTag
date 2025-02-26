#include "rag_tag_util.h"

// The font used by wxWidgets does not display half-star characters as of writing.
// #define HALF_STAR_GLYPH_SUPPORTED

const wxString RagTagUtil::GLYPH_CHECKED = L"\U00002611";  // U+2611 is a checkmark in a box.
const wxString RagTagUtil::GLYPH_UNCOMMITTED = L"\U00002012";  // U+2012 is a figure dash.
const wxString RagTagUtil::GLYPH_UNCHECKED = L"\U00002610";  // U+2610 is an empty checkbox.
const wxString RagTagUtil::GLYPH_RATING_FULL_STAR = L"\U00002605";  // U+2605 is a full star.
#ifdef HALF_STAR_GLYPH_SUPPORTED
const wxString RagTagUtil::GLYPH_RATING_HALF_STAR = L"\U00002BE8";  // U+2BE8 is a half star.
#else
const wxString RagTagUtil::GLYPH_RATING_HALF_STAR = L"\U000000BD";  // U+00BD is a half fraction.
#endif
const int RagTagUtil::MAX_STARS = 5;

wxString RagTagUtil::getStarTextForRating(float rating)
{
  wxString returning = wxEmptyString;
  for (int i = 1; i <= MAX_STARS; ++i) {
    if (rating >= static_cast<float>(i)) {
      returning.Append(GLYPH_RATING_FULL_STAR);
    }
    else if (rating >= static_cast<float>(i) - 0.5f) {
      returning.Append(GLYPH_RATING_HALF_STAR);
    }
  }

  return returning;
}
