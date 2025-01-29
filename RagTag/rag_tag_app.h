#ifndef INCLUDE_RAG_TAG_APP_H
#define INCLUDE_RAG_TAG_APP_H

#include <wx/wx.h>

class RagTagApp : public wxApp {
public:
  bool OnInit() override;
};

wxDECLARE_APP(RagTagApp);

#endif  // INCLUDE_RAG_TAG_APP_H_
