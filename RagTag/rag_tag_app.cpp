#include "main_frame.h"
#include "rag_tag_app.h"

bool RagTagApp::OnInit() {
  MainFrame* frame = new MainFrame();
  frame->Show();
  return true;
}

// Defines application entry point. See
// https://docs.wxwidgets.org/latest/group__group__funcmacro__rtti.html#ga967aafaa261481fe2d2e1cf599e3e003
wxIMPLEMENT_APP(RagTagApp);
