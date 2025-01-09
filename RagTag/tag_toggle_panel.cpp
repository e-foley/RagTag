#include "tag_toggle_panel.h"
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>

TagTogglePanel::TagTogglePanel(wxWindow* parent, std::string label)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED) {
  wxBoxSizer* sz_tag_toggle = new wxBoxSizer(wxHORIZONTAL);
  this->SetSizer(sz_tag_toggle);

  wxCheckBox* cb_tag_toggle = new wxCheckBox(this, wxID_ANY, label,
    wxDefaultPosition, wxDefaultSize, wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);
  sz_tag_toggle->Add(cb_tag_toggle, 1, wxALIGN_CENTER, 5);
  wxButton* b_tag_edit = new wxButton(this, wxID_ANY, "Edit", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  sz_tag_toggle->Add(b_tag_edit, 0, wxALIGN_CENTER, 5);
  wxButton* b_tag_delete = new wxButton(this, wxID_ANY, "X", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  sz_tag_toggle->Add(b_tag_delete, 0, wxALIGN_CENTER, 5);
}

TagTogglePanel::~TagTogglePanel() {}
