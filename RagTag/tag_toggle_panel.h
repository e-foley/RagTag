#ifndef INCLUDE_TAG_TOGGLE_PANEL
#define INCLUDE_TAG_TOGGLE_PANEL

#include <wx/panel.h>
#include <wx/window.h>

class TagTogglePanel : public wxPanel {
public:
  TagTogglePanel(wxWindow* parent, std::string label);
  virtual ~TagTogglePanel();

private:
  // ...
};

#endif  // INCLUDE_MAIN_FRAME
