#include "flicker_app.h"
#include "main_frame.h"

bool FlickerApp::OnInit() {
  MainFrame* frame = new MainFrame();
  frame->Show();
  return true;
}

wxIMPLEMENT_APP(FlickerApp);
