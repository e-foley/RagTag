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
    ID_NEW = 1,
    ID_OPEN,
    ID_SAVE,
    ID_SAVE_AS,
    ID_MEDIA_CTRL,
  };

  MainFrame();

private:
  void OnHello(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnMediaLoaded(wxMediaEvent& event);

  wxMediaCtrl* mc_media_display_{ nullptr };
  std::optional<ragtag::TagMap> tag_map_{};
  std::filesystem::path project_path_{};
};

#endif  // INCLUDE_MAIN_FRAME
