#include "rag_tag_util.h"
#include <format>
#include <wx/wx.h>

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
const ragtag::path_t RagTagUtil::DEFAULT_TAG_MAP_FILE_EXTENSION = L".tagdef";
const ragtag::path_t RagTagUtil::BACKUP_TAG_MAP_FILE_EXTENSION = L".tagdefbk";
const int RagTagUtil::RAGTAG_APP_VERSION_MAJOR = 0;
const int RagTagUtil::RAGTAG_APP_VERSION_MINOR = 1;
const int RagTagUtil::RAGTAG_APP_VERSION_PATCH = 0;

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
    L".bmp", L".gif", L".jpg", L".jpeg", L".jxr", L".png", L".tif", L".tiff", L".wmf"
  };
  std::wstring extension = path.extension().wstring();
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

std::wstring RagTagUtil::getPathsAsNewlineDelineatedString(const std::vector<ragtag::path_t>& paths)
{
  std::wstring building;
  for (int i = 0; i < paths.size(); ++i) {
    if (i > 0) {
      building += L"\n";
    }
    building += paths[i].wstring();
  }
  return building;
}

// Approach from SO user Zeltrax: https://stackoverflow.com/a/70258061
bool RagTagUtil::deleteFile(const ragtag::path_t& path)
{
  // `pFrom` argument needs to be double null-terminated.
  std::wstring widestr = path.wstring() + L'\0';

  SHFILEOPSTRUCT fileOp;
  fileOp.hwnd = NULL;
  fileOp.wFunc = FO_DELETE;
  fileOp.pFrom = widestr.c_str();
  fileOp.pTo = NULL;
  fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
  int result = SHFileOperation(&fileOp);

  if (result != 0) {
    return false;
  }
  else {
    return true;
  }
}

std::wstring RagTagUtil::getRagTagAppVersionString()
{
  return std::vformat(L"v{}.{}.{}", std::make_wformat_args(RAGTAG_APP_VERSION_MAJOR,
    RAGTAG_APP_VERSION_MINOR, RAGTAG_APP_VERSION_PATCH));
}

void RagTagUtil::stamp(const std::string& label) {
  // One-time inits
  //static std::chrono::time_point first = std::chrono::system_clock::now();
  static std::chrono::time_point last = std::chrono::system_clock::now();
  static std::chrono::time_point now = std::chrono::system_clock::now();

  // Real function here
  now = std::chrono::system_clock::now();
  auto time_since_last = now - last;

  std::cout << std::chrono::duration_cast<std::chrono::microseconds>(time_since_last).count() << " us\n";
  std::cout << label << "\t";
  std::flush(std::cout);
  last = now;
}
