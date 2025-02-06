#ifndef INCLUDE_MAIN_FRAME_H
#define INCLUDE_MAIN_FRAME_H

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

class MainFrame : public wxFrame {
public:
  MainFrame();

private:
  enum {
    ID_NONE = 0,
    ID_NEW_PROJECT,
    ID_OPEN_PROJECT,
    ID_SAVE_PROJECT,
    ID_SAVE_PROJECT_AS,
    ID_LOAD_FILE,
    ID_NEXT_FILE,
    ID_PREVIOUS_FILE,
    ID_DEFINE_NEW_TAG,
    ID_MEDIA_CTRL,
    ID_STOP_MEDIA,
    ID_PLAY_PAUSE_MEDIA,
    ID_MUTE_BOX
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

  static const wxColour BACKGROUND_COLOR_FULLY_TAGGED;
  static const wxColour BACKGROUND_COLOR_PARTLY_TAGGED;
  static const wxColour BACKGROUND_COLOR_FULLY_UNTAGGED;

  // Functions updating view to match model
  void refreshTagToggles();
  void refreshFileView();
  // Menu events
  void OnNewProject(wxCommandEvent& event);
  void OnOpenProject(wxCommandEvent& event);
  void OnSaveProject(wxCommandEvent& event);
  void OnSaveProjectAs(wxCommandEvent& event);
  void OnLoadFile(wxCommandEvent& event);
  void OnNextFile(wxCommandEvent& event);
  void OnPreviousFile(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnAbout(wxCommandEvent& event);
  // Button events
  void OnDefineNewTag(wxCommandEvent& event);
  void OnTagToggleButtonClick(TagToggleButtonEvent& event);
  void OnStopMedia(wxCommandEvent& event);
  void OnPlayPauseMedia(wxCommandEvent& event);
  void OnPreviousUntaggedFile(wxCommandEvent& event);
  void OnNextUntaggedFile(wxCommandEvent& event);
  void OnDebug(wxCommandEvent& event);  // TODO: Remove me.
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
  // Custom dialog prompts
  UserIntention promptUnsavedChanges();
  std::optional<std::filesystem::path> promptSaveProjectAs();
  std::optional<std::filesystem::path> promptOpenProject();
  std::optional<std::filesystem::path> promptLoadFile();
  bool promptConfirmTagDeletion(ragtag::tag_t tag);
  // Fundamental project commands
  void newProject();
  bool saveProject();
  bool saveProjectAs(const std::filesystem::path& path);
  bool displayMediaFile(const std::filesystem::path& path);
  bool playMedia();
  bool pauseMedia();
  bool stopMedia();
  static std::optional<ragtag::path_t> getFileAfter(const ragtag::path_t& reference);
  std::optional<ragtag::path_t> getFileBefore(const ragtag::path_t& reference) const;
  // Helper functions
  static std::optional<ragtag::path_t> qualifiedFileNavigatorHelper(const ragtag::path_t& reference,
                                                                    bool find_next);

  wxScrolledWindow* p_tag_toggles_{ nullptr };
  wxBoxSizer* sz_tag_toggles_{ nullptr };
  wxMediaCtrl* mc_media_display_{ nullptr };
  wxCheckBox* cb_autoplay_{ nullptr };
  wxCheckBox* cb_loop_{ nullptr };
  wxCheckBox* cb_mute_{ nullptr };
  wxButton* b_play_pause_media_{ nullptr };
  wxListCtrl* lc_files_in_directory_{ nullptr };
  // Parallel array of full paths to files presented by lc_files_in_directory_, since they cannot be
  // included directly. It's not pretty design, but it's the best way I could come up with given
  // wxListCtrl's limitations.
  std::vector<ragtag::path_t> file_paths_{};
  ragtag::TagMap tag_map_{};
  std::optional<std::filesystem::path> project_path_{};
  std::optional<std::filesystem::path> active_file_{};
  bool is_dirty_{ false };
  bool user_initiated_stop_media_{ false };
};

#endif  // INCLUDE_MAIN_FRAME_H
