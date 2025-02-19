#ifndef INCLUDE_SUMMARY_FRAME_H
#define INCLUDE_SUMMARY_FRAME_H

#include "tag_map.h"
#include <wx/bmpbndl.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/vector.h>
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

  void OnResetSelections(wxCommandEvent& event);
  void OnCopySelections(wxCommandEvent& event);
  static wxString getStarTextForRating(float rating);

  ragtag::TagMap tag_map_{};
  wxListCtrl* lc_summary_{};
};

#endif  // INCLUDE_SUMMARY_FRAME_H
