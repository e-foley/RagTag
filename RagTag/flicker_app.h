#ifndef INCLUDE_FLICKER_APP_H
#define INCLUDE_FLICKER_APP_H

#include <wx/wx.h>

class FlickerApp : public wxApp {
public:
  bool OnInit() override;
};

wxDECLARE_APP(FlickerApp);

#endif  // INCLUDE_FLICKER_APP_H
