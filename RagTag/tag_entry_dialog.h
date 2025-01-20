#ifndef INCLUDE_TAG_ENTRY_DIALOG
#define INCLUDE_TAG_ENTRY_DIALOG

#include "tag_map.h"
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/radiobox.h>
#include <wx/textctrl.h>
#include <wx/wx.h>

class TagEntryDialog : public wxDialog {
public:
  typedef std::pair<ragtag::tag_t, ragtag::TagProperties> tag_entry_t;

  enum {
    ID_CANCEL = 0,
    ID_OK
  };

  TagEntryDialog(wxWindow* parent);
  TagEntryDialog(wxWindow* parent, const tag_entry_t& entry_init);
  std::optional<tag_entry_t> promptTagEntry();

private:
  TagEntryDialog(wxWindow* parent, const std::optional<tag_entry_t>& entry_init_opt);
  void OnOk(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);

  std::optional<std::pair<ragtag::tag_t, ragtag::TagProperties>> entry_{};
  wxWindow* parent_{ nullptr };
  wxTextCtrl* tc_tag_name_{ nullptr };
  wxRadioBox* rb_default_setting_{ nullptr };
};

#endif  // INCLUDE_TAG_ENTRY_DIALOG
