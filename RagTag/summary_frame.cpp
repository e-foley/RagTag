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

  lc_summary_ = new wxListCtrl(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL);
  sz_main->Add(lc_summary_, 1, wxEXPAND | wxALL, 5);

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

  // Approach heavily follows wxWidgets listctrl sample project.
  normal_images_.push_back(wxIcon("icon1", wxBITMAP_TYPE_ICO_RESOURCE));
  normal_images_.push_back(wxIcon("icon2", wxBITMAP_TYPE_ICO_RESOURCE));
  normal_images_.push_back(wxIcon("icon3", wxBITMAP_TYPE_ICO_RESOURCE));
  normal_images_.push_back(wxIcon("icon4", wxBITMAP_TYPE_ICO_RESOURCE));
  normal_images_.push_back(wxIcon("icon5", wxBITMAP_TYPE_ICO_RESOURCE));
  normal_images_.push_back(wxIcon("icon6", wxBITMAP_TYPE_ICO_RESOURCE));
  normal_images_.push_back(wxIcon("icon7", wxBITMAP_TYPE_ICO_RESOURCE));
  normal_images_.push_back(wxIcon("icon8", wxBITMAP_TYPE_ICO_RESOURCE));
  normal_images_.push_back(wxIcon("icon9", wxBITMAP_TYPE_ICO_RESOURCE));
  small_images_.push_back(wxIcon("iconsmall", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16));
  lc_summary_->SetNormalImages(normal_images_);
  lc_summary_->SetSmallImages(small_images_);
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
    // Show rating...
    auto rating = tag_map_.getRating(all_files[i]);
    // TODO: Show star icons rather than textual representation of number.
    lc_summary_->SetItem(i, 1, rating.has_value() ? std::to_string(*rating) : "--");
    // Show state of tags...
    for (int j = 0; j < all_tags.size(); ++j) {
      int icon_index = -1;
      auto tag_setting = tag_map_.getTagSetting(all_files[i], all_tags[j].first);
      // TODO: Don't use magic numbers here.
      if (!tag_setting.has_value()) {
        icon_index = -1;  // Uncommitted
      }
      else if (*tag_setting == ragtag::TagSetting::YES) {
        icon_index = 0;
      }
      
      // Set j+2 column because of Path and Rating columns taking indices 0 and 1.
      lc_summary_->SetItem(i, j + 2, "test", icon_index);
    }
  }
}

void SummaryFrame::OnResetSelections(wxCommandEvent& event)
{
}

void SummaryFrame::OnCopySelections(wxCommandEvent& event)
{
}
