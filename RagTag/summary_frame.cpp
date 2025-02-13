#include "summary_frame.h"
#include <wx/panel.h>
#include <wx/sizer.h>

SummaryFrame::SummaryFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, "Project Summary",
  wxDefaultPosition, wxDefaultSize)
{
  wxPanel* p_main = new wxPanel(this, wxID_ANY);
  wxBoxSizer* sz_main = new wxBoxSizer(wxVERTICAL);
  p_main->SetSizer(sz_main);

  lc_summary_ = new wxListCtrl(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL);
  sz_main->Add(lc_summary_, 1, wxEXPAND | wxALL, 5);
}

void SummaryFrame::setTagMap(const ragtag::TagMap& tag_map) {
  lc_summary_->ClearAll();
  lc_summary_->InsertColumn(0, "Path");
  const auto all_files = tag_map.getAllFiles();
  for (int i = 0; i < all_files.size(); ++i) {
    lc_summary_->InsertItem(i, all_files[i].generic_wstring());
  }
}
