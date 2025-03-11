#ifndef INCLUDE_MAIN_FRAME_H
#define INCLUDE_MAIN_FRAME_H

#include <wx/frame.h>
#include <wx/listctrl.h>

class MainFrame : public wxFrame {
public:
  MainFrame();
private:
  void populateListControlEntries(wxListCtrl* lc);
};

#endif  // INCLUDE_MAIN_FRAME_H
