#ifndef INCLUDE_MAIN_FRAME
#define INCLUDE_MAIN_FRAME

#include <wx/wx.h>

class MainFrame : public wxFrame {
public:
  enum {
    ID_HELLO = 1
  };

  MainFrame();

private:
  void OnHello(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
};

#endif  // INCLUDE_MAIN_FRAME
