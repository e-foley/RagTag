#ifndef INCLUDE_SUMMARY_FRAME_H
#define INCLUDE_SUMMARY_FRAME_H

#include "tag_map.h"
#include <wx/frame.h>
#include <wx/window.h>

class SummaryFrame : public wxFrame {
public:
  SummaryFrame(wxWindow* parent, const ragtag::TagMap& tag_map);

private:
  ragtag::TagMap tag_map_{};
};

#endif  // INCLUDE_SUMMARY_FRAME_H
