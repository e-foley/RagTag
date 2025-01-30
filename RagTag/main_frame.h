#ifndef INCLUDE_MAIN_FRAME_H
#define INCLUDE_MAIN_FRAME_H

#include "tag_map.h"
#include "tag_toggle_panel.h"
#include <filesystem>
#include <optional>
#include <wx/mediactrl.h>
#include <wx/wx.h>

class MainFrame : public wxFrame {
public:
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
  };

  enum class UserIntention {
    NONE = 0,
    SAVE,
    DONT_SAVE,
    CANCEL,
  };

  MainFrame();

private:
  void refreshTagToggles();

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
  // Miscellaneous events
  void OnMediaLoaded(wxMediaEvent& event);
  // Custom dialog prompts
  UserIntention promptUnsavedChanges();
  std::optional<std::filesystem::path> promptSaveAs();
  std::optional<std::filesystem::path> promptOpen();
  bool promptConfirmTagDeletion(ragtag::tag_t tag);
  // Fundamental project commands
  void newProject();
  bool saveProject();
  bool saveProjectAs(const std::filesystem::path& path);

  wxScrolledWindow* p_tag_toggles_{ nullptr };
  wxBoxSizer* sz_tag_toggles_{ nullptr };
  wxMediaCtrl* mc_media_display_{ nullptr };
  ragtag::TagMap tag_map_{};
  std::optional<std::filesystem::path> project_path_{};
  std::optional<std::filesystem::path> active_file_{};
  bool is_dirty_{ false };
};

#endif  // INCLUDE_MAIN_FRAME_H
