#ifndef INCLUDE_MAIN_FRAME_H
#define INCLUDE_MAIN_FRAME_H

#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbox.h>

class MainFrame : public wxFrame {
public:
  MainFrame() : wxFrame(nullptr, wxID_ANY, "Flicker Demo", wxDefaultPosition)
  {
    wxPanel* panel_main = new wxPanel(this);
    wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
    panel_main->SetSizer(sizer_main);

    // The panel on the left will incorporate a static box sizer.
    wxPanel* panel_left = new wxPanel(panel_main);
    wxStaticBoxSizer* sizer_left = new wxStaticBoxSizer(wxVERTICAL, panel_left, "Static Box");
    panel_left->SetSizer(sizer_left);
    wxListCtrl* lc_left = new wxListCtrl(sizer_left->GetStaticBox(), wxID_ANY, wxDefaultPosition,
      wxDefaultSize, wxLC_REPORT);
    populateListControlEntries(lc_left);
    sizer_left->Add(lc_left, 1, wxEXPAND);
    sizer_main->Add(panel_left, 1, wxEXPAND);

    // The panel on the right will incorporate a basic box sizer.
    wxPanel* panel_right = new wxPanel(panel_main);
    wxBoxSizer* sizer_right = new wxBoxSizer(wxVERTICAL);
    panel_right->SetSizer(sizer_right);
    wxListCtrl* lc_right = new wxListCtrl(panel_right, wxID_ANY, wxDefaultPosition,
      wxDefaultSize, wxLC_REPORT);
    populateListControlEntries(lc_right);
    sizer_right->Add(lc_right, 1, wxEXPAND);
    sizer_main->Add(panel_right, 1, wxEXPAND);
  }

private:
  // Places lorem ipsum content in a given wxListCtrl
  void populateListControlEntries(wxListCtrl* lc)
  {
    lc->DeleteAllItems();
    lc->AppendColumn("Column");
    for (int i = 0; i < 50; ++i) {
      lc->InsertItem(i, std::to_string(i));
    }
  }
};

#endif  // INCLUDE_MAIN_FRAME_H
