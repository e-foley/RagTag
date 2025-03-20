#ifndef INCLUDE_RAG_TAG_APP_H
#define INCLUDE_RAG_TAG_APP_H

#include <wx/wx.h>

//! Top-level application for RagTag.
class RagTagApp : public wxApp {
public:
  //! Presents MainFrame, the primary user interface window for the application.
  //! 
  //! @returns True to continue running or false to denote an immediate exit.
  bool OnInit() override;
};

wxDECLARE_APP(RagTagApp);

#endif  // INCLUDE_RAG_TAG_APP_H_
