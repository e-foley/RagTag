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
  parent_(parent)
{
  wxBoxSizer* sz_about = new wxBoxSizer(wxVERTICAL);
  SetSizer(sz_about);

  wxBoxSizer* sz_ragtag = new wxBoxSizer(wxVERTICAL);
  wxString about_string(L"RagTag " + RagTagUtil::getRagTagAppVersionString());
  wxStaticText* st_ragtag = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
    wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
  st_ragtag->SetLabelMarkup("<b>" + about_string + "</b>");
  sz_ragtag->Add(st_ragtag, 0, wxALL | wxEXPAND, 2);
  // (c) replacement technique from https://forums.wxwidgets.org/viewtopic.php?p=158583#p158583
  wxString copyright_string(L"Copyright (c) 2025 by Edward Foley");
#if wxUSE_UNICODE
  const wxString copyright_symbol = wxString::FromUTF8("\xc2\xa9");
  copyright_string.Replace("(c)", copyright_symbol);
#endif
  sz_ragtag->Add(new wxStaticText(this, wxID_ANY, copyright_string, wxDefaultPosition,
    wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->Add(new wxStaticText(this, wxID_ANY,
    "Released under the terms of the GNU General Public License v3.0", wxDefaultPosition,
    wxDefaultSize, wxALIGN_CENTER_HORIZONTAL), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->AddSpacer(10);
  sz_ragtag->Add(new wxHyperlinkCtrl(this, wxID_ANY, "GitHub repository",
    "https://github.com/e-foley/RagTag"), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->Add(new wxHyperlinkCtrl(this, wxID_ANY, "License (GNU General Public License v3.0)",
    "https://www.gnu.org/licenses/gpl-3.0.html"), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->Add(new wxHyperlinkCtrl(this, wxID_ANY, "Logo designed by Freepik",
    "https://www.freepik.com"), 0, wxALL | wxEXPAND, 2);
  sz_ragtag->AddSpacer(18);
  sz_ragtag->Add(new wxStaticText(this, wxID_ANY,
    "This application was made possible thanks to other open-source projects:"),
    0, wxALL | wxEXPAND, 2);
  sz_about->Add(sz_ragtag, 0, wxEXPAND | wxALL, 10);

  wxBoxSizer* sz_wxwidgets = new wxBoxSizer(wxVERTICAL);
  wxStaticText* st_wxwidgets = new wxStaticText(this, wxID_ANY, wxEmptyString);
  st_wxwidgets->SetLabelMarkup("<b>wxWidgets</b>");
  sz_wxwidgets->Add(st_wxwidgets, 0, wxALL | wxEXPAND, 2);
  sz_wxwidgets->Add(new wxHyperlinkCtrl(this, wxID_ANY, "Website",
    "https://wxwidgets.org/"), 0, wxALL | wxEXPAND, 2);
  sz_wxwidgets->Add(new wxHyperlinkCtrl(this, wxID_ANY, "GitHub repository",
    "https://github.com/wxWidgets/wxWidgets"), 0, wxALL | wxEXPAND, 2);
  sz_wxwidgets->Add(new wxHyperlinkCtrl(this, wxID_ANY, "License (wxWindows Library Licence v3.1)",
    "https://wxwidgets.org/about/licence/"), 0, wxALL | wxEXPAND, 2);
  sz_about->Add(sz_wxwidgets, 0, wxEXPAND | wxALL, 10);

  wxBoxSizer* sz_json = new wxBoxSizer(wxVERTICAL);
  wxStaticText* st_json = new wxStaticText(this, wxID_ANY, wxEmptyString);
  st_json->SetLabelMarkup("<b>JSON for Modern C++ by Niels Lohmann</b>");
  sz_json->Add(st_json, 0, wxALL | wxEXPAND, 2);
  sz_json->Add(new wxHyperlinkCtrl(this, wxID_ANY, "Website",
    "https://json.nlohmann.me/"), 0, wxALL | wxEXPAND, 2);
  sz_json->Add(new wxHyperlinkCtrl(this, wxID_ANY, "GitHub repository",
    "https://github.com/nlohmann/json/"), 0, wxALL | wxEXPAND, 2);
  sz_json->Add(new wxHyperlinkCtrl(this, wxID_ANY, "License (MIT License)",
    "https://json.nlohmann.me/home/license/"), 0, wxALL | wxEXPAND, 2);
  sz_about->Add(sz_json, 0, wxEXPAND | wxALL, 10);

  wxBoxSizer* sz_catch = new wxBoxSizer(wxVERTICAL);
  wxStaticText* st_catch = new wxStaticText(this, wxID_ANY, wxEmptyString);
  st_catch->SetLabelMarkup("<b>Catch2</b>");
  sz_catch->Add(st_catch, 0, wxALL | wxEXPAND, 2);
  sz_catch->Add(new wxHyperlinkCtrl(this, wxID_ANY, "GitHub repository",
    "https://github.com/catchorg/Catch2"), 0, wxALL | wxEXPAND, 2);
  sz_catch->Add(new wxHyperlinkCtrl(this, wxID_ANY, "License (Boost Software License v1.0)",
    "https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt"), 0, wxALL | wxEXPAND, 2);
  sz_about->Add(sz_catch, 0, wxEXPAND | wxALL, 10);

  wxBoxSizer* sz_bottom_button = new wxBoxSizer(wxHORIZONTAL);
  wxButton* b_ok = new wxButton(this, wxID_ANY, "OK");
  b_ok->Bind(wxEVT_BUTTON, &AboutDialog::OnOk, this);
  b_ok->SetDefault();
  sz_bottom_button->AddStretchSpacer(1);
  sz_bottom_button->Add(b_ok, 0, wxALL, 0);
  sz_about->AddStretchSpacer(1);
  sz_about->Add(sz_bottom_button, 0, wxEXPAND | wxALL, 10);

  SetSizerAndFit(sz_about);  // Auto-fits window to sizer requirements.
}

void AboutDialog::OnOk(wxCommandEvent& event) {
  Close();
}
