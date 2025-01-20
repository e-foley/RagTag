#ifndef INCLUDE_TAG_ENTRY_FRAME
#define INCLUDE_TAG_ENTRY_FRAME

#include "tag_map.h"
#include <wx/dialog.h>

class TagEntryFrame : public wxDialog {
public:
  typedef std::pair<ragtag::tag_t, ragtag::TagProperties> tag_entry_t;

  TagEntryFrame(wxWindow* parent);
  TagEntryFrame(wxWindow* parent, const tag_entry_t& entry_init);

private:
  TagEntryFrame(wxWindow* parent, const std::optional<tag_entry_t>& entry_init_opt);
  std::optional<std::pair<ragtag::tag_t, ragtag::TagProperties>> entry_{};
};

#endif
