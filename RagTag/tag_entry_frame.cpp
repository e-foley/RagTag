#include "tag_entry_frame.h"

TagEntryFrame::TagEntryFrame() : TagEntryFrame(std::optional<tag_entry_t>{}) {};

TagEntryFrame::TagEntryFrame(const tag_entry_t& entry_init)
  : TagEntryFrame(std::optional<tag_entry_t>{entry_init}) {}

TagEntryFrame::TagEntryFrame(const std::optional<tag_entry_t>& entry_init_opt)
  : wxFrame(nullptr, wxID_ANY, "Create/Modify Tag", wxDefaultPosition, wxSize(400, 300)),
  entry_(entry_init_opt) {}
