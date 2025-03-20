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

// Forward declaration macro required by wxWidgets. See
// https://docs.wxwidgets.org/latest/group__group__funcmacro__rtti.html#ga1523a2d553dea288d66cd35e8a0ffd5c
wxDECLARE_APP(RagTagApp);

#endif  // INCLUDE_RAG_TAG_APP_H_
