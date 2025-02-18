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
  void OnResetSelections(wxCommandEvent& event);
  void OnCopySelections(wxCommandEvent& event);

  ragtag::TagMap tag_map_{};
  wxListCtrl* lc_summary_{};
};

#endif  // INCLUDE_SUMMARY_FRAME_H
