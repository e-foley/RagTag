#include "main_frame.h"

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Hello World") {
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
  wxPanel* panel = new wxPanel(this, wxID_ANY);
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

  wxPanel* panel_l = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxPanel* panel_r = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  hbox->Add(panel_l, 1, wxEXPAND | wxALL, 5);
  hbox->Add(panel_r, 1, wxEXPAND | wxALL, 5);

  panel->SetSizer(hbox);



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