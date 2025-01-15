#ifndef INCLUDE_TAG_TOGGLE_PANEL
#define INCLUDE_TAG_TOGGLE_PANEL

#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/window.h>
#include "tag_map.h"

class TagTogglePanel : public wxPanel {
public:
  TagTogglePanel(wxWindow* parent, std::string label);
  virtual ~TagTogglePanel();
  void setCheckBoxState(const ragtag::TagSetting& state);
  ragtag::TagSetting getCheckBoxState() const;

private:
  wxCheckBox* cb_tag_toggle_{ nullptr };
};

#endif  // INCLUDE_MAIN_FRAME
