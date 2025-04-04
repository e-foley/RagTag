// Copyright (C) 2025 by Edward Foley
//
// This file is part of RagTag.
//
// RagTag is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// RagTag is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with RagTag. If not, see
// <https://www.gnu.org/licenses/>.

#include "tag_toggle_panel.h"
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>

wxDEFINE_EVENT(TAG_TOGGLE_BUTTON_EVENT, TagToggleEvent);

TagTogglePanel::TagTogglePanel(wxWindow* parent, ragtag::tag_t tag, wxString label,
  std::optional<ragtag::rtchar_t> hotkey) 
  : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxWANTS_CHARS),
  parent_(parent), tag_(tag), hotkey_(hotkey) {
  wxBoxSizer* sz_tag_toggle = new wxBoxSizer(wxHORIZONTAL);
  this->SetSizer(sz_tag_toggle);

  st_hotkey_ = new wxStaticText(this, wxID_ANY,
    hotkey_.has_value() ? wxString(*hotkey_) : wxString(wxEmptyString), wxDefaultPosition,
    wxSize(12, -1), wxALIGN_CENTRE_HORIZONTAL | wxST_NO_AUTORESIZE);
  sz_tag_toggle->Add(st_hotkey_, 0, wxALIGN_CENTER | wxALL, 5);

  cb_tag_toggle_ = new wxCheckBox(this, wxID_ANY, label,
    wxDefaultPosition, wxDefaultSize, wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);
  cb_tag_toggle_->Bind(wxEVT_CHECKBOX, &TagTogglePanel::OnCheckboxChange, this);
  sz_tag_toggle->Add(cb_tag_toggle_, 1, wxALIGN_CENTER, 5);
  wxButton* b_tag_edit = new wxButton(this, wxID_ANY, "Edit", wxDefaultPosition,
    wxDefaultSize, wxBU_EXACTFIT);
  b_tag_edit->Bind(wxEVT_BUTTON, &TagTogglePanel::OnClickEdit, this);
  sz_tag_toggle->Add(b_tag_edit, 0, wxALIGN_CENTER, 5);
  wxButton* b_tag_delete = new wxButton(this, wxID_ANY, " X ", wxDefaultPosition,
    wxDefaultSize, wxBU_EXACTFIT);
  b_tag_delete->Bind(wxEVT_BUTTON, &TagTogglePanel::OnClickDelete, this);
  sz_tag_toggle->Add(b_tag_delete, 0, wxALIGN_CENTER, 5);

  disableCheckboxAndHotkey();
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

void TagTogglePanel::disableCheckboxAndHotkey()
{
  hotkey_enabled_ = false;
  st_hotkey_->Disable();
  cb_tag_toggle_->Disable();
}

void TagTogglePanel::enableCheckboxAndHotkey()
{
  hotkey_enabled_ = true;
  st_hotkey_->Enable();
  cb_tag_toggle_->Enable();
}

bool TagTogglePanel::processKeyEvent(wxKeyEvent& event)
{
  if (hotkey_enabled_ && hotkey_.has_value() && event.GetUnicodeKey() == *hotkey_) {
    TagToggleEvent sending(tag_, TagToggleEvent::DesiredAction::UPDATE_TAG_STATE);
    const wxCheckBoxState current_checked_state = cb_tag_toggle_->Get3StateValue();
    switch (event.GetModifiers()) {
    case wxMOD_NONE:
      sending.setDesiredState(current_checked_state == wxCHK_CHECKED ?
        ragtag::TagSetting::UNCOMMITTED : ragtag::TagSetting::YES);
      wxPostEvent(parent_, sending);
      return true;
    case wxMOD_SHIFT:
      sending.setDesiredState(current_checked_state == wxCHK_UNCHECKED ?
        ragtag::TagSetting::UNCOMMITTED : ragtag::TagSetting::NO);
      wxPostEvent(parent_, sending);
      return true;
    default:
      break;
    }
  }
  return false;
}

void TagTogglePanel::OnClickEdit(wxCommandEvent& event) {
  TagToggleEvent sending(tag_, TagToggleEvent::DesiredAction::EDIT_TAG);
  wxPostEvent(parent_, sending);
}

void TagTogglePanel::OnClickDelete(wxCommandEvent& event) {
  TagToggleEvent sending(tag_, TagToggleEvent::DesiredAction::DELETE_TAG);
  wxPostEvent(parent_, sending);
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

  wxPostEvent(parent_, sending);
}
