#include "tag_entry_frame.h"

TagEntryFrame::TagEntryFrame(wxWindow* parent)
  : TagEntryFrame(parent, std::optional<tag_entry_t>{}) {};

TagEntryFrame::TagEntryFrame(wxWindow* parent, const tag_entry_t& entry_init)
  : TagEntryFrame(parent, std::optional<tag_entry_t>{entry_init}) {}

TagEntryFrame::TagEntryFrame(wxWindow* parent, const std::optional<tag_entry_t>& entry_init_opt)
  : wxDialog(parent, wxID_ANY, "Create/Modify Tag", wxDefaultPosition, wxSize(400, 300)),
  entry_(entry_init_opt) {}
