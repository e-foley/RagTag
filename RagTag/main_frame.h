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

class MainFrame : public wxFrame {
public:
  MainFrame();

private:
  typedef std::function<bool(const ragtag::path_t&)> file_qualifier_t;

  // Implementation note: IDs of 0 and 1 are not allowed per wxWidgets documentation:
  // https://docs.wxwidgets.org/latest/overview_windowids.html
  enum {
    ID_NONE = 2,
    ID_NEW_PROJECT,
    ID_OPEN_PROJECT,
    ID_SAVE_PROJECT,
    ID_SAVE_PROJECT_AS,
    ID_LOAD_FILE,
    ID_REFRESH_FILE_VIEW,
    ID_NEXT_FILE,
    ID_PREVIOUS_FILE,
    ID_SHOW_SUMMARY,
    ID_CLEAR_TAGS_FROM_FILE,
    ID_SET_TAGS_TO_DEFAULTS,
    ID_DEFINE_NEW_TAG,
    ID_MEDIA_CTRL,
    ID_STOP_MEDIA,
    ID_PLAY_PAUSE_MEDIA,
    ID_MUTE_BOX,
    ID_NO_RATING,
    ID_RATING_0,
    ID_RATING_MAX = ID_RATING_0 + 5  // Implied ratings 1-5
  };

  enum class UserIntention {
    NONE = 0,
    SAVE,
    DONT_SAVE,
    CANCEL,
  };

  enum FileViewColumn {
    COLUMN_FILENAME,
    COLUMN_TAG_COVERAGE,
    COLUMN_RATING
  };

  enum class TagCoverage {
    NONE,
    SOME,
    ALL,
    NO_TAGS_DEFINED
  };

  static const wxColour BACKGROUND_COLOR_FULLY_TAGGED;
  static const wxColour BACKGROUND_COLOR_PARTLY_TAGGED;
  static const wxColour BACKGROUND_COLOR_FULLY_UNTAGGED;

  // Functions updating view to match model
  void refreshTagToggles();
  void refreshFileView();
  void refreshRatingButtons();
  void refreshSummary();
  // Menu events
  void OnNewProject(wxCommandEvent& event);
  void OnOpenProject(wxCommandEvent& event);
  void OnSaveProject(wxCommandEvent& event);
  void OnSaveProjectAs(wxCommandEvent& event);
  void OnShowSummary(wxCommandEvent& event);
  void OnLoadFile(wxCommandEvent& event);
  void OnRefreshFileView(wxCommandEvent& event);
  void OnNextFile(wxCommandEvent& event);
  void OnPreviousFile(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnAbout(wxCommandEvent& event);
  // Button events
  void OnClearTagsFromFile(wxCommandEvent& event);
  void OnSetTagsToDefaults(wxCommandEvent& event);
  void OnDefineNewTag(wxCommandEvent& event);
  void OnTagToggleButtonClick(TagToggleEvent& event);
  void OnStopMedia(wxCommandEvent& event);
  void OnPlayPauseMedia(wxCommandEvent& event);
  void OnPreviousUntaggedFile(wxCommandEvent& event);
  void OnNextUntaggedFile(wxCommandEvent& event);
  void OnClickRatingButton(wxCommandEvent& event);
  // Checkbox events
  void OnMuteBoxToggle(wxCommandEvent& event);
  // List control events
  void OnFocusFile(wxListEvent& event);
  // Media events
  void OnMediaLoaded(wxMediaEvent& event);
  void OnMediaStop(wxMediaEvent& event);
  void OnMediaFinished(wxMediaEvent& event);
  void OnMediaPlay(wxMediaEvent& event);
  void OnMediaPause(wxMediaEvent& event);
  // Key presses
  void OnKeyDown(wxKeyEvent& event);
  // Custom dialog prompts
  UserIntention promptUnsavedChanges();
  std::optional<ragtag::path_t> promptSaveProjectAs();
  std::optional<ragtag::path_t> promptOpenProject();
  std::optional<ragtag::path_t> promptLoadFile();
  bool promptConfirmTagDeletion(ragtag::tag_t tag);
  bool promptConfirmFileDeletion(const ragtag::path_t& path);
  // Fundamental project commands
  void newProject();
  bool saveProject();
  bool saveProjectAs(const ragtag::path_t& path);
  bool loadFileAndSetAsActive(const ragtag::path_t& path);
  bool loadProject(const ragtag::path_t& path);
  bool displayMediaFile(const ragtag::path_t& path);
  bool playMedia();
  bool pauseMedia();
  bool stopMedia();
  bool deleteFile(const ragtag::path_t& path);
  bool clearRatingOfActiveFile();
  bool setRatingOfActiveFile(ragtag::rating_t rating);
  // Helper functions
  static std::optional<ragtag::path_t> qualifiedFileNavigator(
    const ragtag::path_t& reference, const file_qualifier_t& qualifier, bool find_next);
  TagCoverage getFileTagCoverage(const ragtag::path_t& file) const;
  std::optional<long> getPathListCtrlIndex(const ragtag::path_t& path) const;


  wxScrolledWindow* p_tag_toggles_{ nullptr };
  wxBoxSizer* sz_tag_toggles_{ nullptr };
  wxMediaCtrl* mc_media_display_{ nullptr };
  wxCheckBox* cb_autoplay_{ nullptr };
  wxCheckBox* cb_loop_{ nullptr };
  wxCheckBox* cb_mute_{ nullptr };
  wxButton* b_play_pause_media_{ nullptr };
  wxToggleButton* b_no_rating_{ nullptr };
  std::vector<wxToggleButton*> b_ratings_{6};  // 6 ratings: 0 through 5 inclusive
  wxStaticText* st_current_directory_{ nullptr };
  wxListCtrl* lc_files_in_directory_{ nullptr };
  SummaryFrame* f_summary_{ nullptr };
  // Parallel array of full paths to files presented by lc_files_in_directory_, since they cannot be
  // included directly. It's not pretty design, but it's the best way I could come up with given
  // wxListCtrl's limitations.
  std::vector<ragtag::path_t> file_paths_{};
  ragtag::TagMap tag_map_{};
  std::optional<ragtag::path_t> project_path_{};
  std::optional<ragtag::path_t> active_file_{};
  bool is_dirty_{ false };
  bool user_initiated_stop_media_{ false };
  bool file_view_modification_in_progress_{ false };
};

#endif  // INCLUDE_MAIN_FRAME_H
