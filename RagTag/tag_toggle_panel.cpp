#include "tag_toggle_panel.h"
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

wxDEFINE_EVENT(TAG_TOGGLE_BUTTON_EVENT, TagToggleEvent);

TagTogglePanel::TagTogglePanel(wxWindow* parent, ragtag::tag_t tag, wxString label,
  std::optional<ragtag::rtchar_t> hotkey) 
  : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED | wxWANTS_CHARS), tag_(tag),
  hotkey_(hotkey) {
  wxBoxSizer* sz_tag_toggle = new wxBoxSizer(wxHORIZONTAL);
  this->SetSizer(sz_tag_toggle);

  wxStaticText* st_hotkey = new wxStaticText(this, wxID_ANY,
    hotkey_.has_value() ? wxString(*hotkey_) : wxString(wxEmptyString), wxDefaultPosition,
    wxSize(12, -1), wxALIGN_CENTRE_HORIZONTAL | wxST_NO_AUTORESIZE);
  sz_tag_toggle->Add(st_hotkey, 0, wxALIGN_CENTER | wxALL, 5);

  cb_tag_toggle_ = new wxCheckBox(this, wxID_ANY, label,
    wxDefaultPosition, wxDefaultSize, wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);
  cb_tag_toggle_->Bind(wxEVT_CHECKBOX, &TagTogglePanel::OnCheckboxChange, this);
  sz_tag_toggle->Add(cb_tag_toggle_, 1, wxALIGN_CENTER, 5);
  wxButton* b_tag_edit = new wxButton(this, wxID_ANY, "Edit", wxDefaultPosition,
    wxDefaultSize, wxBU_EXACTFIT);
  b_tag_edit->Bind(wxEVT_BUTTON, &TagTogglePanel::OnClickEdit, this);
  sz_tag_toggle->Add(b_tag_edit, 0, wxALIGN_CENTER, 5);
  wxButton* b_tag_delete = new wxButton(this, wxID_ANY, "X", wxDefaultPosition,
    wxDefaultSize, wxBU_EXACTFIT);
  b_tag_delete->Bind(wxEVT_BUTTON, &TagTogglePanel::OnClickDelete, this);
  sz_tag_toggle->Add(b_tag_delete, 0, wxALIGN_CENTER, 5);

  parent->Bind(wxEVT_CHAR_HOOK, &TagTogglePanel::OnKeyDown, this);
}

TagTogglePanel::~TagTogglePanel() {}

void TagTogglePanel::setCheckBoxState(ragtag::TagSetting state) {
  switch (state) {
  case ragtag::TagSetting::NO:
    cb_tag_toggle_->Set3StateValue(wxCHK_UNCHECKED);
    break;
  case ragtag::TagSetting::YES:
    cb_tag_toggle_->Set3StateValue(wxCHK_CHECKED);
    break;
  default:
  case ragtag::TagSetting::UNCOMMITTED:
    cb_tag_toggle_->Set3StateValue(wxCHK_UNDETERMINED);
    break;
  }
}

ragtag::TagSetting TagTogglePanel::getCheckBoxState() const {
  auto checkbox_value = cb_tag_toggle_->Get3StateValue();
  switch (checkbox_value) {
  case wxCHK_UNCHECKED:
    return ragtag::TagSetting::NO;
  case wxCHK_CHECKED:
    return ragtag::TagSetting::YES;
  default:
  case wxCHK_UNDETERMINED:
    return ragtag::TagSetting::UNCOMMITTED;
  }
}

void TagTogglePanel::OnClickEdit(wxCommandEvent& event) {
  TagToggleEvent sending(tag_, TagToggleEvent::DesiredAction::EDIT_TAG);
  wxPostEvent(GetParent(), sending);
}

void TagTogglePanel::OnClickDelete(wxCommandEvent& event) {
  TagToggleEvent sending(tag_, TagToggleEvent::DesiredAction::DELETE_TAG);
  wxPostEvent(GetParent(), sending);
}

void TagTogglePanel::OnCheckboxChange(wxCommandEvent& event) {
  TagToggleEvent sending(tag_, TagToggleEvent::DesiredAction::UPDATE_TAG_STATE);
  switch (cb_tag_toggle_->Get3StateValue()) {
  case wxCHK_UNCHECKED:
    sending.setDesiredState(ragtag::TagSetting::NO);
    break;
  case wxCHK_CHECKED:
    sending.setDesiredState(ragtag::TagSetting::YES);
    break;
  default:
  case wxCHK_UNDETERMINED:
    sending.setDesiredState(ragtag::TagSetting::UNCOMMITTED);
    break;
  }

  wxPostEvent(GetParent(), sending);
}

void TagTogglePanel::OnKeyDown(wxKeyEvent& event)
{
  if (hotkey_.has_value() && event.GetUnicodeKey() == *hotkey_) {
    TagToggleEvent sending(tag_, TagToggleEvent::DesiredAction::UPDATE_TAG_STATE);
    const wxCheckBoxState current_checked_state = cb_tag_toggle_->Get3StateValue();
    switch (event.GetModifiers()) {
    case wxMOD_NONE:
      sending.setDesiredState(current_checked_state == wxCHK_CHECKED ?
        ragtag::TagSetting::UNCOMMITTED : ragtag::TagSetting::YES);
      wxPostEvent(GetParent(), sending);
      break;
    case wxMOD_SHIFT:
      sending.setDesiredState(current_checked_state == wxCHK_UNCHECKED ?
        ragtag::TagSetting::UNCOMMITTED : ragtag::TagSetting::NO);
      wxPostEvent(GetParent(), sending);
      break;
    default:
      break;
    }
  }
  event.Skip();
}
