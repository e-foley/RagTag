#ifndef INCLUDE_SUMMARY_FRAME_H
#define INCLUDE_SUMMARY_FRAME_H

#include "tag_map.h"
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/window.h>

class SummaryFrame : public wxFrame {
public:
  SummaryFrame(wxWindow* parent);
  void setTagMap(const ragtag::TagMap& tag_map);
  void refresh();

private:
  static const wxString GLYPH_CHECKED;
  static const wxString GLYPH_UNCOMMITTED;
  static const wxString GLYPH_UNCHECKED;
  static const wxString GLYPH_RATING_FULL_STAR;
  static const wxString GLYPH_RATING_HALF_STAR;
  static const int MAX_STARS;
  static const int PATH_COLUMN_INDEX;
  static const int RATING_COLUMN_INDEX;
  static const int FIRST_TAG_COLUMN_INDEX;

  struct SortHelper {
    ragtag::TagMap* p_tag_map{ nullptr };
    ragtag::tag_t tag{};
    bool sort_ascending{ false };
  };

  void OnResetSelections(wxCommandEvent& event);
  void OnCopySelections(wxCommandEvent& event);
  void OnClickHeading(wxListEvent& event);

  static wxString getStarTextForRating(float rating);

  static int wxCALLBACK tagSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data);
  static int wxCALLBACK ratingSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data);

  ragtag::TagMap tag_map_{};
  std::vector<ragtag::path_t> file_paths_{};  // Indices to match those of wxListCtrl sequencing
  wxListCtrl* lc_summary_{};
};

#endif  // INCLUDE_SUMMARY_FRAME_H
