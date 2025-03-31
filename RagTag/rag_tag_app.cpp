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
