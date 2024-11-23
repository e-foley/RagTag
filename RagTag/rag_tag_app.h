#ifndef INCLUDE_RAG_TAG_APP
#define INCLUDE_RAG_TAG_APP

#include <wx/wx.h>

class RagTagApp : public wxApp {
public:
  bool OnInit() override;
};

wxDECLARE_APP(RagTagApp);

#endif  // INCLUDE_RAG_TAG_APP
