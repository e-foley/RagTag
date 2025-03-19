#ifndef INCLUDE_MAIN_FRAME_H
#define INCLUDE_MAIN_FRAME_H

#include "summary_frame.h"
#include "tag_map.h"
#include "tag_toggle_panel.h"
#include <filesystem>
#include <optional>
#include <wx/checkbox.h>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/mediactrl.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/tglbtn.h>

//! Primary UI window for the RagTag application that provides the user essential controls for their
//! project, such as viewing media, assigning ratings, and attaching tags.
class MainFrame : public wxFrame {
public:
  //! Constructor.
  MainFrame();

private:
  //! Type of function that enforces criteria for including a file path within grouping contexts.
  typedef std::function<bool(const ragtag::path_t&)> file_qualifier_t;

  //! Enumeration of IDs used to differentiate various in-window controls for the purposes of
  //! assigning functions to them.
  // Implementation note: IDs of 0 and 1 are not allowed per wxWidgets documentation:
  // https://docs.wxwidgets.org/latest/overview_windowids.html
  enum {
    ID_NONE = 2,
    ID_NEW_PROJECT,
    ID_OPEN_PROJECT,
    ID_SAVE_PROJECT,
    ID_SAVE_PROJECT_AS,
    ID_LOAD_FILE,
    ID_ENTER_COMMAND_MODE,
    ID_FOCUS_DIRECTORY_VIEW,
    ID_FOCUS_TAGS,
    ID_REFRESH_FILE_VIEW,
    ID_NEXT_FILE,
    ID_PREVIOUS_FILE,
    ID_NEXT_UNTAGGED_FILE,
    ID_PREVIOUS_UNTAGGED_FILE,
    ID_SHOW_SUMMARY,
    ID_CLEAR_TAGS_FROM_FILE,
    ID_SET_TAGS_TO_DEFAULTS,
    ID_DEFINE_NEW_TAG,
    ID_MEDIA_CTRL,
    ID_STOP_MEDIA,
    ID_PLAY_PAUSE_MEDIA,
    ID_TOGGLE_AUTOPLAY,
    ID_TOGGLE_LOOPING,
    ID_TOGGLE_MUTE,
    ID_NO_RATING,
    ID_RATING_0,
    ID_RATING_MAX = ID_RATING_0 + 5  // Implied ratings 1-5
  };

  //! User's intended action based on their response to a dialog.
  enum class UserIntention {
    NONE = 0,
    SAVE,
    DONT_SAVE,
    CANCEL,
  };

  //! Columns displayed within the directory viewer.
  enum FileViewColumn {
    COLUMN_FILENAME,
    COLUMN_TAG_COVERAGE,
    COLUMN_RATING
  };

  //! Color used for the background of list items for files that have been fully tagged.
  static const wxColour BACKGROUND_COLOR_FULLY_TAGGED;

  //! Color used for the background of list items for files that have been partly tagged (files that
  //! have some tag coverage).
  static const wxColour BACKGROUND_COLOR_PARTLY_TAGGED;

  //! Color used for the background of list items for files that are untagged (files that have no
  //! tag coverage).
  static const wxColour BACKGROUND_COLOR_FULLY_UNTAGGED;

  //! Starting position for the vertical divider as a proportion of the window width.
  static const double LEFT_PANE_STARTING_PROPORTION;

  //! Minimum position for the vertical divider as a proportion of the window width.
  static const double LEFT_PANE_MINIMUM_PROPORTION;

  //! Horizontal adjustment of the vertical divider as a proportion of window width changes.
  static const double LEFT_PANE_GRAVITY;

  //! Starting position of the media/directory divider as a proportion of the window height.
  static const double MEDIA_PANE_STARTING_PROPORTION;

  //! Minimum position of the media/directory divider as a proportion of the window height.
  static const double MEDIA_PANE_MINIMUM_PROPORTION;

  //! Vertical adjustment of the media/directory divider as a proportion of window height changes.
  static const double MEDIA_PANE_GRAVITY;

  // FUNCTIONS UPDATING VIEW TO MATCH MODEL ========================================================
  //! Immediately update the visual display of tag toggles to match the model, including adding or
  //! removing entries and enabling or disabling checkboxes.
  void refreshTagToggles();

  //! Immediately update the display of files within the directory to match the model.
  void refreshFileView();

  //! Immediately update the display of buttons used for rating files to match the model,
  //! potentially enabling or disabling them.
  void refreshRatingButtons();

  //! Immediately update the project summary window to match the model.
  void refreshSummary();

  //! Immediately update the window's title bar to display the path of the current project and
  //! denote whether the file has been modified.
  void refreshTitleBar();

  //! Immediately update the window's status bar to display active mode (e.g., command mode) and
  //! whether the project has been modified.
  void refreshStatusBar();

  // Custom dialog prompts
  UserIntention promptUnsavedChanges();
  std::optional<ragtag::path_t> promptSaveProjectAs();
  std::optional<ragtag::path_t> promptOpenProject();
  std::optional<ragtag::path_t> promptLoadFile();
  bool promptConfirmTagDeletion(ragtag::tag_t tag);
  bool promptConfirmFileDeletion(const ragtag::path_t& path);
  bool promptSaveOpportunityIfDirty();
  void notifyCouldNotSaveProject(const ragtag::path_t& path);
  void notifyCouldNotOpenProject(const ragtag::path_t& path);
  // Fundamental project commands
  void markDirty();
  void markClean();
  void newProject();
  bool saveProject();
  bool saveProjectAs(const ragtag::path_t& path);
  bool loadFileAndSetAsActive(const ragtag::path_t& path);
  void resetActiveFile();
  bool openProject(const ragtag::path_t& path);
  bool displayMediaFile(const ragtag::path_t& path);
  bool playMedia();
  bool pauseMedia();
  bool stopMedia();
  bool clearRatingOfActiveFile();
  bool setRatingOfActiveFile(ragtag::rating_t rating);
  bool loadNextFile();
  bool loadPreviousFile();
  bool loadNextUntaggedFile();
  bool loadPreviousUntaggedFile();
  void enterCommandMode();
  void exitCommandMode();
  // Helper functions
  static std::optional<ragtag::path_t> qualifiedFileNavigator(
    const ragtag::path_t& reference, const file_qualifier_t& qualifier, bool find_next);
  std::optional<long> getPathListCtrlIndex(const ragtag::path_t& path) const;
  static ragtag::path_t getBackupPath(const ragtag::path_t& nominal_path);

  // MENU EVENTS ===================================================================================
  // All functions are invoked upon selecting them via the window's menu or executing the
  // corresponding accelerator. Function signature is dictated by wxEvtHandler::Bind() requirements
  // for events of type wxEVT_MENU.
  void OnNewProject(wxCommandEvent& event);
  void OnOpenProject(wxCommandEvent& event);
  void OnSaveProject(wxCommandEvent& event);
  void OnSaveProjectAs(wxCommandEvent& event);
  void OnEnterCommandMode(wxCommandEvent& event);
  void OnFocusDirectoryView(wxCommandEvent& event);
  void OnFocusTags(wxCommandEvent& event);
  void OnShowSummary(wxCommandEvent& event);
  void OnLoadFile(wxCommandEvent& event);
  void OnRefreshFileView(wxCommandEvent& event);
  void OnNextFile(wxCommandEvent& event);
  void OnPreviousFile(wxCommandEvent& event);
  void OnToggleMuteMenu(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  // CONTROL EVENTS =================================================================================
  // These functions are invoked by the user's interactions with controls in the window.
  // Events of type wxEVT_BUTTON
  void OnClearTagsFromFile(wxCommandEvent& event);
  void OnSetTagsToDefaults(wxCommandEvent& event);
  void OnDefineNewTag(wxCommandEvent& event);
  void OnTagToggleButtonClick(TagToggleEvent& event);
  void OnStopMedia(wxCommandEvent& event);
  void OnPlayPauseMedia(wxCommandEvent& event);
  void OnPreviousUntaggedFile(wxCommandEvent& event);
  void OnNextUntaggedFile(wxCommandEvent& event);
  void OnClickRatingButton(wxCommandEvent& event);
  // Events of type wxEVT_CHECKBOX
  void OnToggleAutoplay(wxCommandEvent& event);
  void OnToggleLooping(wxCommandEvent& event);
  void OnToggleMuteBox(wxCommandEvent& event);
  // Events of type wxEVT_LIST_ITEM_FOCUSED
  void OnFocusFile(wxListEvent& event);

  // MEDIA EVENTS ==================================================================================
  // Events related to display of the media file.
  void OnMediaLoaded(wxMediaEvent& event);    // wxEVT_MEDIA_LOADED
  void OnMediaStop(wxMediaEvent& event);      // wxEVT_MEDIA_STOP
  void OnMediaFinished(wxMediaEvent& event);  // wxEVT_MEDIA_FINISHED
  void OnMediaPlay(wxMediaEvent& event);      // wxEVT_MEDIA_PLAY
  void OnMediaPause(wxMediaEvent& event);     // wxEVT_MEDIA_PAUSE

  // WINDOW EVENTS =================================================================================
  // Events related to the handling of top-level windows themselves.
  void OnClose(wxCloseEvent& event);      // wxEVT_CLOSE_WINDOW
  void OnKillFocus(wxFocusEvent& event);  // wxEVT_KILL_FOCUS
  // Custom event handler for actions taken within the project summary. (See SummaryFrameEvent.)
  void OnSummaryFrameAction(SummaryFrameEvent& event);

  // KEYBOARD EVENTS ===============================================================================
  void OnKeyDown(wxKeyEvent& event);  // wxEVT_CHAR_DOWN

  wxMenu* m_file_{ nullptr };
  wxMenu* m_project_{ nullptr };
  wxMenu* m_media_{ nullptr };
  wxMenu* m_tags_{ nullptr };
  wxMenu* m_window_{ nullptr };
  wxMenu* m_help_{ nullptr };
  wxScrolledWindow* p_tag_toggles_{ nullptr };
  wxBoxSizer* sz_tag_toggles_{ nullptr };
  wxMediaCtrl* mc_media_display_{ nullptr };
  wxButton* b_stop_media_{ nullptr };
  wxButton* b_play_pause_media_{ nullptr };
  wxCheckBox* cb_autoplay_{ nullptr };
  wxCheckBox* cb_loop_{ nullptr };
  wxCheckBox* cb_mute_{ nullptr };
  wxToggleButton* b_no_rating_{ nullptr };
  std::vector<wxToggleButton*> b_ratings_{6};  // 6 ratings: 0 through 5 inclusive
  wxButton* b_clear_tags_from_file_{ nullptr };
  wxButton* b_set_tags_to_defaults_{ nullptr };
  wxStaticText* st_current_directory_{ nullptr };
  wxListCtrl* lc_files_in_directory_{ nullptr };
  wxButton* b_refresh_file_view_{ nullptr };
  wxButton* b_previous_untagged_file_{ nullptr };
  wxButton* b_next_untagged_file_{ nullptr };
  SummaryFrame* f_summary_{ nullptr };
  // Parallel array of full paths to files presented by lc_files_in_directory_, since they cannot be
  // included directly. It's not pretty design, but it's the best way I could come up with given
  // wxListCtrl's limitations.
  std::vector<ragtag::path_t> file_paths_{};
  ragtag::TagMap tag_map_{};
  std::optional<ragtag::path_t> project_path_{};
  std::optional<ragtag::path_t> active_file_{};
  std::vector<TagTogglePanel*> tag_toggle_panels_{};
  bool is_dirty_{ false };
  bool user_initiated_stop_media_{ false };
  bool file_view_modification_in_progress_{ false };
  bool command_mode_active_{ true };
};

#endif  // INCLUDE_MAIN_FRAME_H
