#include "summary_frame.h"

SummaryFrame::SummaryFrame(wxWindow* parent, const ragtag::TagMap& tag_map)
  : wxFrame(parent, wxID_ANY, "Project Summary", wxDefaultPosition, wxDefaultSize),
  tag_map_(tag_map)
{
}
