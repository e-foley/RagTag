// Copyright (C) 2025 by Edward Foley
//
// This file is part of RagTag.
//
// RagTag is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// RagTag is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with RagTag. If not, see
// <https://www.gnu.org/licenses/>.

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
