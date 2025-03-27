#include "main_frame.h"
#include "rag_tag_app.h"

bool RagTagApp::OnInit() {
  // Dark mode is gorgeous but induces bad flickering that appears to be a rendering bug.
  // TODO: Change this to Appearance::System after the flickering issue is addressed (either by
  // wxWidgets or by code changes on my side).
  SetAppearance(Appearance::Light);
  MainFrame* frame = new MainFrame();
  frame->Show();
  return true;
}

// Defines application entry point. See
// https://docs.wxwidgets.org/latest/group__group__funcmacro__rtti.html#ga967aafaa261481fe2d2e1cf599e3e003
wxIMPLEMENT_APP(RagTagApp);
