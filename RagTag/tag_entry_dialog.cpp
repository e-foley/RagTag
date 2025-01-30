#include "tag_entry_dialog.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/wx.h>

TagEntryDialog::TagEntryDialog(wxWindow* parent)
  : TagEntryDialog(parent, std::optional<tag_entry_t>{}) {};

TagEntryDialog::TagEntryDialog(wxWindow* parent, const tag_entry_t& entry_init)
  : TagEntryDialog(parent, std::optional<tag_entry_t>{entry_init}) {}

TagEntryDialog::TagEntryDialog(wxWindow* parent, const std::optional<tag_entry_t>& entry_init_opt)
  : wxDialog(parent, wxID_ANY, "Create/Modify Tag", wxDefaultPosition, wxSize(300, 250)),
  parent_(parent), entry_(entry_init_opt) {
  wxBoxSizer* sz_rows = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(sz_rows);

  wxBoxSizer* sz_tag_entry_row = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText* st_tag_name = new wxStaticText(this, wxID_ANY, "Tag name:");
  sz_tag_entry_row->Add(st_tag_name, 0, wxALL, 5);
  std::string default_tag_text = entry_.has_value() ? entry_->first : "new tag";
  tc_tag_name_ = new wxTextCtrl(this, wxID_ANY, default_tag_text);
  sz_tag_entry_row->Add(tc_tag_name_, 1, wxALL, 5);
  sz_rows->Add(sz_tag_entry_row, 0, wxEXPAND | wxALL, 5);

  wxBoxSizer* sz_default_setting_row = new wxBoxSizer(wxHORIZONTAL);
  //wxStaticText* st_default_setting_label = new wxStaticText(this, wxID_ANY, "Default setting:");
  //sz_default_setting_row->Add(st_default_setting_label, 0, wxALL, 5);

  // TODO: Tie this array closer to switch labels used in OnOk() and selection_index logic below.
  // NOTE: Make sure this and switch labels in OnOk() match!
  wxString choices[3] = { "No", "Yes", "Uncommitted" };
  rb_default_setting_ = new wxRadioBox(this, wxID_ANY, "Default state",
    wxDefaultPosition, wxDefaultSize, 3, choices, 1, wxRA_SPECIFY_COLS);

  int selection_index = 0;
  if (entry_.has_value() && entry_->second.default_setting == ragtag::TagSetting::YES) {
    selection_index = 1;
  }
  else if (entry_.has_value() && entry_->second.default_setting == ragtag::TagSetting::UNCOMMITTED) {
    selection_index = 2;
  }

  rb_default_setting_->SetSelection(selection_index);

  sz_default_setting_row->Add(rb_default_setting_, 1, wxALL, 5);
  sz_rows->Add(sz_default_setting_row, 0, wxEXPAND | wxALL, 5);

  wxBoxSizer* sz_button_row = new wxBoxSizer(wxHORIZONTAL);
  sz_button_row->AddStretchSpacer(1);
  wxButton* b_cancel = new wxButton(this, ID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize);
  sz_button_row->Add(b_cancel, 0);
  wxButton* b_ok = new wxButton(this, ID_OK, "OK", wxDefaultPosition, wxDefaultSize);
  b_ok->SetDefault();
  sz_button_row->Add(b_ok, 0);
  sz_rows->Add(sz_button_row, 0, wxEXPAND | wxALL, 5);

  Bind(wxEVT_BUTTON, &TagEntryDialog::OnOk, this, ID_OK);
  Bind(wxEVT_BUTTON, &TagEntryDialog::OnCancel, this, ID_CANCEL);
}

std::optional<TagEntryDialog::tag_entry_t> TagEntryDialog::promptTagEntry() {
  this->ShowModal();

  return entry_;
}

void TagEntryDialog::OnOk(wxCommandEvent& event) {
  tag_entry_t tag_entry_pending{};
  tag_entry_pending.first = tc_tag_name_->GetLineText(0);

  ragtag::TagProperties& tag_properties = tag_entry_pending.second;

  // TODO: Tie these switch labels closer to the ordering of `choices` in the constructor.
  // NOTE: Maintain same order/meaning as `choices` in constructor!
  switch (rb_default_setting_->GetSelection()) {
  case 0:
    tag_properties.default_setting = ragtag::TagSetting::NO;
    break;
  case 1:
    tag_properties.default_setting = ragtag::TagSetting::YES;
    break;
  default:
  case 2:
    tag_properties.default_setting = ragtag::TagSetting::UNCOMMITTED;
    break;
  }

  entry_ = tag_entry_pending;

  // TODO: Validate the entry first?
  Close();
}

void TagEntryDialog::OnCancel(wxCommandEvent& event) {
  entry_ = {};
  Close();
}
