#include "main_frame.h"
#include "tag_toggle_panel.h"
#include <wx/mediactrl.h>
#include <wx/scrolwin.h>
#include <wx/stdpaths.h>

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "RagTag v0.0.1", wxDefaultPosition, wxSize(800, 600)) {
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

  wxScrolledWindow* p_tag_toggles = new wxScrolledWindow(p_left, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_tag_toggles = new wxBoxSizer(wxVERTICAL);
  p_tag_toggles->SetSizer(sz_tag_toggles);

  for (int i = 0; i < 10; ++i) {
    TagTogglePanel* p_tag_toggle = new TagTogglePanel(p_tag_toggles, "Demo");
    sz_tag_toggles->Add(p_tag_toggle, 0, wxEXPAND | wxALL, 0);
  }

  p_tag_toggles->FitInside();
  p_tag_toggles->SetScrollRate(5, 5);

  sz_left->Add(p_tag_toggles, 1, wxEXPAND | wxALL, 5);

  wxPanel* p_tag_toggles_button_bar = new wxPanel(p_left, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_tag_toggles_button_bar = new wxBoxSizer(wxHORIZONTAL);
  p_tag_toggles_button_bar->SetSizer(sz_tag_toggles_button_bar);
  wxButton* b_define_new_tag = new wxButton(p_tag_toggles_button_bar, wxID_ANY, "Define New Tag...");
  sz_tag_toggles_button_bar->AddStretchSpacer(1);  // Empty space at left to force right-alignment.
  sz_tag_toggles_button_bar->Add(b_define_new_tag, 0, wxALL, 5);
  sz_left->Add(p_tag_toggles_button_bar, 0, wxEXPAND | wxALL, 5);

  wxPanel* p_right = new wxPanel(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_right = new wxBoxSizer(wxVERTICAL);
  p_right->SetSizer(sz_right);

  // Note: Even though wxWidgets says we should "almost certainly leave [the backend selection] up
  //       to wxMediaCtrl," a documented bug involving wxWidgets' interaction with the DirectShow
  //       API suppresses the media-loaded event. This also affects the mediaplayer demo provided by
  //       wxWidgets itself. To circumvent this, we explicitly choose the Windows Media Player
  //       backend.
  // See: https://docs.wxwidgets.org/stable/classwx_media_ctrl.html
  // See: https://forums.wxwidgets.org/viewtopic.php?t=47476
  // See: https://github.com/wxWidgets/wxWidgets/issues/18976   
  mc_media_display_ = new wxMediaCtrl(p_right, ID_MEDIA_CTRL, wxEmptyString, wxDefaultPosition,
      wxDefaultSize, wxMC_NO_AUTORESIZE, wxMEDIABACKEND_WMP10);
  sz_right->Add(mc_media_display_, 1, wxEXPAND | wxALL, 0);

  // TODO: Replace this temporary location with real paths once application is stable.
  const wxString debug_media_dir = wxStandardPaths::Get().GetDocumentsDir() + "/ragtag-debug/";
  mc_media_display_->Load(debug_media_dir + "videomp4.mp4");
  //mc_media_display_->Load(debug_media_dir + "imagejpg.jpg");
  //mc_media_display_->Load(debug_media_dir + "imagepng.png");

  sz_main->Add(p_left, 1, wxEXPAND | wxALL, 5);
  sz_main->Add(p_right, 1, wxEXPAND | wxALL, 5);

  Bind(wxEVT_MENU, &MainFrame::OnHello, this, ID_HELLO);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_MEDIA_LOADED, &MainFrame::OnMediaLoaded, this, ID_MEDIA_CTRL);
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

void MainFrame::OnMediaLoaded(wxMediaEvent& event) {
  mc_media_display_->Play();
}
