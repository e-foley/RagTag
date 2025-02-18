#include "summary_frame.h"
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>

SummaryFrame::SummaryFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, "Project Summary",
  wxDefaultPosition, wxSize(1280, 768))
{
  wxPanel* p_main = new wxPanel(this, wxID_ANY);
  wxBoxSizer* sz_main = new wxBoxSizer(wxVERTICAL);
  p_main->SetSizer(sz_main);

  wxPanel* p_summary_buttons = new wxPanel(p_main, wxID_ANY);
  wxBoxSizer* sz_summary_buttons = new wxBoxSizer(wxHORIZONTAL);
  p_summary_buttons->SetSizer(sz_summary_buttons);
  sz_summary_buttons->AddStretchSpacer(1);  // Stretch spacer at left to right-align buttons
  wxButton* b_reset_selections = new wxButton(p_summary_buttons, wxID_ANY, "Reset Selections");
  b_reset_selections->Bind(wxEVT_BUTTON, &SummaryFrame::OnResetSelections, this);
  sz_summary_buttons->Add(b_reset_selections, 0, wxALL, 5);
  wxButton* b_copy_selections = new wxButton(p_summary_buttons, wxID_ANY,
    "Copy Selected Files to Directory...");
  b_copy_selections->Bind(wxEVT_BUTTON, &SummaryFrame::OnCopySelections, this);
  sz_summary_buttons->Add(b_copy_selections, 0, wxALL, 5);
  sz_main->Add(p_summary_buttons, 0, wxEXPAND | wxALL, 0);

  lc_summary_ = new wxListCtrl(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL);
  sz_main->Add(lc_summary_, 1, wxEXPAND | wxALL, 5);
}

void SummaryFrame::setTagMap(const ragtag::TagMap& tag_map) {
  tag_map_ = tag_map;
}

void SummaryFrame::refresh()
{
  lc_summary_->ClearAll();
  lc_summary_->AppendColumn("Path", wxLIST_FORMAT_LEFT, 500);
  lc_summary_->AppendColumn("Rating", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
  const auto all_tags = tag_map_.getAllTags();
  for (const auto& tag : all_tags) {
    lc_summary_->AppendColumn(tag.first, wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
  }
  const auto all_files = tag_map_.getAllFiles();
  for (int i = 0; i < all_files.size(); ++i) {
    lc_summary_->InsertItem(i, all_files[i].generic_wstring());
  }
}

void SummaryFrame::OnResetSelections(wxCommandEvent& event)
{
}

void SummaryFrame::OnCopySelections(wxCommandEvent& event)
{
}
