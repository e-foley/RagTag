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

#ifndef INCLUDE_TAG_ENTRY_DIALOG_H
#define INCLUDE_TAG_ENTRY_DIALOG_H

#include "tag_map.h"
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/radiobox.h>
#include <wx/textctrl.h>
#include <wx/wx.h>

//! User interface dialog allowing the user to configure a tag in the context of a RagTag project.
class TagEntryDialog : public wxDialog {
public:
  //! Summary of the user's actions within the TagEntryDialog.
  struct Response {
    //! Tag described by the user through the dialog's text entry control.
    ragtag::tag_t tag;
    //! Tag properties communicated by the user through the dialog.
    ragtag::TagProperties tag_properties;
    //! Whether the user has indicated that the tag's default state should be applied to all files
    //! lacking an explicit setting for the tag.
    bool apply_to_all_project_files{ false };
  };

  //! Simple constructor.
  //! 
  //! @param parent The parent window.
  TagEntryDialog(wxWindow* parent);

  //! Detailed constructor.
  //! 
  //! @param parent The parent window.
  //! @param tag The tag to initialize the dialog with (e.g., by presenting it as default text
  //!     within the text entry control).
  //! @param tag_properties Properties of this tag to initialize the dialog with (e.g., by
  //!     configuring radio buttons accordingly).
  TagEntryDialog(wxWindow* parent, ragtag::tag_t tag, const ragtag::TagProperties& tag_properties);

  //! Displays the TagEntryDialog and gathers the user's response to the dialog.
  //! 
  //! @returns The tag configuration and related settings communicated by the user through their
  //!     actions in this dialog, or a default optional if the user exits the dialog early.
  std::optional<Response> promptTagEntry();

private:
  //! Event handler for when the user clicks "OK".
  //! 
  //! Composes a Response based on the state of the user interface controls.
  //! 
  //! @param event Information about the event that triggered this function.
  void OnOk(wxCommandEvent& event);

  //! Event handler for when the user clicks "Cancel".
  //! 
  //! Closes the dialog.
  //! 
  //! @param event Information about the event that triggered this function.
  void OnCancel(wxCommandEvent& event);

  //! Event handler for key presses.
  //! 
  //! Exits the dialog upon Ctrl+W or Esc.
  //! 
  //! @param event Information about the event that triggered this function, including keyboard
  //!     state.
  void OnKeyDown(wxKeyEvent& event);

  //! Summary of user's actions within the dialog to be communicated via promptTagEntry().
  Response response_{};

  //! Whether the user confirmed the dialog by pressing "OK".
  bool response_confirmed_{ false };

  //! Parent window.
  wxWindow* parent_{ nullptr };

  //! Text control allowing the user to enter a tag name.
  wxTextCtrl* tc_tag_name_{ nullptr };

  //! Text control allowing the user to specify a hotkey.
  wxTextCtrl* tc_hotkey_{ nullptr };

  //! Radio button group allowing the user to choose a default state for the tag.
  wxRadioBox* rb_default_setting_{ nullptr };

  //! Checkbox allowing the user to indicate that the tag's default state should be applied to all
  //! files lacking an explicit setting for this tag.
  wxCheckBox* cb_apply_to_all_files_{ nullptr };
};

#endif  // INCLUDE_TAG_ENTRY_DIALOG_H
