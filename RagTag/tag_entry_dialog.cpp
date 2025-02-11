#include "tag_entry_dialog.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/wx.h>

TagEntryDialog::TagEntryDialog(wxWindow* parent)
  : TagEntryDialog(parent, "new tag", ragtag::TagProperties{}) {
};

TagEntryDialog::TagEntryDialog(wxWindow* parent, ragtag::tag_t tag,
  const ragtag::TagProperties& tag_properties) : wxDialog(parent, wxID_ANY, "Create/Modify Tag",
    wxDefaultPosition, wxSize(320, 320)), parent_(parent), response_() {
  response_.tag = tag;
  response_.tag_properties = tag_properties;

  wxBoxSizer* sz_rows = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(sz_rows);

  wxFlexGridSizer* sz_text_entry_grid = new wxFlexGridSizer(2, 5, 5);  // cols, vgap, hgap
  sz_text_entry_grid->AddGrowableCol(1, 1);
  wxStaticText* st_tag_name = new wxStaticText(this, wxID_ANY, "Tag name:");
  sz_text_entry_grid->Add(st_tag_name, 0, wxALL, 5);
  std::string default_tag_text = response_.tag;
  tc_tag_name_ = new wxTextCtrl(this, wxID_ANY, default_tag_text);
  sz_text_entry_grid->Add(tc_tag_name_, 1, wxALL | wxEXPAND, 5);
  wxStaticText* st_hotkey = new wxStaticText(this, wxID_ANY, "Hotkey:");
  sz_text_entry_grid->Add(st_hotkey, 0, wxALL, 5);
  std::string default_hotkey_text;
  if (response_.tag_properties.hotkey.has_value()) {
    default_hotkey_text = *response_.tag_properties.hotkey;
  }
  else {
    default_hotkey_text = "";
  }
  tc_hotkey_ = new wxTextCtrl(this, wxID_ANY, default_hotkey_text, wxDefaultPosition, wxDefaultSize);
  tc_hotkey_->SetMaxLength(1);
  sz_text_entry_grid->Add(tc_hotkey_, 1, wxALL | wxEXPAND, 5);

  sz_rows->Add(sz_text_entry_grid, 0, wxEXPAND | wxALL, 5);

  wxBoxSizer* sz_default_setting_row = new wxBoxSizer(wxHORIZONTAL);

  // TODO: Tie this array closer to switch labels used in OnOk() and selection_index logic below.
  // NOTE: Make sure this and switch labels in OnOk() match!
  wxString choices[3] = { "No", "Yes", "Uncommitted" };
  rb_default_setting_ = new wxRadioBox(this, wxID_ANY, "Default state",
    wxDefaultPosition, wxDefaultSize, 3, choices, 1, wxRA_SPECIFY_COLS);

  int selection_index = 0;
  if (response_.tag_properties.default_setting == ragtag::TagSetting::YES) {
    selection_index = 1;
  }
  else if (response_.tag_properties.default_setting == ragtag::TagSetting::UNCOMMITTED) {
    selection_index = 2;
  }

  rb_default_setting_->SetSelection(selection_index);

  sz_default_setting_row->Add(rb_default_setting_, 1, wxALL, 5);
  sz_rows->Add(sz_default_setting_row, 0, wxEXPAND | wxALL, 5);

  cb_apply_to_all_files_ = new wxCheckBox(this, wxID_ANY, "Replace uncommitted tag status in existing files",
    wxDefaultPosition, wxDefaultSize);
  sz_rows->Add(cb_apply_to_all_files_, 0, wxALL, 10);

  wxBoxSizer* sz_button_row = new wxBoxSizer(wxHORIZONTAL);
  sz_button_row->AddStretchSpacer(1);
  wxButton* b_cancel = new wxButton(this, ID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize);
  sz_button_row->Add(b_cancel, 0);
  wxButton* b_ok = new wxButton(this, ID_OK, "OK", wxDefaultPosition, wxDefaultSize);
  b_ok->SetDefault();
  sz_button_row->Add(b_ok, 0);
  sz_rows->Add(sz_button_row, 0, wxEXPAND | wxALL, 10);

  Bind(wxEVT_BUTTON, &TagEntryDialog::OnOk, this, ID_OK);
  Bind(wxEVT_BUTTON, &TagEntryDialog::OnCancel, this, ID_CANCEL);
}

std::optional<TagEntryDialog::Response> TagEntryDialog::promptTagEntry() {
  this->ShowModal();
  return response_;
}

void TagEntryDialog::OnOk(wxCommandEvent& event) {
  Response response_pending{};
  response_pending.tag = tc_tag_name_->GetLineText(0);

  ragtag::TagProperties& tag_properties = response_pending.tag_properties;

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

  response_pending.apply_to_all_project_files = cb_apply_to_all_files_->IsChecked();

  const wxString hotkey_text = tc_hotkey_->GetLineText(0);
  if (hotkey_text.Length() > 0) {
    wxChar hotkey_char = hotkey_text.GetChar(0);
    if (hotkey_char >= L'a' && hotkey_char <= L'z') {
      hotkey_char -= 32;  // Lowercase appears 32 positions after uppercase.
    }
    if (hotkey_char >= L'A' && hotkey_char <= L'Z') {
      tag_properties.hotkey = hotkey_char;
    }
    else {
      // Invalid hotkey entered.
      // TODO: Report this somehow?
      tag_properties.hotkey = {};
    }
  }
  else {
    // Empty hotkey text.
    tag_properties.hotkey = {};
  }

  response_ = response_pending;

  // TODO: Validate the entry first?
  Close();
}

void TagEntryDialog::OnCancel(wxCommandEvent& event) {
  response_ = {};
  Close();
}
