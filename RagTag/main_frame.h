#ifndef INCLUDE_MAIN_FRAME
#define INCLUDE_MAIN_FRAME

#include "tag_map.h"
#include <filesystem>
#include <optional>
#include <wx/wx.h>
#include <wx/mediactrl.h>

class MainFrame : public wxFrame {
public:
  enum {
    ID_NONE = 0,
    ID_NEW,
    ID_OPEN,
    ID_SAVE,
    ID_SAVE_AS,
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
  void OnNew(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnSaveAs(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  // Button events
  void OnDefineNewTag(wxCommandEvent& event);
  // Miscellaneous events
  void OnMediaLoaded(wxMediaEvent& event);
  UserIntention promptUnsavedChanges();
  std::optional<std::filesystem::path> promptSaveAs();
  std::optional<std::filesystem::path> promptOpen();
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

#endif  // INCLUDE_MAIN_FRAME
