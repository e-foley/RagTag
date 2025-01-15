#include "main_frame.h"
#include "tag_toggle_panel.h"
#include <wx/filedlg.h>
#include <wx/mediactrl.h>
#include <wx/scrolwin.h>
#include <wx/stdpaths.h>

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "RagTag v0.0.1", wxDefaultPosition, wxSize(800, 600)) {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(ID_NEW, "&New Project...\tCtrl-N");
  menuFile->Append(ID_OPEN, "&Open Project...\tCtrl-O");
  menuFile->Append(ID_SAVE, "&Save Project...\tCtrl-S");
  menuFile->Append(ID_SAVE_AS, "Save &As...\tCtrl-Shift-S");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenu* menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);

  CreateStatusBar();

  // Temporary: Build a panel as a proof of concept...
  wxPanel* p_main = new wxPanel(this, wxID_ANY);
  wxBoxSizer* sz_main = new wxBoxSizer(wxHORIZONTAL);
  p_main->SetSizer(sz_main);

  wxPanel* p_left = new wxPanel(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_left = new wxBoxSizer(wxVERTICAL);
  p_left->SetSizer(sz_left);

  p_tag_toggles_ = new wxScrolledWindow(p_left, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  sz_tag_toggles_ = new wxBoxSizer(wxVERTICAL);
  p_tag_toggles_->SetSizer(sz_tag_toggles_);

  // DEBUG
  for (int i = 0; i < 25; ++i) {
    tag_map_.registerTag("Tag " + std::to_string(i));
  }

  // Contents of this sizer added dynamically via refreshTagToggles().
  refreshTagToggles();

  p_tag_toggles_->FitInside();
  p_tag_toggles_->SetScrollRate(5, 5);

  sz_left->Add(p_tag_toggles_, 1, wxEXPAND | wxALL, 5);

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
  //       API used by default suppresses the media-loaded event. This also affects the mediaplayer
  //       demo provided by wxWidgets itself. To circumvent this, we explicitly choose the Windows
  //       Media Player backend.
  // See: https://docs.wxwidgets.org/stable/classwx_media_ctrl.html
  // See: https://forums.wxwidgets.org/viewtopic.php?t=47476
  // See: https://github.com/wxWidgets/wxWidgets/issues/18976   
  mc_media_display_ = new wxMediaCtrl(p_right, ID_MEDIA_CTRL, wxEmptyString, wxDefaultPosition,
      wxDefaultSize, wxMC_NO_AUTORESIZE, wxMEDIABACKEND_WMP10);
  sz_right->Add(mc_media_display_, 1, wxEXPAND | wxALL, 0);

  // TODO: Replace this temporary location with real paths once application is stable.
  const wxString debug_media_dir = wxStandardPaths::Get().GetDocumentsDir() + "/ragtag-debug/";
  //mc_media_display_->Load(debug_media_dir + "videomp4.mp4");
  mc_media_display_->Load(debug_media_dir + "imagejpg.jpg");
  //mc_media_display_->Load(debug_media_dir + "imagepng.png");

  sz_main->Add(p_left, 1, wxEXPAND | wxALL, 5);
  sz_main->Add(p_right, 1, wxEXPAND | wxALL, 5);

  Bind(wxEVT_MENU, &MainFrame::OnNew, this, ID_NEW);
  Bind(wxEVT_MENU, &MainFrame::OnOpen, this, ID_OPEN);
  Bind(wxEVT_MENU, &MainFrame::OnSave, this, ID_SAVE);
  Bind(wxEVT_MENU, &MainFrame::OnSaveAs, this, ID_SAVE_AS);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_MEDIA_LOADED, &MainFrame::OnMediaLoaded, this, ID_MEDIA_CTRL);
}

void MainFrame::refreshTagToggles() {
  bool is_file_active = active_file_.has_value();  // Shorthand

  auto all_tags = tag_map_.getAllTags();
  for (const auto& tag_element : all_tags) {
    TagTogglePanel* p_tag_toggle = new TagTogglePanel(p_tag_toggles_, tag_element.first);
    sz_tag_toggles_->Add(p_tag_toggle, 0, wxEXPAND | wxALL, 0);
    if (is_file_active) {
      // If file is active, configure the checked/unchecked marks to match applied tags.
      // TODO: Implement this!
    } else {
      // If no file is active, just use the tag configuration default.
      p_tag_toggle->setCheckBoxState(tag_element.second.default_setting);
    }
  }
}

void MainFrame::OnNew(wxCommandEvent& event) {
  if (is_dirty_) {
    const UserIntention intention = promptUnsavedChanges();
    switch (intention) {
    case UserIntention::SAVE:
      if (project_path_.has_value()) {
        if (!saveProject()) {
          // TODO: Report error.
          return;
        }
      } else {
        const std::optional<std::filesystem::path> path = promptSaveAs();
        if (!path.has_value()) {
          // User canceled dialog.
          return;
        }
        if (!saveProjectAs(*path)) {
          // Failed to save.
          // TODO: Report error.
          return;
        }
      }
      break;
    case UserIntention::DONT_SAVE:
      break;
    case UserIntention::CANCEL:
      return;
    default:
      // TODO: Log error
      return;
    }
  }

  // If we've made it this far, we have permission to create a new project.
  newProject();
  SetStatusText("Created new project.");
}

void MainFrame::OnOpen(wxCommandEvent& event) {
  if (is_dirty_) {
    const UserIntention intention = promptUnsavedChanges();
    switch (intention) {
    case UserIntention::SAVE:
      if (project_path_.has_value()) {
        if (!saveProject()) {
          // TODO: Report error.
          return;
        }
      } else {
        const std::optional<std::filesystem::path> path = promptSaveAs();
        if (!path.has_value()) {
          // User canceled dialog.
          return;
        }
        if (!saveProjectAs(*path)) {
          // Failed to save.
          // TODO: Report error.
          return;
        }
      }
      break;
    case UserIntention::DONT_SAVE:
      break;
    case UserIntention::CANCEL:
      return;
    default:
      // TODO: Log error
      return;
    }
  }

  // If we've gotten this far, we have permission to open a file.
  std::optional<std::filesystem::path> path_pending = promptOpen();
  if (!path_pending.has_value()) {
    // User canceled dialog.
    return;
  }
  std::optional<ragtag::TagMap> tag_map_pending = ragtag::TagMap::fromFile(*path_pending);
  if (!tag_map_pending.has_value()) {
    // Loading operation failed.
    // TODO: Report error
    return;
  }

  // Loaded successfully!
  tag_map_ = *tag_map_pending;
  project_path_ = path_pending;
  // TODO: Redraw dialog
  is_dirty_ = false;
  SetStatusText(L"Opened project " + project_path_->wstring() + L".");
}

void MainFrame::OnSave(wxCommandEvent& event) {
  if (!project_path_.has_value()) {
    const std::optional<std::filesystem::path> path = promptSaveAs();
    if (!path.has_value()) {
      // User canceled dialog.
      return;
    }
    if (!saveProjectAs(*path)) {
      // Failed to save.
      // TODO: Report error.
      return;
    }
    project_path_ = path;
  } else if (!saveProject()) {
    // TODO: Report error
    return;
  }

  is_dirty_ = false;
  SetStatusText(L"Saved project " + project_path_->wstring() + L".");
}

void MainFrame::OnSaveAs(wxCommandEvent& event) {
  const std::optional<std::filesystem::path> path = promptSaveAs();
  if (!path.has_value()) {
    // User canceled dialog.
    return;
  }
  if (!saveProjectAs(*path)) {
    // Failed to save.
    // TODO: Report error.
    return;
  }
  project_path_ = path;
  is_dirty_ = false;
  SetStatusText(L"Saved project " + project_path_->wstring() + L".");
}

void MainFrame::OnExit(wxCommandEvent& event) {
  if (is_dirty_) {
    const UserIntention intention = promptUnsavedChanges();
    switch (intention) {
    case UserIntention::SAVE:
      if (project_path_.has_value()) {
        if (!saveProject()) {
          // TODO: Report error.
          return;
        }
      }
      else {
        const std::optional<std::filesystem::path> path = promptSaveAs();
        if (!path.has_value()) {
          // User canceled dialog.
          return;
        }
        if (!saveProjectAs(*path)) {
          // Failed to save.
          // TODO: Report error.
          return;
        }
      }
      break;
    case UserIntention::DONT_SAVE:
      break;
    case UserIntention::CANCEL:
      return;
    default:
      // TODO: Log error
      return;
    }
  }

  // If we've gotten this far, we have permission to close.
  Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
  // (c) replacement technique from https://forums.wxwidgets.org/viewtopic.php?p=158583#p158583
  wxString about_string("RagTag v0.0.1\n\n(c) 2025 by Ed Foley");
#if wxUSE_UNICODE
  const wxString copyright_symbol = wxString::FromUTF8("\xc2\xa9");
  about_string.Replace("(c)", copyright_symbol);
#endif

  wxMessageBox(about_string, "About", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnMediaLoaded(wxMediaEvent& event) {
  mc_media_display_->Play();
}

MainFrame::UserIntention MainFrame::promptUnsavedChanges() {
  wxMessageDialog md_unsaved_changes(this, "You have unsaved changes.", "Unsaved Changes",
      wxYES_NO | wxCANCEL | wxICON_WARNING);
  md_unsaved_changes.SetYesNoCancelLabels("Save", "Don't Save", "Cancel");
  const int selection = md_unsaved_changes.ShowModal();
  switch (selection) {
  case wxID_YES:
    return UserIntention::SAVE;
    break;
  case wxID_NO:
    return UserIntention::DONT_SAVE;
    break;
  default:
  case wxID_CANCEL:
    return UserIntention::CANCEL;
  }
}

std::optional<std::filesystem::path> MainFrame::promptSaveAs() {
  wxString wx_path = wxFileSelector("Save Project As", wxEmptyString, "project.tagdef", ".tagdef",
    "RagTag project files (*.tagdef)|*.tagdef", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return std::filesystem::path(wx_path.ToStdWstring());
}

std::optional<std::filesystem::path> MainFrame::promptOpen() {
  wxString wx_path = wxFileSelector("Open Project", wxEmptyString, wxEmptyString, wxEmptyString,
      "RagTag project files (*.tagdef)|*.tagdef", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return std::filesystem::path(wx_path.ToStdWstring());
}

void MainFrame::newProject() {
  tag_map_ = ragtag::TagMap();
  project_path_.reset();
  is_dirty_ = true;
}

bool MainFrame::saveProject() {
  if (!project_path_.has_value()) {
    // This shouldn't happen per the function's preconditions.
    // TODO: Log error
    return false;
  }

  return tag_map_.toFile(*project_path_);
}

bool MainFrame::saveProjectAs(const std::filesystem::path& path) {
  return tag_map_.toFile(path);
}
