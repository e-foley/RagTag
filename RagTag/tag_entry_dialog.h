#ifndef INCLUDE_TAG_ENTRY_DIALOG
#define INCLUDE_TAG_ENTRY_DIALOG

#include "tag_map.h"
#include <wx/dialog.h>

class TagEntryDialog : public wxDialog {
public:
  typedef std::pair<ragtag::tag_t, ragtag::TagProperties> tag_entry_t;

  TagEntryDialog(wxWindow* parent);
  TagEntryDialog(wxWindow* parent, const tag_entry_t& entry_init);

private:
  TagEntryDialog(wxWindow* parent, const std::optional<tag_entry_t>& entry_init_opt);
  std::optional<std::pair<ragtag::tag_t, ragtag::TagProperties>> entry_{};
};

#endif  // INCLUDE_TAG_ENTRY_DIALOG
