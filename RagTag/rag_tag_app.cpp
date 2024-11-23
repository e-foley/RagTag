#include "main_frame.h"
#include "rag_tag_app.h"

bool RagTagApp::OnInit() {
  MainFrame* frame = new MainFrame();
  frame->Show();
  return true;
}

wxIMPLEMENT_APP(RagTagApp);
