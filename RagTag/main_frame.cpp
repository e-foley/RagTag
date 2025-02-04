#include "main_frame.h"
#include "tag_entry_dialog.h"
#include "tag_toggle_panel.h"
#include <wx/filedlg.h>
#include <wx/statusbr.h>
#include <wx/stdpaths.h>

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "RagTag v0.0.1", wxDefaultPosition, wxSize(800, 600)) {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(ID_NEW_PROJECT, "&New Project...\tCtrl-N");
  menuFile->Append(ID_OPEN_PROJECT, "&Open Project...\tCtrl-Shift-O");
  menuFile->Append(ID_SAVE_PROJECT, "&Save Project...\tCtrl-S");
  menuFile->Append(ID_SAVE_PROJECT_AS, "Save Project &As...\tCtrl-Shift-S");
  menuFile->AppendSeparator();
  menuFile->Append(ID_LOAD_FILE, "Load File...\tCtrl-O");
  menuFile->Append(ID_NEXT_FILE, "Next File in Directory\tSpace");
  menuFile->Append(ID_PREVIOUS_FILE, "Previous File in Directory\tShift-Space");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT, "Quit\tAlt-F4");

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

  // Contents of this sizer added dynamically via refreshTagToggles().
  refreshTagToggles();

  p_tag_toggles_->FitInside();
  p_tag_toggles_->SetScrollRate(5, 5);

  sz_left->Add(p_tag_toggles_, 1, wxEXPAND | wxALL, 5);

  wxPanel* p_tag_toggles_button_bar = new wxPanel(p_left, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_tag_toggles_button_bar = new wxBoxSizer(wxHORIZONTAL);
  p_tag_toggles_button_bar->SetSizer(sz_tag_toggles_button_bar);
  wxButton* b_define_new_tag = new wxButton(p_tag_toggles_button_bar, ID_DEFINE_NEW_TAG, "Define New Tag...");
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

  wxPanel* p_media_buttons = new wxPanel(p_right, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_media_buttons = new wxBoxSizer(wxHORIZONTAL);
  p_media_buttons->SetSizer(sz_media_buttons);
  wxButton* b_stop_media = new wxButton(p_media_buttons, ID_STOP_MEDIA, "Stop");
  sz_media_buttons->Add(b_stop_media, 1, wxALL, 5);
  b_play_pause_media_ = new wxButton(p_media_buttons, ID_PLAY_PAUSE_MEDIA, "Play");
  sz_media_buttons->Add(b_play_pause_media_, 1, wxALL, 5);

  sz_right->Add(p_media_buttons, 0, wxEXPAND | wxALL, 5);

  wxPanel* p_media_options = new wxPanel(p_right, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_media_options = new wxBoxSizer(wxHORIZONTAL);
  p_media_options->SetSizer(sz_media_options);
  cb_autoplay_ = new wxCheckBox(p_media_options, wxID_ANY, "Autoplay");
  cb_autoplay_->SetValue(true);  // True == checked
  sz_media_options->Add(cb_autoplay_, 1, wxALL, 5);
  cb_loop_ = new wxCheckBox(p_media_options, wxID_ANY, "Loop");
  cb_loop_->SetValue(true);  // True == checked
  sz_media_options->Add(cb_loop_, 1, wxALL, 5);
  cb_mute_ = new wxCheckBox(p_media_options, ID_MUTE_BOX, "Mute");
  cb_mute_->SetValue(true);  // True == checked
  sz_media_options->Add(cb_mute_, 1, wxALL, 5);

  sz_right->Add(p_media_options, 0, wxEXPAND | wxALL, 5);

  wxListCtrl* lc_files_in_directory = new wxListCtrl(p_right, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
  lc_files_in_directory->InsertColumn(0, "File", wxLIST_FORMAT_LEFT, 250);
  lc_files_in_directory->InsertColumn(1, "Tagged?", wxLIST_FORMAT_LEFT, 80);
  sz_right->Add(lc_files_in_directory, 0, wxEXPAND | wxALL, 5);

  wxPanel* p_file_navigation = new wxPanel(p_right, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  wxBoxSizer* sz_file_navigation = new wxBoxSizer(wxHORIZONTAL);
  p_file_navigation->SetSizer(sz_file_navigation);
  wxButton* b_previous_file = new wxButton(p_file_navigation, ID_PREVIOUS_FILE, "Previous File");
  sz_file_navigation->Add(b_previous_file, 1, wxALL, 5);
  wxButton* b_open_file = new wxButton(p_file_navigation, ID_LOAD_FILE, "Load File");
  sz_file_navigation->Add(b_open_file, 1, wxALL, 5);
  wxButton* b_next_file = new wxButton(p_file_navigation, ID_NEXT_FILE, "Next File");
  sz_file_navigation->Add(b_next_file, 1, wxALL, 5);

  sz_right->Add(p_file_navigation, 0, wxEXPAND | wxALL, 5);

  // TODO: Replace this temporary location with real paths once application is stable.
  const wxString debug_media_dir = wxStandardPaths::Get().GetDocumentsDir() + "/ragtag-debug/";
  displayMediaFile(std::wstring(debug_media_dir) + L"videomp4.mp4");
  //displayMediaFile(std::wstring(debug_media_dir) + L"imagejpg.jpg");
  //displayMediaFile(std::wstring(debug_media_dir) + L"imagepng.png");

  sz_main->Add(p_left, 1, wxEXPAND | wxALL, 5);
  sz_main->Add(p_right, 1, wxEXPAND | wxALL, 5);

  Bind(wxEVT_MENU, &MainFrame::OnNewProject, this, ID_NEW_PROJECT);
  Bind(wxEVT_MENU, &MainFrame::OnOpenProject, this, ID_OPEN_PROJECT);
  Bind(wxEVT_MENU, &MainFrame::OnSaveProject, this, ID_SAVE_PROJECT);
  Bind(wxEVT_MENU, &MainFrame::OnSaveProjectAs, this, ID_SAVE_PROJECT_AS);
  Bind(wxEVT_MENU, &MainFrame::OnLoadFile, this, ID_LOAD_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnNextFile, this, ID_NEXT_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnPreviousFile, this, ID_PREVIOUS_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_BUTTON, &MainFrame::OnDefineNewTag, this, ID_DEFINE_NEW_TAG);
  Bind(wxEVT_BUTTON, &MainFrame::OnStopMedia, this, ID_STOP_MEDIA);
  Bind(wxEVT_BUTTON, &MainFrame::OnPlayPauseMedia, this, ID_PLAY_PAUSE_MEDIA);
  Bind(wxEVT_CHECKBOX, &MainFrame::OnMuteBoxToggle, this, ID_MUTE_BOX);
  Bind(wxEVT_MEDIA_LOADED, &MainFrame::OnMediaLoaded, this, ID_MEDIA_CTRL);
  Bind(wxEVT_MEDIA_STOP, &MainFrame::OnMediaStop, this, ID_MEDIA_CTRL);
  Bind(wxEVT_MEDIA_FINISHED, &MainFrame::OnMediaFinished, this, ID_MEDIA_CTRL);
  Bind(wxEVT_MEDIA_PLAY, &MainFrame::OnMediaPlay, this, ID_MEDIA_CTRL);
  Bind(wxEVT_MEDIA_PAUSE, &MainFrame::OnMediaPause, this, ID_MEDIA_CTRL);
  Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
  Bind(TAG_TOGGLE_BUTTON_EVENT, &MainFrame::OnTagToggleButtonClick, this);
}

void MainFrame::refreshTagToggles() {
  // Clear out existing UI entries.
  sz_tag_toggles_->Clear(true);

  bool is_file_active = active_file_.has_value();  // Shorthand

  auto registered_tags = tag_map_.getAllTags();

  if (is_file_active && !tag_map_.hasFile(*active_file_)) {
    std::cerr << "Active file '" << active_file_->generic_string()
      << "' isn't known to tag map in refreshTagToggles().\n";
    return;
  }

  for (const auto& tag_element : registered_tags) {
    std::optional<ragtag::TagSetting> tag_setting;
    if (is_file_active) {
      tag_setting = tag_map_.getTagSetting(*active_file_, tag_element.first);
    }
    else {
      // TODO: Replace this logic when hide TagProperties.
      const auto tag_props = tag_map_.getTagProperties(tag_element.first);
      if (!tag_props.has_value()) {
        std::cerr << "Can't get tag properties for tag '" << tag_element.first << "'.\n";
        continue;
      }
      tag_setting = tag_props->default_setting;
    }

    if (!tag_setting.has_value()) {
      std::cerr << "Failed to establish tag setting for '" << tag_element.first << "' within file '"
        << active_file_->generic_string() << "'.\n";
      continue;
    }

    TagTogglePanel* p_tag_toggle = new TagTogglePanel(p_tag_toggles_, tag_element.first);
    sz_tag_toggles_->Add(p_tag_toggle, 0, wxEXPAND | wxALL, 0);
    p_tag_toggle->setCheckBoxState(*tag_setting);
  }

  // Invoking Layout() on the p_tag_toggles_ parent redraws the scrollbar if needed, whereas
  // invoking it on p_tag_toggles_ itself crunches entries into the existing unscrollable area.
  p_tag_toggles_->GetParent()->Layout();
}

void MainFrame::refreshFileView()
{
  lc_files_in_directory_->DeleteAllItems();

  if (!active_file_.has_value()) {
    return;
  }

  std::filesystem::path parent_directory = active_file_->parent_path();

  int i = 0;
  // Note: directory_iterator documentation explains that the end iterator is equal to the
  // default-constructed iterator.
  for (auto file_it = std::filesystem::directory_iterator(parent_directory);
    file_it != std::filesystem::directory_iterator(); file_it++) {
    lc_files_in_directory_->InsertItem(i, file_it->path().filename().generic_wstring());
    ++i;
  }
}

void MainFrame::OnNewProject(wxCommandEvent& event) {
  if (is_dirty_) {
    const UserIntention intention = promptUnsavedChanges();
    switch (intention) {
    case UserIntention::SAVE:
      if (project_path_.has_value()) {
        if (!saveProject()) {
          // TODO: Report error.
          SetStatusText(L"Could not save project '" + project_path_->generic_wstring() + L"'.");
          return;
        }
      } else {
        const std::optional<std::filesystem::path> path = promptSaveProjectAs();
        if (!path.has_value()) {
          // User canceled dialog.
          return;
        }
        if (!saveProjectAs(*path)) {
          // Failed to save.
          // TODO: Report error.
          SetStatusText(L"Could not save project '" + path->generic_wstring() + L"'.");
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
  active_file_ = {};
  refreshTagToggles();
  refreshFileView();
  user_initiated_stop_media_ = true;
  stopMedia();
  SetStatusText("Created new project.");
}

void MainFrame::OnOpenProject(wxCommandEvent& event) {
  if (is_dirty_) {
    const UserIntention intention = promptUnsavedChanges();
    switch (intention) {
    case UserIntention::SAVE:
      if (project_path_.has_value()) {
        if (!saveProject()) {
          // TODO: Report error.
          SetStatusText(L"Could not save project '" + project_path_->generic_wstring() + L"'.");
          return;
        }
      } else {
        const std::optional<std::filesystem::path> path = promptSaveProjectAs();
        if (!path.has_value()) {
          // User canceled dialog.
          return;
        }
        if (!saveProjectAs(*path)) {
          // Failed to save.
          // TODO: Report error.
          SetStatusText(L"Could not save project '" + path->generic_wstring() + L"'.");
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
  std::optional<std::filesystem::path> path_pending = promptOpenProject();
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
  active_file_ = {};
  refreshTagToggles();
  refreshFileView();
  user_initiated_stop_media_ = true;
  stopMedia();
  // TODO: Find a way to either reset the wxMediaCtrl or disable playing until a file is loaded.
  is_dirty_ = false;
  SetStatusText(L"Opened project '" + project_path_->wstring() + L"'.");
}

void MainFrame::OnSaveProject(wxCommandEvent& event) {
  if (!project_path_.has_value()) {
    const std::optional<std::filesystem::path> path = promptSaveProjectAs();
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
  SetStatusText(L"Saved project '" + project_path_->wstring() + L"'.");
}

void MainFrame::OnSaveProjectAs(wxCommandEvent& event) {
  const std::optional<std::filesystem::path> path = promptSaveProjectAs();
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
  SetStatusText(L"Saved project '" + project_path_->wstring() + L"'.");
}

void MainFrame::OnLoadFile(wxCommandEvent& event)
{
  std::optional<std::filesystem::path> path_pending = promptLoadFile();
  if (!path_pending.has_value()) {
    // User canceled dialog.
    return;
  }

  active_file_ = *path_pending;

  if (!displayMediaFile(*active_file_)) {
    // File is "loaded," it just can't be displayed.
    // TODO: Display placeholder text or something in the media preview when this happens.
    SetStatusText(L"Couldn't display file '" + active_file_->generic_wstring() + L"'.");
  }

  if (!tag_map_.hasFile(*active_file_)) {
    is_dirty_ = true;

    // Declare file to our tag map.
    if (!tag_map_.addFile(*active_file_)) {
      // TODO: Report error.
      SetStatusText(L"Couldn't add file '" + active_file_->wstring() + L"' to tag map.");
      return;
    }

    // Assign default tags to our newly opened file.
    for (auto tag_it : tag_map_.getAllTags()) {
      if (!tag_map_.setTag(*active_file_, tag_it.first, tag_it.second.default_setting)) {
        // TODO: Report error.
        SetStatusText(L"Couldn't set tag on file '" + active_file_->wstring() + L"'.");
        return;
      }
    }
  }
  
  refreshFileView();
  refreshTagToggles();
  SetStatusText(L"Loaded file '" + active_file_->wstring() + L"'.");
}

void MainFrame::OnNextFile(wxCommandEvent& event)
{
  SetStatusText("Next file");
}

void MainFrame::OnPreviousFile(wxCommandEvent& event)
{
  SetStatusText("Previous file");
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
        const std::optional<std::filesystem::path> path = promptSaveProjectAs();
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

void MainFrame::OnClose(wxCloseEvent& event) {
  if (!event.CanVeto() || !is_dirty_) {
    Destroy();
    return;
  }

  auto intention = promptUnsavedChanges();
  switch (intention) {
  case UserIntention::SAVE:
    if (project_path_.has_value()) {
      if (!saveProject()) {
        // TODO: Report error.
        event.Veto();
        return;
      }

      Destroy();
    }
    else {
      const std::optional<std::filesystem::path> path = promptSaveProjectAs();
      if (!path.has_value()) {
        // User canceled dialog.
        event.Veto();
        return;
      }
      if (!saveProjectAs(*path)) {
        // Failed to save.
        // TODO: Report error.
        event.Veto();
        return;
      }

      Destroy();
    }
    break;
  case UserIntention::DONT_SAVE:
    Destroy();
    return;
  case UserIntention::CANCEL:
    event.Veto();
    return;
  default:
    // TODO: Log error.
    event.Veto();
    return;
  }
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

void MainFrame::OnStopMedia(wxCommandEvent& event) {
  user_initiated_stop_media_ = true;
  // TODO: Returns an undocumented bool that we can use if we want to.
  stopMedia();
}

void MainFrame::OnPlayPauseMedia(wxCommandEvent& event)
{
  const wxMediaState media_state = mc_media_display_->GetState();
  switch (media_state) {
  case wxMEDIASTATE_STOPPED:
  case wxMEDIASTATE_PAUSED:
    // TODO: Returns an undocumented bool that we can use here if we want to.
    playMedia();
    break;
  default:
  case wxMEDIASTATE_PLAYING:
    // TODO: Returns an undocumented bool that we can use here if we want to.
    pauseMedia();
    break;
  }
}

void MainFrame::OnMuteBoxToggle(wxCommandEvent& event)
{
  mc_media_display_->SetVolume(cb_mute_->IsChecked() ? 0.0 : 1.0);
}

void MainFrame::OnDefineNewTag(wxCommandEvent& event) {
  TagEntryDialog* tag_entry_frame = new TagEntryDialog(this);
  auto tag_entry_result = tag_entry_frame->promptTagEntry();
  if (!tag_entry_result.has_value()) {
    // Prompt was canceled. Don't do anything.
    // TODO: Remove this debug message.
    SetStatusText("Tag entry prompt canceled.");
    return;
  }

  if (tag_map_.isTagRegistered(tag_entry_result->first)) {
    // The "newly created" tag has a name that's already registered.
    // TODO: Report error.
    SetStatusText("Tag '" + tag_entry_result->first + "' is already registered.");
    return;
  }

  bool tag_register_result =
    tag_map_.registerTag(tag_entry_result->first, tag_entry_result->second);

  if (!tag_register_result) {
    // TODO: Report error.
    SetStatusText("Could not register tag '" + tag_entry_result->first + "'.");
    return;
  }

  // Looks like everything was successful. Refresh the panel to show our new tag.
  is_dirty_ = true;
  refreshTagToggles();
}

void MainFrame::OnTagToggleButtonClick(TagToggleButtonEvent& event) {
  switch (event.getDesiredAction()) {
  case TagToggleButtonEvent::DesiredAction::EDIT_TAG: {
    // Cache existing tag properties for convenience.
    const ragtag::tag_t old_tag = event.getTag();
    const auto props_ret = tag_map_.getTagProperties(old_tag);
    if (!props_ret.has_value()) {
      // Shouldn't happen.
      SetStatusText("Couldn't get existing tag properties for tag '" + old_tag + "'.");
      break;
    }
    const ragtag::TagProperties old_props = *props_ret;

    TagEntryDialog* tag_entry_frame = new TagEntryDialog(this, {old_tag, old_props});
    auto tag_entry_result = tag_entry_frame->promptTagEntry();
    if (!tag_entry_result.has_value()) {
      // Prompt was canceled. Don't do anything.
      // TODO: Remove this debug message.
      SetStatusText("Tag entry prompt canceled.");
      break;
    }

    const ragtag::tag_t& new_tag = tag_entry_result->first;  // Alias for convenience

    // TODO: Don't assume that changes have been made.
    is_dirty_ = true;

    // If the name is different from before, the tag has been renamed.
    if (new_tag != old_tag) {
      if (!tag_map_.renameTag(old_tag, new_tag)) {
        SetStatusText("Could not rename tag '" + old_tag + "' to '" + new_tag + "'.");
        break;
      }
    }

    if (!tag_map_.setTagProperties(new_tag, tag_entry_result->second)) {
      SetStatusText("Could not set properties for tag '" + new_tag + "'.");
    }

    SetStatusText("Modified tag '" + old_tag + "'/'" + new_tag + "'.");
    break;
  }
  case TagToggleButtonEvent::DesiredAction::DELETE_TAG: {
    if (promptConfirmTagDeletion(event.getTag())) {
      if (tag_map_.deleteTag(event.getTag())) {
        SetStatusText("Deleted tag '" + event.getTag() + "'.");
      }
      else {
        // TODO: Report error.
        SetStatusText("Could not delete tag '" + event.getTag() + "'.");
      }
    }
    break;
  }
  default:
    std::cerr << "Unexpected desired action from tag toggle button.\n";
    break;
  }

  // Whether or not we know a change was made, refresh the tag toggle list to ensure we're
  // presenting the latest information to the user.
  refreshTagToggles();
}

void MainFrame::OnMediaLoaded(wxMediaEvent& event)
{
  // NOTE: SetVolume() returns an undocumented bool.
  mc_media_display_->SetVolume(cb_mute_->IsChecked() ? 0.0 : 1.0);
  if (cb_autoplay_->IsChecked()) {
    playMedia();
  }
}

void MainFrame::OnMediaStop(wxMediaEvent& event)
{
  // When a media file finishes playing, it emits Finished and Stopped events nearly simultaneously.
  // Without this conditional, the button text tends to flash to "Play" then immediately back to
  // "Pause" as a video loops, which is distracting.
  //
  // I'd like to think there's a more elegant way of doing this, but this is the best approach I
  // found in the half hour I devoted to the problem.
  if (user_initiated_stop_media_ || !cb_loop_->IsChecked()) {
    b_play_pause_media_->SetLabel("Play");
  }
  user_initiated_stop_media_ = false;
}

void MainFrame::OnMediaFinished(wxMediaEvent& event)
{
  if (cb_loop_->IsChecked()) {
    playMedia();
  }
  else {
    b_play_pause_media_->SetLabel("Play");
  }
}

void MainFrame::OnMediaPlay(wxMediaEvent& event)
{
  b_play_pause_media_->SetLabel("Pause");
}

void MainFrame::OnMediaPause(wxMediaEvent& event)
{
  b_play_pause_media_->SetLabel("Play");
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

std::optional<std::filesystem::path> MainFrame::promptSaveProjectAs() {
  wxString wx_path = wxFileSelector("Save Project As", wxEmptyString, "project.tagdef", ".tagdef",
    "RagTag project files (*.tagdef)|*.tagdef", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return std::filesystem::path(wx_path.ToStdWstring());
}

std::optional<std::filesystem::path> MainFrame::promptOpenProject() {
  wxString wx_path = wxFileSelector("Open Project", wxEmptyString, wxEmptyString, wxEmptyString,
      "RagTag project files (*.tagdef)|*.tagdef", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return std::filesystem::path(wx_path.ToStdWstring());
}

std::optional<std::filesystem::path> MainFrame::promptLoadFile() {
  // TODO: Enumerate all valid media files.
  wxString wx_path = wxFileSelector("Load File", wxEmptyString, wxEmptyString, wxEmptyString,
    "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return std::filesystem::path(wx_path.ToStdWstring());
}

bool MainFrame::promptConfirmTagDeletion(ragtag::tag_t tag)
{
  wxMessageDialog* dialog = new wxMessageDialog(this, "Are you sure you wish to delete tag '" + tag
    + "'?\n\nDeleting a tag will remove it from all files in this project.", "Confirm Tag Deletion",
    wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING);
  dialog->SetOKCancelLabels("Delete tag", "Cancel");
  const int result = dialog->ShowModal();
  return result == wxID_OK;
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

bool MainFrame::displayMediaFile(const std::filesystem::path& path)
{
  bool load_result = mc_media_display_->Load(path.generic_wstring());
  // TODO: SetVolume() returns a bool that we can choose to use if we'd like.
  mc_media_display_->SetVolume(cb_mute_->IsChecked() ? 0.0 : 1.0);
  return load_result;
}

bool MainFrame::playMedia()
{
  // NOTE: Returns an undocumented bool.
  return mc_media_display_->Play();
}

bool MainFrame::pauseMedia()
{
  // NOTE: Returns an undocumented bool.
  return mc_media_display_->Pause();
}

bool MainFrame::stopMedia()
{
  // NOTE: Returns an undocumented bool.
  return mc_media_display_->Stop();
}
