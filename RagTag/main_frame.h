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
    ID_HELLO = 1,
    ID_MEDIA_CTRL = 2
  };

  MainFrame();

  static bool writeTagMapToFile(const ragtag::TagMap& tag_map, const std::filesystem::path& path);
  static std::optional<ragtag::TagMap> readTagMapFromFile(const std::filesystem::path& path);

private:
  void OnHello(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnMediaLoaded(wxMediaEvent& event);

  wxMediaCtrl* mc_media_display_;
};

#endif  // INCLUDE_MAIN_FRAME
