#include "main_frame.h"

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "RagTag v0.0.1") {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(ID_HELLO, "&Hello...\tCtrl-H",
    "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenu* menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);

  CreateStatusBar();
  SetStatusText("Welcome to wxWidgets!");


  // Temporary: Build a panel as a proof of concept...
  wxPanel* p_main = new wxPanel(this, wxID_ANY);
  wxBoxSizer* sz_main = new wxBoxSizer(wxHORIZONTAL);


  wxPanel* p_left = new wxPanel(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_left = new wxBoxSizer(wxVERTICAL);

  wxPanel* p_tag_display = new wxPanel(p_left, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_tag_display = new wxBoxSizer(wxHORIZONTAL);

  wxCheckBox* cb_tag_toggle = new wxCheckBox(p_tag_display, wxID_ANY, "Test");  // Second argument is checkbox ID.
  sz_tag_display->Add(cb_tag_toggle, 1, wxEXPAND, 5);
  wxButton* b_tag_edit = new wxButton(p_tag_display, wxID_ANY, "Edit");
  sz_tag_display->Add(b_tag_edit, 1, wxEXPAND, 5);

  p_tag_display->SetSizer(sz_tag_display);

  sz_left->Add(p_tag_display, 1, wxEXPAND, 5);
  p_left->SetSizer(sz_left);

  wxPanel* p_right = new wxPanel(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  sz_main->Add(p_left, 1, wxEXPAND | wxALL, 5);
  sz_main->Add(p_right, 1, wxEXPAND | wxALL, 5);

  p_main->SetSizer(sz_main);



  Bind(wxEVT_MENU, &MainFrame::OnHello, this, ID_HELLO);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
}

void MainFrame::OnExit(wxCommandEvent& event) {
  Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
  wxMessageBox("This is a (slightly modified) wxWidgets Hello World example",
    "About Hello World", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnHello(wxCommandEvent& event) {
  wxLogMessage("Hello world from wxWidgets!");
}