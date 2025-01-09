#include "main_frame.h"
#include "tag_toggle_panel.h"

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
  p_main->SetSizer(sz_main);

  wxPanel* p_left = new wxPanel(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_left = new wxBoxSizer(wxVERTICAL);
  p_left->SetSizer(sz_left);

  TagTogglePanel* p_tag_toggle = new TagTogglePanel(p_left, "Demo");
  sz_left->Add(p_tag_toggle, 0, 0, 5);

  wxPanel* p_right = new wxPanel(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  sz_main->Add(p_left, 1, wxEXPAND | wxALL, 5);
  sz_main->Add(p_right, 1, wxEXPAND | wxALL, 5);


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