#include "rag_tag_util.h"
#include <string>

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

bool RagTagUtil::isStaticMedia(const ragtag::path_t& path)
{
  // Trusting that the 2009 list from this link is still accurate:
  // https://answers.microsoft.com/en-us/windows/forum/all/windows-media-player-cant-display-pictures/a73834b3-bc70-4fdf-85ea-74b83d5f9c01
  static std::vector<std::wstring> static_types = {
    L".bmp", L".gif", L".jpg", L".jpeg", L".png", L".tif", L".tiff", L".wmf"
  };
  std::wstring extension = path.extension().generic_wstring();
  // Lowercase conversion from https://stackoverflow.com/a/313990
  std::transform(extension.begin(), extension.end(), extension.begin(),
    [](wchar_t c) { return std::tolower(c); });
  for (const auto& ext_it : static_types) {
    if (extension == ext_it) {
      return true;
    }
  }

  return false;
}

