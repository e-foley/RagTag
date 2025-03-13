#include "main_frame.h"
#include "rag_tag_util.h"
#include "tag_entry_dialog.h"
#include <wx/filedlg.h>
#include <wx/splitter.h>
#include <wx/statusbr.h>
#include <wx/stdpaths.h>

const wxColour MainFrame::BACKGROUND_COLOR_FULLY_TAGGED = wxColour(200, 255, 200);
const wxColour MainFrame::BACKGROUND_COLOR_PARTLY_TAGGED = wxColour(255, 255, 200);
const wxColour MainFrame::BACKGROUND_COLOR_FULLY_UNTAGGED = wxColour(255, 255, 255);
const double MainFrame::LEFT_PANE_STARTING_PROPORTION = 0.5;
const double MainFrame::LEFT_PANE_MINIMUM_PROPORTION = 0.4;
const double MainFrame::LEFT_PANE_GRAVITY = 0.75;
const double MainFrame::MEDIA_PANE_STARTING_PROPORTION = 0.55;
const double MainFrame::MEDIA_PANE_MINIMUM_PROPORTION = 0.25;
const double MainFrame::MEDIA_PANE_GRAVITY = 0.75;

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "RagTag v0.0.1", wxDefaultPosition,
  wxSize(900, 720)) {
  SetMinSize(wxSize(800, 540));

  wxMenu* m_file = new wxMenu;
  m_file->Append(ID_NEW_PROJECT, "&New Project...\tCtrl-N");
  m_file->Append(ID_OPEN_PROJECT, "&Open Project...\tCtrl-Shift-O");
  m_file->Append(ID_SAVE_PROJECT, "&Save Project...\tCtrl-S");
  m_file->Append(ID_SAVE_PROJECT_AS, "Save Project &As...\tCtrl-Shift-S");
  m_file->AppendSeparator();
  m_file->Append(wxID_EXIT, "&Quit\tAlt-F4");

  wxMenu* m_project = new wxMenu;
  m_project->Append(ID_LOAD_FILE, "&Load File...\tCtrl-O");
  m_project->AppendSeparator();
  m_project->Append(ID_NEXT_FILE, "Next File in Directory\t.");
  m_project->Append(ID_PREVIOUS_FILE, "Previous File in Directory\t,");
  m_project->Append(ID_NEXT_UNTAGGED_FILE, "Next Untagged File in Directory\tSpace");
  m_project->Append(ID_PREVIOUS_UNTAGGED_FILE, "Previous Untagged File in Directory\tShift-Space");

  wxMenu* m_tags = new wxMenu;
  m_tags->Append(ID_DEFINE_NEW_TAG, "Define New &Tag...\tCtrl-T");
  m_tags->Append(ID_CLEAR_TAGS_FROM_FILE, "&Clear Tags from Active File\tCtrl-C");
  m_tags->Append(ID_SET_TAGS_TO_DEFAULTS, "&Default Tags on Active File\tCtrl-D");

  wxMenu* m_window = new wxMenu;
  m_window->Append(ID_SHOW_SUMMARY, "Show &Project Summary\tCtrl-P");
  m_window->AppendSeparator();
  m_window->Append(ID_REFRESH_FILE_VIEW, "&Refresh Directory View\tF5");

  wxMenu* m_help = new wxMenu;
  m_help->Append(wxID_ABOUT);

  wxMenuBar* mb_menu_bar = new wxMenuBar;
  mb_menu_bar->Append(m_file, "&File");
  mb_menu_bar->Append(m_project, "&Project");
  mb_menu_bar->Append(m_tags, "&Tags");
  mb_menu_bar->Append(m_window, "&Window");
  mb_menu_bar->Append(m_help, "&Help");

  SetMenuBar(mb_menu_bar);

  CreateStatusBar();

  wxPanel* p_main = new wxPanel(this, wxID_ANY);
  wxBoxSizer* sz_main = new wxBoxSizer(wxHORIZONTAL);
  p_main->SetSizer(sz_main);

  wxSplitterWindow* sw_main = new wxSplitterWindow(p_main, wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
  wxPanel* p_left = new wxPanel(sw_main, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxBORDER_NONE);
  wxBoxSizer* sz_left = new wxBoxSizer(wxVERTICAL);
  p_left->SetSizer(sz_left);
  wxPanel* p_right = new wxPanel(sw_main, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxBORDER_NONE);
  wxBoxSizer* sz_right = new wxBoxSizer(wxVERTICAL);
  p_right->SetSizer(sz_right);

  sw_main->SplitVertically(p_left, p_right, GetSize().x * LEFT_PANE_STARTING_PROPORTION);
  sw_main->SetSize(wxSize(GetSize().x * LEFT_PANE_STARTING_PROPORTION, -1));
  sw_main->SetSashGravity(LEFT_PANE_GRAVITY);
  sw_main->SetMinimumPaneSize(GetSize().x * LEFT_PANE_MINIMUM_PROPORTION);

  wxSplitterWindow* sw_left = new wxSplitterWindow(p_left, wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

  wxPanel* p_media = new wxPanel(sw_left);

  // Note: When using StaticBoxSizer, items added to the panel actually need the sizer's StaticBox
  // to be their parent instead of the panel like other sizers would have. I don't know exactly why
  // this is, but wxWidgets is consistent at advising us to do this, and who are we to question it?
  wxStaticBoxSizer* sz_media = new wxStaticBoxSizer(wxVERTICAL, p_media, "Media");
  p_media->SetSizer(sz_media);

  // Note: Even though wxWidgets says we should "almost certainly leave [the backend selection] up
  //       to wxMediaCtrl," a documented bug involving wxWidgets' interaction with the DirectShow
  //       API used by default suppresses the media-loaded event. This also affects the mediaplayer
  //       demo provided by wxWidgets itself. To circumvent this, we explicitly choose the Windows
  //       Media Player backend.
  // See: https://docs.wxwidgets.org/stable/classwx_media_ctrl.html
  // See: https://forums.wxwidgets.org/viewtopic.php?t=47476
  // See: https://github.com/wxWidgets/wxWidgets/issues/18976   
  mc_media_display_ = new wxMediaCtrl(sz_media->GetStaticBox(), ID_MEDIA_CTRL, wxEmptyString,
    wxDefaultPosition, wxDefaultSize, wxMC_NO_AUTORESIZE, wxMEDIABACKEND_WMP10);
  mc_media_display_->Bind(wxEVT_MEDIA_LOADED, &MainFrame::OnMediaLoaded, this);
  mc_media_display_->Bind(wxEVT_MEDIA_STOP, &MainFrame::OnMediaStop, this);
  mc_media_display_->Bind(wxEVT_MEDIA_FINISHED, &MainFrame::OnMediaFinished, this);
  mc_media_display_->Bind(wxEVT_MEDIA_PLAY, &MainFrame::OnMediaPlay, this);
  mc_media_display_->Bind(wxEVT_MEDIA_PAUSE, &MainFrame::OnMediaPause, this);
  sz_media->Add(mc_media_display_, 1, wxEXPAND | wxALL, 5);

  wxPanel* p_media_buttons = new wxPanel(sz_media->GetStaticBox(), wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxBORDER_NONE);
  wxBoxSizer* sz_media_buttons = new wxBoxSizer(wxHORIZONTAL);
  p_media_buttons->SetSizer(sz_media_buttons);
  b_stop_media_ = new wxButton(p_media_buttons, ID_STOP_MEDIA, "Stop");
  b_stop_media_->Disable();
  b_stop_media_->Bind(wxEVT_BUTTON, &MainFrame::OnStopMedia, this);
  sz_media_buttons->Add(b_stop_media_, 1, wxALL, 5);
  b_play_pause_media_ = new wxButton(p_media_buttons, ID_PLAY_PAUSE_MEDIA, "Play");
  b_play_pause_media_->Disable();
  b_play_pause_media_->Bind(wxEVT_BUTTON, &MainFrame::OnPlayPauseMedia, this, ID_PLAY_PAUSE_MEDIA);
  sz_media_buttons->Add(b_play_pause_media_, 1, wxALL, 5);
  sz_media->Add(p_media_buttons, 0, wxEXPAND | wxALL, 0);

  wxPanel* p_media_options = new wxPanel(sz_media->GetStaticBox(), wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxBORDER_NONE);
  wxBoxSizer* sz_media_options = new wxBoxSizer(wxHORIZONTAL);
  p_media_options->SetSizer(sz_media_options);
  cb_autoplay_ = new wxCheckBox(p_media_options, wxID_ANY, "Autoplay");
  cb_autoplay_->SetValue(true);  // True == checked
  sz_media_options->Add(cb_autoplay_, 1, wxALL, 5);
  cb_loop_ = new wxCheckBox(p_media_options, wxID_ANY, "Loop");
  cb_loop_->SetValue(true);  // True == checked
  sz_media_options->Add(cb_loop_, 1, wxALL, 5);
  cb_mute_ = new wxCheckBox(p_media_options, ID_MUTE_BOX, "Mute");
  cb_mute_->Bind(wxEVT_CHECKBOX, &MainFrame::OnMuteBoxToggle, this);
  cb_mute_->SetValue(true);  // True == checked
  sz_media_options->Add(cb_mute_, 1, wxALL, 5);
  sz_media->Add(p_media_options, 0, wxEXPAND | wxALL, 0);

  wxPanel* p_directory = new wxPanel(sw_left);
  wxStaticBoxSizer* sz_directory = new wxStaticBoxSizer(wxVERTICAL, p_directory, "Directory");
  p_directory->SetSizer(sz_directory);

  wxPanel* p_current_directory_line = new wxPanel(sz_directory->GetStaticBox());
  wxBoxSizer* sz_current_directory_line = new wxBoxSizer(wxHORIZONTAL);
  p_current_directory_line->SetSizer(sz_current_directory_line);
  wxStaticText* st_current_directory_label = new wxStaticText(p_current_directory_line, wxID_ANY,
    "Path: ");
  sz_current_directory_line->Add(st_current_directory_label, 0, wxALL, 5);
  st_current_directory_ = new wxStaticText(p_current_directory_line, wxID_ANY, wxEmptyString,
    wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_START | wxST_NO_AUTORESIZE);
  sz_current_directory_line->Add(st_current_directory_, 1, wxALL, 5);
  sz_directory->Add(p_current_directory_line, 0, wxEXPAND | wxALL, 0);

  lc_files_in_directory_ = new wxListCtrl(sz_directory->GetStaticBox(), wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
  // Workaround to wxWidgets issue 25238 that causes list control to flicker when placed within
  // a static box sizer. (See https://github.com/wxWidgets/wxWidgets/issues/25238. Thank you, GitHub
  // user PBfordev.)
  //
  // TODO: Remove this line if/when issue 25238 is fixed.
  lc_files_in_directory_->MSWDisableComposited();
  lc_files_in_directory_->InsertColumn(COLUMN_FILENAME, "File", wxLIST_FORMAT_LEFT, 250);
  lc_files_in_directory_->InsertColumn(COLUMN_RATING, "Rating", wxLIST_FORMAT_LEFT, 80);
  lc_files_in_directory_->InsertColumn(COLUMN_TAG_COVERAGE, "Tag Coverage", wxLIST_FORMAT_LEFT, 85);
  lc_files_in_directory_->Bind(wxEVT_LIST_ITEM_FOCUSED, &MainFrame::OnFocusFile, this);
  refreshFileView();
  sz_directory->Add(lc_files_in_directory_, 1, wxEXPAND | wxALL, 5);

  wxPanel* p_file_navigation = new wxPanel(sz_directory->GetStaticBox());
  wxBoxSizer* sz_file_navigation = new wxBoxSizer(wxHORIZONTAL);
  p_file_navigation->SetSizer(sz_file_navigation);
  wxButton* b_open_file = new wxButton(p_file_navigation, ID_LOAD_FILE, "Load File...");
  b_open_file->Bind(wxEVT_BUTTON, &MainFrame::OnLoadFile, this);
  sz_file_navigation->Add(b_open_file, 1, wxEXPAND | wxALL, 5);
  b_refresh_file_view_ = new wxButton(p_file_navigation, ID_REFRESH_FILE_VIEW, "Refresh");
  b_refresh_file_view_->Disable();
  b_refresh_file_view_->Bind(wxEVT_BUTTON, &MainFrame::OnRefreshFileView, this);
  sz_file_navigation->Add(b_refresh_file_view_, 1, wxEXPAND | wxALL, 5);
  b_previous_file_ = new wxButton(p_file_navigation, ID_PREVIOUS_FILE,
    "Previous\nUntagged");
  b_previous_file_->Disable();
  b_previous_file_->Bind(wxEVT_BUTTON, &MainFrame::OnPreviousUntaggedFile, this);
  sz_file_navigation->Add(b_previous_file_, 1, wxEXPAND | wxALL, 5);
  b_next_file_ = new wxButton(p_file_navigation, ID_NEXT_FILE, "Next\nUntagged");
  b_next_file_->Disable();
  b_next_file_->Bind(wxEVT_BUTTON, &MainFrame::OnNextUntaggedFile, this);
  sz_file_navigation->Add(b_next_file_, 1, wxEXPAND | wxALL, 5);
  sz_directory->Add(p_file_navigation, 0, wxEXPAND | wxALL, 0);

  sw_left->SplitHorizontally(p_media, p_directory, GetSize().y * MEDIA_PANE_STARTING_PROPORTION);
  sw_left->SetSize(wxSize(-1, GetSize().y * MEDIA_PANE_STARTING_PROPORTION));
  sw_left->SetMinimumPaneSize(GetSize().y * MEDIA_PANE_MINIMUM_PROPORTION);
  sw_left->SetSashGravity(MEDIA_PANE_GRAVITY);
  sz_left->Add(sw_left, 1, wxEXPAND | wxALL, 5);

  wxPanel* p_rating = new wxPanel(p_right);
  wxStaticBoxSizer* sz_rating = new wxStaticBoxSizer(wxVERTICAL, p_rating, "Rating");
  p_rating->SetSizer(sz_rating);

  wxPanel* p_rating_buttons = new wxPanel(sz_rating->GetStaticBox(), wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxBORDER_NONE);
  wxBoxSizer* sz_rating_buttons = new wxBoxSizer(wxHORIZONTAL);
  p_rating_buttons->SetSizer(sz_rating_buttons);
  sz_rating_buttons->AddStretchSpacer(1);
  b_no_rating_ = new wxToggleButton(p_rating_buttons, ID_NO_RATING, " None ",
    wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  b_no_rating_->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnClickRatingButton, this);
  sz_rating_buttons->Add(b_no_rating_, 0, wxEXPAND | wxALL, 2);
  for (int r = 0; r <= 5; ++r) {
    wxToggleButton* b_rating = new wxToggleButton(p_rating_buttons, ID_RATING_0 + r,
      " " + std::to_string(r) + RagTagUtil::GLYPH_RATING_FULL_STAR + " ", wxDefaultPosition,
      wxDefaultSize, wxBU_EXACTFIT);
    b_rating->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnClickRatingButton, this);
    b_ratings_[r] = b_rating;
    sz_rating_buttons->Add(b_rating, 0, wxEXPAND | wxALL, 2);
  }
  sz_rating_buttons->AddStretchSpacer(1);
  refreshRatingButtons();
  sz_rating->Add(p_rating_buttons, 0, wxEXPAND | wxALL, 0);
  sz_right->Add(p_rating, 0, wxEXPAND | wxALL, 5);

  wxPanel* p_tags = new wxPanel(p_right);
  wxStaticBoxSizer* sz_tags = new wxStaticBoxSizer(wxVERTICAL, p_tags, "Tags");
  p_tags->SetSizer(sz_tags);

  p_tag_toggles_ = new wxScrolledWindow(sz_tags->GetStaticBox(), wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxBORDER_NONE);
  sz_tag_toggles_ = new wxBoxSizer(wxVERTICAL);
  p_tag_toggles_->SetSizer(sz_tag_toggles_);

  // Contents of this sizer added dynamically via refreshTagToggles().
  refreshTagToggles();
  p_tag_toggles_->FitInside();
  p_tag_toggles_->SetScrollRate(5, 5);
  sz_tags->Add(p_tag_toggles_, 1, wxEXPAND | wxALL, 0);

  wxPanel* p_tag_toggles_button_bar = new wxPanel(sz_tags->GetStaticBox(), wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
  wxBoxSizer* sz_tag_toggles_button_bar = new wxBoxSizer(wxHORIZONTAL);
  p_tag_toggles_button_bar->SetSizer(sz_tag_toggles_button_bar);
  b_clear_tags_from_file_ = new wxButton(p_tag_toggles_button_bar, ID_CLEAR_TAGS_FROM_FILE,
    "Clear Tags\nfrom File");
  b_clear_tags_from_file_->Bind(wxEVT_BUTTON, &MainFrame::OnClearTagsFromFile, this);
  sz_tag_toggles_button_bar->Add(b_clear_tags_from_file_, 1, wxEXPAND | wxALL, 5);
  b_set_tags_to_defaults_ = new wxButton(p_tag_toggles_button_bar, ID_SET_TAGS_TO_DEFAULTS,
    "Set Tags\nto Defaults");
  b_set_tags_to_defaults_->Bind(wxEVT_BUTTON, &MainFrame::OnSetTagsToDefaults, this);
  sz_tag_toggles_button_bar->Add(b_set_tags_to_defaults_, 1, wxEXPAND | wxALL, 5);
  wxButton* b_define_new_tag = new wxButton(p_tag_toggles_button_bar, ID_DEFINE_NEW_TAG,
    "Define\nNew Tag...");
  b_define_new_tag->Bind(wxEVT_BUTTON, &MainFrame::OnDefineNewTag, this);
  sz_tag_toggles_button_bar->Add(b_define_new_tag, 1, wxEXPAND | wxALL, 5);
  sz_tags->Add(p_tag_toggles_button_bar, 0, wxEXPAND | wxALL, 0);

  sz_right->Add(p_tags, 1, wxEXPAND | wxALL, 5);
  sz_main->Add(sw_main, 1, wxEXPAND | wxALL, 0);

  f_summary_ = new SummaryFrame(this);
  refreshSummary();

  Layout();  // Without a Layout() here, certain controls get squished.

  Bind(wxEVT_MENU, &MainFrame::OnNewProject, this, ID_NEW_PROJECT);
  Bind(wxEVT_MENU, &MainFrame::OnOpenProject, this, ID_OPEN_PROJECT);
  Bind(wxEVT_MENU, &MainFrame::OnSaveProject, this, ID_SAVE_PROJECT);
  Bind(wxEVT_MENU, &MainFrame::OnSaveProjectAs, this, ID_SAVE_PROJECT_AS);
  Bind(wxEVT_MENU, &MainFrame::OnShowSummary, this, ID_SHOW_SUMMARY);
  Bind(wxEVT_MENU, &MainFrame::OnLoadFile, this, ID_LOAD_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnRefreshFileView, this, ID_REFRESH_FILE_VIEW);
  Bind(wxEVT_MENU, &MainFrame::OnNextFile, this, ID_NEXT_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnPreviousFile, this, ID_PREVIOUS_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnNextUntaggedFile, this, ID_NEXT_UNTAGGED_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnPreviousUntaggedFile, this, ID_PREVIOUS_UNTAGGED_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnDefineNewTag, this, ID_DEFINE_NEW_TAG);
  Bind(wxEVT_MENU, &MainFrame::OnClearTagsFromFile, this, ID_CLEAR_TAGS_FROM_FILE);
  Bind(wxEVT_MENU, &MainFrame::OnSetTagsToDefaults, this, ID_SET_TAGS_TO_DEFAULTS);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
  Bind(wxEVT_CHAR_HOOK, &MainFrame::OnKeyDown, this);
  // TODO: I only seem to be able to handle this custom event when it's bound to the frame and not
  // directly to the TagTogglePanel object. (All other button events are bound directly to their
  // object.) Figure out the difference and implement a fix such that the custom event matches how
  // ordinary events on other objects are handled.
  Bind(TAG_TOGGLE_BUTTON_EVENT, &MainFrame::OnTagToggleButtonClick, this);
  Bind(SUMMARY_FRAME_EVENT, &MainFrame::OnSummaryFrameAction, this);

  // TODO: Replace this temporary location with real paths once application is stable.
  //const wxString debug_media_dir = wxStandardPaths::Get().GetDocumentsDir() + "/ragtag-debug/";
  const wxString debug_project_dir = wxStandardPaths::Get().GetDocumentsDir() + "/ragtag-debug/";
  //displayMediaFile(std::wstring(debug_media_dir) + L"videomp4.mp4");
  //displayMediaFile(std::wstring(debug_media_dir) + L"imagejpg.jpg");
  //displayMediaFile(std::wstring(debug_media_dir) + L"imagepng.png");
  openProject(std::wstring(debug_project_dir) + L"beachcoolgradient.tagdef");
}

void MainFrame::refreshTagToggles() {
  // Freeze to temporarily prevent redrawing as we rebuild our list of tag toggles.
  p_tag_toggles_->Freeze();

  // Clear out existing UI entries.
  sz_tag_toggles_->Clear(true);

  // Clear out cache of toggle panels.
  tag_toggle_panels_.clear();

  bool is_file_active = active_file_.has_value();  // Shorthand

  auto registered_tags = tag_map_.getAllTags();

  if (is_file_active && !tag_map_.hasFile(*active_file_)) {
    std::wcerr << "Active file '" << active_file_->generic_wstring()
      << "' isn't known to tag map in refreshTagToggles().\n";
    p_tag_toggles_->Thaw();
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
        std::wcerr << "Can't get tag properties for tag '" << tag_element.first << "'.\n";
        continue;
      }
      tag_setting = tag_props->default_setting;
    }

    if (!tag_setting.has_value()) {
      std::wcerr << "Failed to establish tag setting for '" << tag_element.first
        << "' within file '" << active_file_->generic_wstring() << "'.\n";
      continue;
    }

    TagTogglePanel* p_tag_toggle = new TagTogglePanel(p_tag_toggles_, tag_element.first,
      tag_element.first, tag_element.second.hotkey);
    tag_toggle_panels_.push_back(p_tag_toggle);
    sz_tag_toggles_->Add(p_tag_toggle, 0, wxEXPAND | wxALL, 0);
    p_tag_toggle->setCheckBoxState(*tag_setting);
    if (is_file_active) {
      p_tag_toggle->enableCheckboxAndHotkey();
    }
    else {
      p_tag_toggle->disableCheckboxAndHotkey();
    }
  }

  p_tag_toggles_->Thaw();

  // Invoking Layout() on the p_tag_toggles_ grandparent redraws the scrollbar if needed, whereas
  // invoking it on p_tag_toggles_ itself crunches entries into the existing unscrollable area.
  p_tag_toggles_->GetGrandParent()->Layout();

  // Invoking Update() here fixes an odd issue where toggling the box while the main frame is in
  // front of the summary frame can cause the checkbox event to fire but no visual update of the
  // checkbox to occur until the user mouses off the box.
  p_tag_toggles_->Update();
}

void MainFrame::refreshFileView()
{
  Freeze();

  if (!active_file_.has_value()) {
    lc_files_in_directory_->DeleteAllItems();
    file_paths_.clear();
    st_current_directory_->SetLabelText("(None)");
    Thaw();
    return;
  }

  file_paths_.clear();
  file_paths_.reserve(10000);  // Reserve enough room for large directories.

  // Cache tag list since we'll be referring to it within the loop.
  const auto all_tags = tag_map_.getAllTags();

  // Cache quantity of entries presently in the list control so that we know how many entries to
  // remove in the case we don't need as many to display this directory.
  const int num_list_view_entries_original = lc_files_in_directory_->GetItemCount();

  const ragtag::path_t parent_directory = active_file_->parent_path();
  st_current_directory_->SetLabelText(parent_directory.generic_wstring());

  int i = 0;
  // Note: directory_iterator documentation explains that the end iterator is equal to the
  // default-constructed iterator.
  for (auto file_it = std::filesystem::directory_iterator(parent_directory);
    file_it != std::filesystem::directory_iterator(); file_it++) {
    // Skip things like symlinks and such.
    if (!file_it->is_regular_file()) {
      continue;
    }

    if (i >= lc_files_in_directory_->GetItemCount()) {
      // We're expanding beyond the current capacity and need to insert a new item.
      lc_files_in_directory_->InsertItem(i, file_it->path().filename().generic_wstring());
    }
    else {
      // We're within the list's capacity and can simply edit existing data.
      lc_files_in_directory_->SetItem(i, 0, file_it->path().filename().generic_wstring());
    }
    
    file_paths_.push_back(file_it->path());
    // Set the "user data" for the list control entry to a pointer directed at the full path.
    // Yes, this is ugly and I wish there were a better way.
    lc_files_in_directory_->SetItemPtrData(i, reinterpret_cast<wxUIntPtr>(&file_paths_[i]));

    // Handle file rating...
    const auto rating_ret = tag_map_.getRating(file_it->path());
    if (rating_ret.has_value()) {
      lc_files_in_directory_->SetItem(i, COLUMN_RATING,
        RagTagUtil::getStarTextForRating(*rating_ret));
    }
    else {
      lc_files_in_directory_->SetItem(i, COLUMN_RATING, L"--");
    }
    
    const TagCoverage tag_coverage = getFileTagCoverage(file_it->path());
    switch (tag_coverage) {
      case TagCoverage::NONE:
        lc_files_in_directory_->SetItem(i, COLUMN_TAG_COVERAGE, L"None");
        lc_files_in_directory_->SetItemBackgroundColour(i, BACKGROUND_COLOR_FULLY_UNTAGGED);
        break;
      case TagCoverage::SOME:
        lc_files_in_directory_->SetItem(i, COLUMN_TAG_COVERAGE, L"Some");
        lc_files_in_directory_->SetItemBackgroundColour(i, BACKGROUND_COLOR_PARTLY_TAGGED);
        break;
      case TagCoverage::ALL:
        lc_files_in_directory_->SetItem(i, COLUMN_TAG_COVERAGE, L"All");
        lc_files_in_directory_->SetItemBackgroundColour(i, BACKGROUND_COLOR_FULLY_TAGGED);
        break;
      default:
      case TagCoverage::NO_TAGS_DEFINED:
        lc_files_in_directory_->SetItem(i, COLUMN_TAG_COVERAGE, L"--");
        lc_files_in_directory_->SetItemBackgroundColour(i, BACKGROUND_COLOR_FULLY_UNTAGGED);
        break;
    }

    ++i;
  }

  // Remove any excess list control entries that were needed to display prior directories but not
  // this one.
  //
  // Set the file view modification flag so that we don't attempt to load files as the DeleteItem()
  // function might automatically change the selected item in the list.
  file_view_modification_in_progress_ = true;
  for (int j = i; j < num_list_view_entries_original; ++j) {
    // Yes, wxWidgets uses int type for GetItemCount() but long type for indices.
    const long index_to_delete = lc_files_in_directory_->GetItemCount() - 1;
    // This deselects and de-focuses the item before deletion. This fixes a bug that suppressed list
    // item selection events when clicking the last item in a list after loading a file from a
    // directory with fewer files than the prior directory had.
    lc_files_in_directory_->SetItemState(index_to_delete, 0,
      wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    lc_files_in_directory_->DeleteItem(index_to_delete);
  }

  // Handle highlighting: Only the active file should be highlighted; all others should be
  // unhighlighted.
  auto active_file_index = getPathListCtrlIndex(*active_file_);
  for (long i = 0; i < lc_files_in_directory_->GetItemCount(); ++i) {
    if (active_file_index.has_value() && i == *active_file_index) {
      lc_files_in_directory_->SetItemState(i, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
        wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    }
    else {
      lc_files_in_directory_->SetItemState(i, 0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    }
  }

  // Without a refresh, old text can linger visually even though we've updated the data the control
  // displays.
  lc_files_in_directory_->Refresh();
  Thaw();

  file_view_modification_in_progress_ = false;
}

void MainFrame::refreshRatingButtons()
{
  if (!active_file_.has_value()) {
    // We can't rate anything if there's no active file.
    // The button will still change to appear "selected" by default, so we want to reject this.
    b_no_rating_->SetValue(false);
    b_no_rating_->Disable();
    // Disable all the buttons since they'd have no effect without an active file.
    for (int r = 0; r <= 5; ++r) {
      b_ratings_[r]->SetValue(false);
      b_ratings_[r]->Disable();
    }
    return;
  }

  b_no_rating_->Enable();
  for (int r = 0; r <= 5; ++r) {
    b_ratings_[r]->Enable();
  }

  const auto rating = tag_map_.getRating(*active_file_);
  if (!rating.has_value()) {
    b_no_rating_->SetValue(true);
    for (int r = 0; r <= 5; ++r) {
      b_ratings_[r]->SetValue(false);
    }
    return;
  }

  b_no_rating_->SetValue(false);
  for (int r = 0; r <= 5; ++r) {
    b_ratings_[r]->SetValue(r == *rating);
  }
}

void MainFrame::refreshSummary()
{
  f_summary_->setTagMap(tag_map_);
  f_summary_->refreshTagFilter();
  f_summary_->refreshFileList();
}

void MainFrame::OnNewProject(wxCommandEvent& event) {
  if (!promptSaveOpportunityIfDirty()) {
    return;
  }

  // If we've made it this far, we have permission to create a new project.
  newProject();
  resetActiveFile();
  SetStatusText("Created new project.");
}

void MainFrame::OnOpenProject(wxCommandEvent& event) {
  if (!promptSaveOpportunityIfDirty()) {
    return;
  }

  // If we've gotten this far, we have permission to open a file.
  std::optional<ragtag::path_t> path_pending = promptOpenProject();
  if (!path_pending.has_value()) {
    // User canceled dialog.
    return;
  }

  if (!openProject(*path_pending)) {
    notifyCouldNotOpenProject(*path_pending);
    return;
  }
}

void MainFrame::OnSaveProject(wxCommandEvent& event) {
  if (!project_path_.has_value()) {
    const std::optional<ragtag::path_t> path = promptSaveProjectAs();
    if (!path.has_value()) {
      // User canceled dialog.
      return;
    }
    if (!saveProjectAs(*path)) {
      notifyCouldNotSaveProject(*path);
      return;
    }
    project_path_ = path;
  } else if (!saveProject()) {
    notifyCouldNotSaveProject(*project_path_);
    return;
  }

  is_dirty_ = false;
  SetStatusText(L"Saved project '" + project_path_->generic_wstring() + L"'.");
}

void MainFrame::OnSaveProjectAs(wxCommandEvent& event) {
  const std::optional<ragtag::path_t> path = promptSaveProjectAs();
  if (!path.has_value()) {
    // User canceled dialog.
    return;
  }
  if (!saveProjectAs(*path)) {
    notifyCouldNotSaveProject(*path);
    return;
  }
  project_path_ = path;
  is_dirty_ = false;
  SetStatusText(L"Saved project '" + project_path_->generic_wstring() + L"'.");
}

void MainFrame::OnShowSummary(wxCommandEvent& event)
{
  if (f_summary_->IsShown()) {
    f_summary_->SetFocus();
  }
  else {
    f_summary_->Show();
  }
}

void MainFrame::OnLoadFile(wxCommandEvent& event)
{
  std::optional<ragtag::path_t> path_pending = promptLoadFile();
  if (!path_pending.has_value()) {
    // User canceled dialog.
    return;
  }

  loadFileAndSetAsActive(*path_pending);
}

void MainFrame::OnRefreshFileView(wxCommandEvent& event)
{
  refreshFileView();
}

void MainFrame::OnNextFile(wxCommandEvent& event)
{
  if (!active_file_.has_value()) {
    // Can't find next file when there's no current file.
    SetStatusText("No active file.");
    return;
  }

  const auto next_file = qualifiedFileNavigator(
    *active_file_, [](const ragtag::path_t&) {return true; }, true);
  if (!next_file.has_value()) {
    SetStatusText("Couldn't find next file.");
    return;
  }

  loadFileAndSetAsActive(*next_file);
}

void MainFrame::OnPreviousFile(wxCommandEvent& event)
{
  if (!active_file_.has_value()) {
    // Can't find previous file when there's no current file.
    SetStatusText("No active file.");
    return;
  }

  const auto previous_file = qualifiedFileNavigator(
    *active_file_, [](const ragtag::path_t&) {return true; }, false);
  if (!previous_file.has_value()) {
    SetStatusText("Couldn't find previous file.");
    return;
  }

  loadFileAndSetAsActive(*previous_file);
}

void MainFrame::OnExit(wxCommandEvent& event) {
  Close(false);  // `false` allows action to be vetoed in OnClose event handler.
}

void MainFrame::OnClose(wxCloseEvent& event) {
  if (!event.CanVeto()) {
    Destroy();
    return;
  }

  if (promptSaveOpportunityIfDirty()) {
    Destroy();
  }
  else {
    event.Veto();
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

void MainFrame::OnClearTagsFromFile(wxCommandEvent& event)
{
  if (!active_file_.has_value()) {
    return;
  }

  for (auto tag : tag_map_.getAllTags()) {
    tag_map_.clearTag(*active_file_, tag.first);
  }

  refreshTagToggles();
  refreshFileView();
  refreshSummary();
}

void MainFrame::OnSetTagsToDefaults(wxCommandEvent& event)
{
  if (!active_file_.has_value()) {
    return;
  }

  for (auto tag : tag_map_.getAllTags()) {
    tag_map_.setTag(*active_file_, tag.first, tag.second.default_setting);
  }

  refreshTagToggles();
  refreshFileView();
  refreshSummary();
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

void MainFrame::OnPreviousUntaggedFile(wxCommandEvent& event)
{
  if (!active_file_.has_value()) {
    // Can't find previous untagged file when there's no current file.
    SetStatusText("No active file.");
    return;
  }

  const auto previous_untagged = qualifiedFileNavigator(*active_file_,
    [this](const ragtag::path_t& path) {
      const auto tag_coverage = getFileTagCoverage(path);
      return tag_coverage == TagCoverage::NONE || tag_coverage == TagCoverage::SOME;
    }, false);
  if (!previous_untagged.has_value()) {
    SetStatusText("Couldn't find previous untagged file.");
    return;
  }

  loadFileAndSetAsActive(*previous_untagged);
}

void MainFrame::OnNextUntaggedFile(wxCommandEvent& event)
{
  if (!active_file_.has_value()) {
    // Can't find next untagged file when there's no current file.
    SetStatusText("No active file.");
    return;
  }

  const auto next_untagged_file = qualifiedFileNavigator(*active_file_,
    [this](const ragtag::path_t& path) {
      const auto tag_coverage = getFileTagCoverage(path);
      return tag_coverage == TagCoverage::NONE || tag_coverage == TagCoverage::SOME;
    }, true);
  if (!next_untagged_file.has_value()) {
    SetStatusText("Couldn't find next untagged file.");
    return;
  }

  loadFileAndSetAsActive(*next_untagged_file);
}

void MainFrame::OnClickRatingButton(wxCommandEvent& event)
{
  if (!active_file_.has_value()) {
    // wxWidgets shows puts the button in its "selected" state upon a click, but we don't want this.
    // Instead, we refresh the buttons to make them all show as unselected.
    refreshRatingButtons();
    return;
  }

  if (event.GetId() == ID_NO_RATING) {
    if (!clearRatingOfActiveFile()) {
      // TODO: Report error.
      SetStatusText("Could not clear rating from active file.");
    }
  }
  else {
    if (!setRatingOfActiveFile(event.GetId() - ID_RATING_0)) {
      // TODO: Report error.
      SetStatusText("Could not set rating on active file.");
    }
  }
}

void MainFrame::OnMuteBoxToggle(wxCommandEvent& event)
{
  mc_media_display_->SetVolume(cb_mute_->IsChecked() ? 0.0 : 1.0);
}

void MainFrame::OnFocusFile(wxListEvent& event)
{
  if (file_view_modification_in_progress_) {
    // When items are removed from the list view, such as when redrawing the control after loading a
    // file from a directory with fewer items than the current item's directory, the currently
    // "focused" items can change without user intent. The associated event--this function--fires
    // and causes the files to be loaded transiently, potentially triggering an unintended file to
    // be marked as the presently active file and/or causing a crash.
    //
    // Instead, we just eschew loading when we're told that the control is being modified.
    return;
  }

  // Load and display the file.
  // 
  // Gross, but the best I could come up with given wxListCtrl's limitations.
  // Goal is to reinterpret the user data as a pointer leading to the path name that was set for
  // this item in refreshFileView().
  const wxUIntPtr user_data = lc_files_in_directory_->GetItemData(event.GetIndex());
  if (!loadFileAndSetAsActive(*reinterpret_cast<ragtag::path_t*>(user_data))) {
    // TODO: Log error.
  }
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

  if (tag_map_.isTagRegistered(tag_entry_result->tag)) {
    // The "newly created" tag has a name that's already registered.
    // TODO: Report error.
    SetStatusText(L"Tag '" + tag_entry_result->tag + L"' is already registered.");
    return;
  }

  if (!tag_map_.registerTag(tag_entry_result->tag, tag_entry_result->tag_properties)) {
    // TODO: Report error.
    SetStatusText(L"Could not register tag '" + tag_entry_result->tag + L"'.");
    return;
  }

  if (tag_entry_result->apply_to_all_project_files) {
    auto all_files = tag_map_.getAllFiles();
    for (auto file : all_files) {
      // TODO: Log if any of these fails.
      tag_map_.setTag(file, tag_entry_result->tag,
        tag_entry_result->tag_properties.default_setting);
    }
  }

  // Assign the tag's default to the currently opened file, if applicable.
  if (active_file_.has_value()) {
    if (!tag_map_.setTag(*active_file_, tag_entry_result->tag,
      tag_entry_result->tag_properties.default_setting)) {
      // TODO: Report error.
      SetStatusText(L"Could not set tag '" + tag_entry_result->tag + L"' on currently open file.");
    }
  }

  // Looks like everything was successful. Refresh the panel to show our new tag.
  is_dirty_ = true;
  refreshTagToggles();
  refreshFileView();
  refreshSummary();
}

void MainFrame::OnTagToggleButtonClick(TagToggleEvent& event) {
  switch (event.getDesiredAction()) {
  case TagToggleEvent::DesiredAction::EDIT_TAG: {
    // Cache existing tag properties for convenience.
    const ragtag::tag_t old_tag = event.getTag();
    const auto props_ret = tag_map_.getTagProperties(old_tag);
    if (!props_ret.has_value()) {
      // Shouldn't happen.
      SetStatusText(L"Couldn't get existing tag properties for tag '" + old_tag + L"'.");
      break;
    }
    const ragtag::TagProperties old_props = *props_ret;

    TagEntryDialog* tag_entry_frame = new TagEntryDialog(this, old_tag, old_props);
    auto tag_entry_result = tag_entry_frame->promptTagEntry();
    if (!tag_entry_result.has_value()) {
      // Prompt was canceled. Don't do anything.
      // TODO: Remove this debug message.
      SetStatusText("Tag entry prompt canceled.");
      break;
    }

    const ragtag::tag_t& new_tag = tag_entry_result->tag;  // Alias for convenience

    // TODO: Don't assume that changes have been made.
    is_dirty_ = true;

    // If the name is different from before, the tag has been renamed.
    if (new_tag != old_tag) {
      if (!tag_map_.renameTag(old_tag, new_tag)) {
        SetStatusText(L"Could not rename tag '" + old_tag + L"' to '" + new_tag + L"'.");
        break;
      }
    }

    if (!tag_map_.setTagProperties(new_tag, tag_entry_result->tag_properties)) {
      SetStatusText(L"Could not set properties for tag '" + new_tag + L"'.");
    }

    // Apply default to all files in project if requested.
    if (tag_entry_result->apply_to_all_project_files) {
      auto all_files = tag_map_.getAllFiles();
      for (auto file : all_files) {
        // TODO: Log if any of these fails.
        tag_map_.setTag(file, tag_entry_result->tag,
          tag_entry_result->tag_properties.default_setting);
      }
    }

    SetStatusText(L"Modified tag '" + old_tag + L"'/'" + new_tag + L"'.");
    break;
  }
  case TagToggleEvent::DesiredAction::DELETE_TAG: {
    if (promptConfirmTagDeletion(event.getTag())) {
      // TODO: Don't assume that changes have been made.
      is_dirty_ = true;

      if (tag_map_.deleteTag(event.getTag())) {
        SetStatusText(L"Deleted tag '" + event.getTag() + L"'.");
      }
      else {
        // TODO: Report error.
        SetStatusText(L"Could not delete tag '" + event.getTag() + L"'.");
      }
    }
    break;
  }
  case TagToggleEvent::DesiredAction::UPDATE_TAG_STATE: {
    if (active_file_.has_value()) {
      // TODO: Don't assume that changes have been made.
      is_dirty_ = true;

      if (!tag_map_.setTag(*active_file_, event.getTag(), event.getDesiredState())) {
        // TODO: Report error.
        SetStatusText(L"Could not assert tag '" + event.getTag() + L"' on file '"
          + active_file_->generic_wstring() + L"'.");
      }
    }
    break;
  }
  default:
    std::wcerr << "Unexpected desired action from tag toggle button.\n";
    break;
  }

  // Whether or not we know a change was made, refresh the tag toggle list to ensure we're
  // presenting the latest information to the user.
  refreshTagToggles();
  refreshFileView();
  refreshSummary();
}

void MainFrame::OnMediaLoaded(wxMediaEvent& event)
{
  if (!active_file_.has_value()) {
    // Shouldn't happen, but let's avoid a segfault just in case.
    return;
  }

  // NOTE: SetVolume() returns an undocumented bool.
  mc_media_display_->SetVolume(cb_mute_->IsChecked() ? 0.0 : 1.0);
  if (cb_autoplay_->IsChecked() && !RagTagUtil::isStaticMedia(*active_file_)) {
    // Unused bool.
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

void MainFrame::OnSummaryFrameAction(SummaryFrameEvent& event)
{
  const auto action = event.getAction();
  switch (action) {
  case SummaryFrameEvent::Action::SELECT_FILE: {
    const auto path_container = event.getPaths();
    if (path_container.empty()) {
      // This shouldn't happen.
      std::wcerr << "OnSummaryFrameAction called with empty path list.\n";
    }

    if (!loadFileAndSetAsActive(path_container[0])) {
      // TODO: Report error
      SetStatusText(L"Could not display file '" + path_container[0].generic_wstring() + L"'.");
    }
    break;
  }
  case SummaryFrameEvent::Action::REMOVE_FILES: {
    // Confirmation has already been granted if this event fires.
    int removed_file_count = 0;
    for (const auto& path : event.getPaths()) {
      // TODO: Use this bool for extra error reporting.
      if (tag_map_.removeFile(path)) {
        is_dirty_ = true;
        ++removed_file_count;
        if (active_file_.has_value() && path == *active_file_) {
          // The file we're removing from the project is the actively loaded one. Reset active_file_.
          resetActiveFile();
        }
      }
    }

    // Note that some of these already get invoked in the case that we resetActiveFile(). We can
    // optimize these redundant calls out later if we really want to.
    refreshFileView();
    refreshRatingButtons();
    refreshSummary();

    // This shouldn't be nullptr, but we still guard against it to be safe.
    if (f_summary_ != nullptr) {
      const std::string file_plural = removed_file_count == 1 ? " was" : "s were";
      wxMessageDialog dialog(f_summary_, std::to_string(removed_file_count)
        + " file" + file_plural + " removed from the project.", "Files Removed");
      dialog.ShowModal();
    }
    break;
  }
  default:
    std::wcerr << "Unrecognized SummaryFrameEvent action.\n";
    break;
  }
}

void MainFrame::OnKeyDown(wxKeyEvent& event)
{
  const int key_code = event.GetKeyCode();
  const int modifiers = event.GetModifiers();
  if (key_code == WXK_DELETE) {
    // Attempt to delete the file with prompting.
    if (active_file_.has_value() && promptConfirmFileDeletion(*active_file_)) {
      ragtag::path_t path_cache = *active_file_;  // Copy for use in error dialog.
      // Cache next file name so that we can switch to it if deletion is successful.
      const auto next_file = qualifiedFileNavigator(
        *active_file_, [](const ragtag::path_t&) {return true; }, true);
      if (!deleteFile(path_cache)) {
        // TODO: Report error.
        SetStatusText(L"Could not delete file '" + path_cache.generic_wstring() + L"'.");
      }

      // Remove the file from our project also.
      if (!tag_map_.removeFile(path_cache)) {
        SetStatusText(L"Could not remove file '" + path_cache.generic_wstring() +
          L"' from the project.");
      }

      if (next_file.has_value()) {
        loadFileAndSetAsActive(*next_file);
      }
      else {
        // User might have deleted last file in its directory.
        resetActiveFile();
      }
    }
  }
  else if (key_code == 'W' && modifiers == wxMOD_CONTROL) {
    Close(false);  // `false` allows action to be vetoed in OnClose event handler.
  }
  else if (key_code == WXK_ESCAPE) {
    p_tag_toggles_->SetFocus();  // Focus away from elements that might otherwise intercept keys.
  }
  else if (lc_files_in_directory_->HasFocus()) {
    // Don't preempt directory navigation keystrokes.
    event.Skip();
  }
  else if (key_code >= '0' && key_code <= '5') {
    if ((modifiers & wxMOD_SHIFT) != 0) {
      clearRatingOfActiveFile();
    }
    else {
      const int desired_rating = key_code - '0';
      setRatingOfActiveFile(desired_rating);
    }
  }
  else if (key_code >= WXK_NUMPAD0 && key_code <= WXK_NUMPAD5) {
    const int desired_rating = key_code - WXK_NUMPAD0;
    setRatingOfActiveFile(desired_rating);
  }
  else if (key_code == WXK_NUMPAD_DECIMAL) {
    // We provide this option because shift+numpad navigates controls by default.
    clearRatingOfActiveFile();
  }
  else {
    // Test against all the tag toggle hotkeys.
    bool key_processed = false;
    for (const auto& tag_toggle_panel : tag_toggle_panels_) {
      if (tag_toggle_panel->processKeyEvent(event)) {
        key_processed = true;
        // Could `break` here, but this would break the situation where multiple tags share a
        // hotkey. I don't think we should support this use case, but the enforcement mechanism to
        // prevent it definitely shouldn't be placed at *this* spot.
      }
    }
    if (!key_processed) {
      event.Skip();
    }
  }
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

std::optional<ragtag::path_t> MainFrame::promptSaveProjectAs() {
  wxString wx_path = wxFileSelector("Save Project As", wxEmptyString, "project.tagdef", ".tagdef",
    "RagTag project files (*.tagdef)|*.tagdef", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return ragtag::path_t(wx_path.ToStdWstring());
}

std::optional<ragtag::path_t> MainFrame::promptOpenProject() {
  wxString wx_path = wxFileSelector("Open Project", wxEmptyString, wxEmptyString, wxEmptyString,
      "RagTag project files (*.tagdef)|*.tagdef", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return ragtag::path_t(wx_path.ToStdWstring());
}

std::optional<ragtag::path_t> MainFrame::promptLoadFile() {
  // TODO: Enumerate all valid media files.
  wxString wx_path = wxFileSelector("Load File", wxEmptyString, wxEmptyString, wxEmptyString,
    "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return ragtag::path_t(wx_path.ToStdWstring());
}

bool MainFrame::promptConfirmTagDeletion(ragtag::tag_t tag)
{
  wxMessageDialog dialog(this, L"Are you sure you wish to delete tag '" + tag
    + L"'?\n\nDeleting a tag will remove it from all files in this project.", L"Confirm Tag Deletion",
    wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING);
  dialog.SetOKCancelLabels("Delete tag", "Cancel");
  return dialog.ShowModal() == wxID_OK;
}

bool MainFrame::promptConfirmFileDeletion(const ragtag::path_t& path)
{
  wxMessageDialog dialog(this, L"Are you sure you wish to delete file '"
    + path.generic_wstring() + L"'?\n\nThis action will remove the file from your machine.",
    "Confirm File Deletion", wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING);
  dialog.SetOKCancelLabels("Delete file", "Cancel");
  return dialog.ShowModal() == wxID_OK;
}

bool MainFrame::promptSaveOpportunityIfDirty()
{
  if (is_dirty_) {
    const UserIntention intention = promptUnsavedChanges();
    switch (intention) {
    case UserIntention::SAVE:
      if (project_path_.has_value()) {
        if (!saveProject()) {
          notifyCouldNotSaveProject(*project_path_);
          return false;
        }
      }
      else {
        const std::optional<ragtag::path_t> path = promptSaveProjectAs();
        if (!path.has_value()) {
          // User canceled dialog.
          return false;
        }
        if (!saveProjectAs(*path)) {
          notifyCouldNotSaveProject(*path);
          return false;
        }
      }
      break;
    case UserIntention::DONT_SAVE:
      return true;
    case UserIntention::CANCEL:
      return false;
    default:
      // TODO: Log error
      return false;
    }
  }

  // If we've gotten this far, we have permission to close.
  return true;
}

void MainFrame::notifyCouldNotSaveProject(const ragtag::path_t& path)
{
  SetStatusText(L"Could not save project '" + path.generic_wstring() + L"'.");
  wxMessageDialog dialog(this, L"Failed to save project '" + path.generic_wstring() + L"'.",
    "Failed to Save Project", wxICON_ERROR);
  dialog.ShowModal();
}

void MainFrame::notifyCouldNotOpenProject(const ragtag::path_t& path)
{
  SetStatusText(L"Could not open project '" + path.generic_wstring() + L"'.");
  wxMessageDialog dialog(this, L"Failed to open project '" + path.generic_wstring() + L"'.",
    "Failed to Open Project", wxICON_ERROR);
  dialog.ShowModal();
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

bool MainFrame::saveProjectAs(const ragtag::path_t& path) {
  return tag_map_.toFile(path);
}

bool MainFrame::loadFileAndSetAsActive(const ragtag::path_t& path)
{
  if (active_file_ == path) {
    // File is already loaded.
    return true;
  }

  if (!std::filesystem::exists(path)) {
    // TODO: Report error.
    SetStatusText(L"File '" + path.generic_wstring() + L"' does not exist.");
    return false;
  }

  active_file_ = path;

  if (!displayMediaFile(*active_file_)) {
    // File is "loaded," it just can't be displayed.
    // TODO: Display placeholder text or something in the media preview when this happens.
    SetStatusText(L"Couldn't display file '" + active_file_->generic_wstring() + L"'.");
  }

  if (RagTagUtil::isStaticMedia(*active_file_)) {
    b_stop_media_->Disable();
    b_play_pause_media_->Disable();
  }
  else {
    b_stop_media_->Enable();
    b_play_pause_media_->Enable();
  }

  const bool is_newly_added_file = !tag_map_.hasFile(*active_file_);
  if (is_newly_added_file) {
    is_dirty_ = true;

    // Declare file to our tag map.
    if (!tag_map_.addFile(*active_file_)) {
      // TODO: Report error.
      SetStatusText(L"Couldn't add file '" + active_file_->generic_wstring() + L"' to tag map.");
      return false;
    }

    // Assign default tags to our newly opened file.
    for (auto tag_it : tag_map_.getAllTags()) {
      if (!tag_map_.setTag(*active_file_, tag_it.first, tag_it.second.default_setting)) {
        // TODO: Report error.
        SetStatusText(L"Couldn't set tag on file '" + active_file_->generic_wstring() + L"'.");
        return false;
      }
    }
  }

  b_refresh_file_view_->Enable();
  b_previous_file_->Enable();
  b_next_file_->Enable();
  b_clear_tags_from_file_->Enable();
  b_set_tags_to_defaults_->Enable();

  refreshTagToggles();
  refreshFileView();
  refreshRatingButtons();
  if (is_newly_added_file) {
    // We only update the summary when the file list has changed. Otherwise, after the user selects
    // a file in the summary view, the invoking of this function instantly deselects the file that
    // was just clicked on. This makes it rather annoying to navigate project files.
    refreshSummary();
  }

  f_summary_->highlightFileIfPresent(path);

  Refresh();

  SetStatusText(L"Loaded file '" + active_file_->generic_wstring() + L"'.");
  return true;
}

void MainFrame::resetActiveFile()
{
  active_file_ = {};
  refreshTagToggles();
  refreshFileView();
  refreshRatingButtons();
  refreshSummary();
  b_refresh_file_view_->Disable();
  b_previous_file_->Disable();
  b_next_file_->Disable();
  b_clear_tags_from_file_->Disable();
  b_set_tags_to_defaults_->Disable();
  user_initiated_stop_media_ = true;
  stopMedia();
  mc_media_display_->Load(wxEmptyString);  // Loading empty path resets the media display to black.
  Refresh();
}

bool MainFrame::openProject(const ragtag::path_t& path)
{
  std::optional<ragtag::TagMap> tag_map_pending = ragtag::TagMap::fromFile(path);
  if (!tag_map_pending.has_value()) {
    return false;
  }

  // Opened successfully!
  tag_map_ = *tag_map_pending;
  project_path_ = path;
  resetActiveFile();
  is_dirty_ = false;
  SetStatusText(L"Opened project '" + project_path_->generic_wstring() + L"'.");
  return true;
}

bool MainFrame::displayMediaFile(const ragtag::path_t& path)
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

// Approach from SO user Zeltrax: https://stackoverflow.com/a/70258061
bool MainFrame::deleteFile(const ragtag::path_t& path)
{
  // `pFrom` argument needs to be double null-terminated.
  std::wstring widestr = path.wstring() + L'\0';

  SHFILEOPSTRUCT fileOp;
  fileOp.hwnd = NULL;
  fileOp.wFunc = FO_DELETE;
  fileOp.pFrom = widestr.c_str();
  fileOp.pTo = NULL;
  fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
  int result = SHFileOperation(&fileOp);

  if (result != 0) {
    return false;
  }
  else {
    return true;
  }
}

bool MainFrame::clearRatingOfActiveFile()
{
  if (!active_file_.has_value()) {
    return false;
  }

  if (!tag_map_.clearRating(*active_file_)) {
    // TODO: Log error.
    SetStatusText(L"Could not clear rating on file '" + active_file_->generic_wstring() + L"'.");
    return false;
  }

  is_dirty_ = true;
  refreshFileView();
  refreshRatingButtons();
  refreshSummary();
  return true;
}

bool MainFrame::setRatingOfActiveFile(ragtag::rating_t rating)
{
  if (!active_file_.has_value()) {
    return false;
  }

  if (!tag_map_.setRating(*active_file_, rating)) {
    // TODO: Log error.
    SetStatusText(L"Could not set rating on file '" + active_file_->generic_wstring() + L"'.");
    return false;
  }

  is_dirty_ = true;
  refreshFileView();
  refreshRatingButtons();
  refreshSummary();
  return true;
}

std::optional<ragtag::path_t> MainFrame::qualifiedFileNavigator(
  const ragtag::path_t& reference, const MainFrame::file_qualifier_t& qualifier, bool find_next)
{
  if (!reference.has_parent_path()) {
    return {};
  }

  auto isQualified = [&qualifier](const ragtag::path_t& file_it) {
    return std::filesystem::is_regular_file(file_it) && qualifier(file_it);
    };

  // Strategy is to construct a vector of paths to regular, qualified files within this directory,
  // find the entry that matches our reference file, then return the neighboring entry.
  //
  // This approach is a little cheesy, but IMO it's easier to follow than maintaining multiple
  // directory iterators that all need to be conditioned to seek only qualified files.
  std::vector<ragtag::path_t> qualified_or_self;
  for (std::filesystem::directory_iterator dir_it(reference.parent_path());
    dir_it != std::filesystem::directory_iterator(); ++dir_it) {
    // If it's qualified, add it to our vector. Also include our reference file regardless of its
    // qualifications such that we can obtain a reference index afterward.
    if (isQualified(dir_it->path()) || dir_it->path() == reference) {
      qualified_or_self.push_back(dir_it->path());
    }
  }

  if (qualified_or_self.empty()) {
    // Somehow, we have no files. (We didn't even encounter our reference file.)
    return {};
  }

  if (qualified_or_self.size() == 1) {
    // We only have our reference file, meaning no other files in the directory are qualified.
    return isQualified(reference) ? reference : std::optional<ragtag::path_t>();
  }

  for (int i = 0; i < qualified_or_self.size(); ++i) {
    if (qualified_or_self[i] == reference) {
      const size_t seek_index = find_next ? (i + 1) % qualified_or_self.size()
        : (i + qualified_or_self.size() - 1) % qualified_or_self.size();
      return qualified_or_self[seek_index];
    }
  }

  // We didn't encounter our reference file. This shouldn't happen, since we added our reference
  // file to the `qualified_or_self` vector earlier.
  //
  // TODO: Report error.
  return {};
}

MainFrame::TagCoverage MainFrame::getFileTagCoverage(const ragtag::path_t& file) const
{
  if (tag_map_.numTags() == 0) {
    return TagCoverage::NO_TAGS_DEFINED;
  }

  // Handle tag presence. If all tags within the tag map are asserted yes or no for the file, the
  // file is considered fully tagged. If some but not all are uncommitted, the file is partly
  // tagged. If all are uncommitted, the file is fully untagged ("None").
  // TODO: Consider moving this function to the TagMap interface.
  bool any_tag_is_committed = false;
  bool any_tag_is_uncommitted = false;
  for (auto tag_it : tag_map_.getAllTags()) {
    auto tag_setting = tag_map_.getTagSetting(file, tag_it.first);
    if (!tag_setting.has_value()) {
      continue;
    }

    if (*tag_setting == ragtag::TagSetting::NO || *tag_setting == ragtag::TagSetting::YES) {
      any_tag_is_committed = true;
    }
    else if (*tag_setting == ragtag::TagSetting::UNCOMMITTED) {
      any_tag_is_uncommitted = true;
    }

    // If we have a mix of committed and uncommitted, we know for sure we're partly tagged.
    // Nothing can change this, so we can return immediately.
    if (any_tag_is_committed && any_tag_is_uncommitted) {
      return TagCoverage::SOME;
    }
  }

  if (any_tag_is_committed && !any_tag_is_uncommitted) {
    return TagCoverage::ALL;
  }
  else if (!any_tag_is_committed && any_tag_is_uncommitted) {
    return TagCoverage::NONE;
  }

  // Shouldn't be possible to get here.
  // TODO: Record error.
  return TagCoverage::NONE;
}

std::optional<long> MainFrame::getPathListCtrlIndex(const ragtag::path_t& path) const
{
  for (long i = 0; i < lc_files_in_directory_->GetItemCount(); ++i) {
    // User data is a pointer to the path corresponding to the list control entry.
    const wxUIntPtr user_data = lc_files_in_directory_->GetItemData(i);
    if (path == *reinterpret_cast<ragtag::path_t*>(user_data)) {
      return i;
    }
  }

  return {};
}
