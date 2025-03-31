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

#include "about_dialog.h"
#include "rag_tag_util.h"
#include <wx/button.h>
#include <wx/hyperlink.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

AboutDialog::AboutDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "About RagTag"),
    parent_(parent) {
  SetSize(500, 700);
  wxBoxSizer* sz_about = new wxBoxSizer(wxVERTICAL);
  SetSizer(sz_about);
  //sz_about->AddSpacer(20);

  wxBoxSizer* sz_ragtag = new wxBoxSizer(wxVERTICAL);
  // (c) replacement technique from https://forums.wxwidgets.org/viewtopic.php?p=158583#p158583
  wxString about_string(L"RagTag " + RagTagUtil::getRagTagAppVersionString());
  wxStaticText* st_ragtag = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
    wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  st_ragtag->SetLabelMarkup("<b>" + about_string + "</b>");
  sz_ragtag->Add(st_ragtag, 0, wxALL | wxEXPAND, 2);
  wxString copyright_string(L"Copyright (c) 2025 by Edward Foley");
#if wxUSE_UNICODE
  const wxString copyright_symbol = wxString::FromUTF8("\xc2\xa9");
  copyright_string.Replace("(c)", copyright_symbol);
#endif
  sz_ragtag->Add(new wxStaticText(this, wxID_ANY, copyright_string, wxDefaultPosition,
    wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->Add(new wxStaticText(this, wxID_ANY,
    "Released under the terms of the GNU General Public License v3.0",  wxDefaultPosition,
    wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->AddSpacer(10);
  sz_ragtag->Add(new wxHyperlinkCtrl(this, wxID_ANY, "RagTag GitHub repository",
    "https://github.com/e-foley/RagTag"), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->Add(new wxHyperlinkCtrl(this, wxID_ANY, "GNU General Public License v3.0",
    "https://www.gnu.org/licenses/gpl-3.0.html"), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->Add(new wxHyperlinkCtrl(this, wxID_ANY, "RagTag logo designed by Freepik",
    "https://www.freepik.com"), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->Add(new wxStaticText(this, wxID_ANY,
    "This application was made possible thanks to other open-source projects:"),
    0, wxALL | wxEXPAND, 2);
  sz_about->Add(sz_ragtag, 0, wxEXPAND | wxALL, 10);
  //sz_about->AddSpacer(20);

  wxBoxSizer* sz_wxwidgets = new wxBoxSizer(wxVERTICAL);
  wxStaticText* st_wxwidgets = new wxStaticText(this, wxID_ANY, wxEmptyString);
  st_wxwidgets->SetLabelMarkup("<b>wxWidgets</b>");
  sz_wxwidgets->Add(st_wxwidgets, 0, wxALL | wxEXPAND, 2);
  sz_wxwidgets->Add(new wxHyperlinkCtrl(this, wxID_ANY, "wxWidgets website",
    "https://wxwidgets.org/"), 0, wxALL | wxEXPAND, 2);
  sz_wxwidgets->Add(new wxHyperlinkCtrl(this, wxID_ANY, "wxWidgets GitHub repository",
    "https://github.com/wxWidgets/wxWidgets"), 0, wxALL | wxEXPAND, 2);
  sz_wxwidgets->Add(new wxHyperlinkCtrl(this, wxID_ANY, "wxWindows Library Licence v3.1",
    "https://wxwidgets.org/about/licence/"), 0, wxALL | wxEXPAND, 2);
  sz_about->Add(sz_wxwidgets, 0, wxEXPAND | wxALL, 10);
  //sz_about->AddSpacer(20);

  wxBoxSizer* sz_json = new wxBoxSizer(wxVERTICAL);
  wxStaticText* st_json = new wxStaticText(this, wxID_ANY, wxEmptyString);
  st_json->SetLabelMarkup("<b>JSON for Modern C++ by Niels Lohmann</b>");
  sz_json->Add(st_json, 0, wxALL | wxEXPAND, 2);
  sz_json->Add(new wxHyperlinkCtrl(this, wxID_ANY, "JSON for Modern C++ website",
    "https://json.nlohmann.me/"), 0, wxALL | wxEXPAND, 2);
  sz_json->Add(new wxHyperlinkCtrl(this, wxID_ANY, "JSON for Modern C++ GitHub repository",
    "https://github.com/nlohmann/json/"), 0, wxALL | wxEXPAND, 2);
  sz_json->Add(new wxHyperlinkCtrl(this, wxID_ANY, "MIT License",
    "https://github.com/nlohmann/json/blob/develop/LICENSE.MIT"), 0, wxALL | wxEXPAND, 2);
  sz_about->Add(sz_json, 0, wxEXPAND | wxALL, 10);
  //sz_about->AddSpacer(20);

  wxBoxSizer* sz_catch = new wxBoxSizer(wxVERTICAL);
  wxStaticText* st_catch = new wxStaticText(this, wxID_ANY, wxEmptyString);
  st_catch->SetLabelMarkup("<b>Catch2 by </b>");
  sz_catch->Add(st_catch, 0, wxALL | wxEXPAND, 2);
  sz_catch->Add(new wxHyperlinkCtrl(this, wxID_ANY, "Catch2 GitHub repository",
    "https://github.com/catchorg/Catch2"), 0, wxALL | wxEXPAND, 2);
  sz_catch->Add(new wxHyperlinkCtrl(this, wxID_ANY, "Boost Software License v1.0",
    "https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt"), 0, wxALL | wxEXPAND, 2);
  sz_about->Add(sz_catch, 0, wxEXPAND | wxALL, 10);

  //wxBoxSizer* sz_freepik = new wxBoxSizer(wxVERTICAL);
  //wxStaticText* st_freepik = new wxStaticText(this, wxID_ANY, wxEmptyString);
  //st_freepik->SetLabelMarkup("<b>Freepik</b>");
  //sz_freepik->Add(st_freepik, 0, wxALL | wxEXPAND, 2);

  //sz_about->Add(sz_freepik, 0, wxEXPAND | wxALL, 10);

  //SetIcons(wxIconBundle(wxICON(IDI_ICON1)));
  //response_.tag = tag;
  //response_.tag_properties = tag_properties;

  //wxBoxSizer* sz_rows = new wxBoxSizer(wxVERTICAL);

  //wxFlexGridSizer* sz_text_entry_grid = new wxFlexGridSizer(2, 5, 5);  // cols, vgap, hgap
  //sz_text_entry_grid->AddGrowableCol(1, 1);
  //wxStaticText* st_tag_name = new wxStaticText(this, wxID_ANY, "Tag name:");
  //sz_text_entry_grid->Add(st_tag_name, 0, wxALL, 5);
  //wxString default_tag_text = response_.tag;
  //tc_tag_name_ = new wxTextCtrl(this, wxID_ANY, default_tag_text);
  //sz_text_entry_grid->Add(tc_tag_name_, 1, wxALL | wxEXPAND, 5);
  //wxStaticText* st_hotkey = new wxStaticText(this, wxID_ANY, "Hotkey:");
  //sz_text_entry_grid->Add(st_hotkey, 0, wxALL, 5);
  //wxString default_hotkey_text;
  //if (response_.tag_properties.hotkey.has_value()) {
  //  default_hotkey_text = *response_.tag_properties.hotkey;
  //}
  //else {
  //  default_hotkey_text = "";
  //}
  //tc_hotkey_ = new wxTextCtrl(this, wxID_ANY, default_hotkey_text, wxDefaultPosition, wxDefaultSize);
  //tc_hotkey_->SetMaxLength(1);
  //sz_text_entry_grid->Add(tc_hotkey_, 1, wxALL | wxEXPAND, 5);

  //sz_rows->Add(sz_text_entry_grid, 0, wxEXPAND | wxALL, 5);

  //wxBoxSizer* sz_default_setting_row = new wxBoxSizer(wxHORIZONTAL);

  //// TODO: Tie this array closer to switch labels used in OnOk() and selection_index logic below.
  //// NOTE: Make sure this and switch labels in OnOk() match!
  //wxString choices[3] = { "No", "Yes", "Uncommitted" };
  //rb_default_setting_ = new wxRadioBox(this, wxID_ANY, "Default state",
  //  wxDefaultPosition, wxDefaultSize, 3, choices, 1, wxRA_SPECIFY_COLS);

  //int selection_index = 0;
  //if (response_.tag_properties.default_setting == ragtag::TagSetting::YES) {
  //  selection_index = 1;
  //}
  //else if (response_.tag_properties.default_setting == ragtag::TagSetting::UNCOMMITTED) {
  //  selection_index = 2;
  //}

  //rb_default_setting_->SetSelection(selection_index);

  //sz_default_setting_row->Add(rb_default_setting_, 1, wxALL, 5);
  //sz_rows->Add(sz_default_setting_row, 0, wxEXPAND | wxALL, 5);

  //cb_apply_to_all_files_ = new wxCheckBox(this, wxID_ANY, "Replace uncommitted tag status in existing files",
  //  wxDefaultPosition, wxDefaultSize);
  //sz_rows->Add(cb_apply_to_all_files_, 0, wxALL, 10);

  //wxBoxSizer* sz_button_row = new wxBoxSizer(wxHORIZONTAL);
  //sz_button_row->AddStretchSpacer(1);
  //wxButton* b_ok = new wxButton(this, wxID_ANY, "OK", wxDefaultPosition, wxDefaultSize);
  //b_ok->Bind(wxEVT_BUTTON, &AboutDialog::OnOk, this);
  //b_ok->SetDefault();
  //sz_button_row->Add(b_ok, 0);
  //wxButton* b_cancel = new wxButton(this, wxID_ANY, "Cancel", wxDefaultPosition, wxDefaultSize);
  //b_cancel->Bind(wxEVT_BUTTON, &AboutDialog::OnCancel, this);
  //sz_button_row->Add(b_cancel, 0);
  //sz_rows->Add(sz_button_row, 0, wxEXPAND | wxALL, 10);

  //this->SetSizerAndFit(sz_rows);  // Auto-fits window to sizer requirements.

  //Bind(wxEVT_CHAR_HOOK, &AboutDialog::OnKeyDown, this);
}

//void AboutDialog::display() {}

void AboutDialog::OnOk(wxCommandEvent& event) {
  Close();
}
