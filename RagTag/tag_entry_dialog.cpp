#include "tag_entry_frame.h"

TagEntryDialog::TagEntryDialog(wxWindow* parent)
  : TagEntryDialog(parent, std::optional<tag_entry_t>{}) {};

TagEntryDialog::TagEntryDialog(wxWindow* parent, const tag_entry_t& entry_init)
  : TagEntryDialog(parent, std::optional<tag_entry_t>{entry_init}) {}

TagEntryDialog::TagEntryDialog(wxWindow* parent, const std::optional<tag_entry_t>& entry_init_opt)
  : wxDialog(parent, wxID_ANY, "Create/Modify Tag", wxDefaultPosition, wxSize(400, 300)),
  entry_(entry_init_opt) {}
