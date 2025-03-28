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

#ifndef INCLUDE_ABOUT_DIALOG_H
#define INCLUDE_ABOUT_DIALOG_H

#include <wx/dialog.h>
#include <wx/textctrl.h>

//! A simple dialog that acknowledges people and projects that enabled RagTag.
class AboutDialog : public wxDialog {
public:
  //! Constructor.
  //! 
  //! @param parent The parent window.
  AboutDialog(wxWindow* parent);

  ////! Displays the AboutDialog.
  //void display();

private:
  //! Event handler for when the user clicks "OK".
  //! 
  //! Composes a Response based on the state of the user interface controls.
  //! 
  //! @param event Information about the event that triggered this function.
  void OnOk(wxCommandEvent& event);

  //! Parent window.
  wxWindow* parent_{ nullptr };
};

#endif  // INCLUDE_ABOUT_DIALOG_H
