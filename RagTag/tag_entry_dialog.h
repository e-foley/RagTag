#ifndef INCLUDE_TAG_ENTRY_DIALOG_H
#define INCLUDE_TAG_ENTRY_DIALOG_H

#include "tag_map.h"
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <wx/radiobox.h>
#include <wx/textctrl.h>
#include <wx/wx.h>

class TagEntryDialog : public wxDialog {
public:
  struct Response {
    ragtag::tag_t tag;
    ragtag::TagProperties tag_properties;
    bool apply_to_all_project_files{ false };
  };

  TagEntryDialog(wxWindow* parent);
  TagEntryDialog(wxWindow* parent, ragtag::tag_t tag, const ragtag::TagProperties& tag_properties);
  std::optional<Response> promptTagEntry();
  void OnKeyDown(wxKeyEvent& event);

private:
  void OnOk(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);

  Response response_{};
  bool response_confirmed_{ false };
  wxWindow* parent_{ nullptr };
  wxTextCtrl* tc_tag_name_{ nullptr };
  wxTextCtrl* tc_hotkey_{ nullptr };
  wxRadioBox* rb_default_setting_{ nullptr };
  wxCheckBox* cb_apply_to_all_files_{ nullptr };
};

#endif  // INCLUDE_TAG_ENTRY_DIALOG_H
