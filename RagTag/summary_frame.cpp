#include "summary_frame.h"
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>

// The font used by wxWidgets does not display half-star characters as of writing.
// #define HALF_STAR_GLYPH_SUPPORTED

const wxString SummaryFrame::GLYPH_CHECKED = L"\U00002611";  // U+2611 is a checkmark in a box.
const wxString SummaryFrame::GLYPH_UNCOMMITTED = L"\U00002012";  // U+2012 is a figure dash.
const wxString SummaryFrame::GLYPH_UNCHECKED = L"\U00002610";  // U+2610 is an empty checkbox.
const wxString SummaryFrame::GLYPH_RATING_FULL_STAR = L"\U00002605";  // U+2605 is a full star.
#ifdef HALF_STAR_GLYPH_SUPPORTED
const wxString SummaryFrame::GLYPH_RATING_HALF_STAR = L"\U00002BE8";  // U+2BE8 is a half star.
#else
const wxString SummaryFrame::GLYPH_RATING_HALF_STAR = L"\U000000BD";  // U+00BD is a half fraction.
#endif
const int SummaryFrame::MAX_STARS = 5;

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
}

void SummaryFrame::setTagMap(const ragtag::TagMap& tag_map) {
  tag_map_ = tag_map;
}

void SummaryFrame::refresh()
{
  lc_summary_->ClearAll();
  lc_summary_->AppendColumn("Path", wxLIST_FORMAT_LEFT, 500);
  lc_summary_->AppendColumn("Rating", wxLIST_FORMAT_LEFT, 65);
  const auto all_tags = tag_map_.getAllTags();
  for (const auto& tag : all_tags) {
    lc_summary_->AppendColumn(tag.first, wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
  }
  const auto all_files = tag_map_.getAllFiles();
  for (int i = 0; i < all_files.size(); ++i) {
    lc_summary_->InsertItem(i, all_files[i].generic_wstring(), -1);
    // Show rating...
    auto rating = tag_map_.getRating(all_files[i]);
    lc_summary_->SetItem(i, 1, rating.has_value() ? getStarTextForRating(*rating) : wxString("--"));
    // Show state of tags...
    for (int j = 0; j < all_tags.size(); ++j) {  
      wxString tag_state_glyph = wxEmptyString;
      auto tag_setting = tag_map_.getTagSetting(all_files[i], all_tags[j].first);
      if (!tag_setting.has_value() || *tag_setting == ragtag::TagSetting::UNCOMMITTED) {
        tag_state_glyph = GLYPH_UNCOMMITTED;
      }
      else if (*tag_setting == ragtag::TagSetting::YES) {
        tag_state_glyph = GLYPH_CHECKED;
      }
      else if (*tag_setting == ragtag::TagSetting::NO) {
        tag_state_glyph = GLYPH_UNCHECKED;
      }

      // Set j+2 column because of Path and Rating columns taking indices 0 and 1.
      lc_summary_->SetItem(i, j + 2, tag_state_glyph, -1);
    }
  }
}

void SummaryFrame::OnResetSelections(wxCommandEvent& event)
{
}

void SummaryFrame::OnCopySelections(wxCommandEvent& event)
{
}

wxString SummaryFrame::getStarTextForRating(float rating)
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
