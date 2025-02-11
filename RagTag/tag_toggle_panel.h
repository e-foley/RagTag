#ifndef INCLUDE_TAG_TOGGLE_PANEL_H
#define INCLUDE_TAG_TOGGLE_PANEL_H

#include "tag_map.h"
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/window.h>

class TagTogglePanel : public wxPanel {
public:
  TagTogglePanel(wxWindow* parent, ragtag::tag_t tag, std::string label,
    std::optional<ragtag::rtchar_t> hotkey);
  virtual ~TagTogglePanel();
  // This function does not emit a checkbox event.
  void setCheckBoxState(ragtag::TagSetting state);
  ragtag::TagSetting getCheckBoxState() const;

private:
  void OnClickEdit(wxCommandEvent& event);
  void OnClickDelete(wxCommandEvent& event);
  void OnCheckboxChange(wxCommandEvent& event);
  void OnKeyDown(wxKeyEvent& event);

  ragtag::tag_t tag_{};
  std::optional<ragtag::rtchar_t> hotkey_{};
  wxCheckBox* cb_tag_toggle_{ nullptr };
};


class TagToggleButtonEvent;
wxDECLARE_EVENT(TAG_TOGGLE_BUTTON_EVENT, TagToggleButtonEvent);

// Extension of wxCommandEvent that communicates info specific to the tag being modified.
// Approach adapted from https://wiki.wxwidgets.org/Custom_Events#Subclassing_wxCommandEvent.
class TagToggleButtonEvent : public wxCommandEvent {
public:
  enum class DesiredAction {
    NONE,
    EDIT_TAG,
    DELETE_TAG,
    UPDATE_TAG_STATE
  };

  // NOTE: wxCommandEvent ctrl allows communication of a specific ID. Until we have use for that, we
  // will default it to 0.
  TagToggleButtonEvent(ragtag::tag_t tag, DesiredAction action,
    ragtag::TagSetting desired_state = ragtag::TagSetting::UNCOMMITTED)
    : wxCommandEvent(TAG_TOGGLE_BUTTON_EVENT, 0), tag_(tag), desired_state_(desired_state),
    desired_action_(action) {}

  TagToggleButtonEvent(const TagToggleButtonEvent& event) : wxCommandEvent(event) {
    tag_ = event.tag_;
    desired_state_ = event.desired_state_;
    desired_action_ = event.desired_action_;
  }

  wxEvent* Clone() const {
    return new TagToggleButtonEvent(*this);
  }

  ragtag::tag_t getTag() const {
    return tag_;
  }

  void setTag(ragtag::tag_t tag) {
    tag_ = tag;
  }

  ragtag::TagSetting getDesiredState() const {
    return desired_state_;
  }

  void setDesiredState(ragtag::TagSetting state) {
    desired_state_ = state;
  }

  DesiredAction getDesiredAction() const {
    return desired_action_;
  }

  void setDesiredAction(DesiredAction action) {
    desired_action_ = action;
  }

private:
  ragtag::tag_t tag_{};
  ragtag::TagSetting desired_state_{ragtag::TagSetting::UNCOMMITTED};
  DesiredAction desired_action_{DesiredAction::NONE};
};

typedef void (wxEvtHandler::* TagToggleButtonEventFunction)(TagToggleButtonEvent&);
#define TagToggleButtonEventHandler(func) wxEVENT_HANDLER_CAST(TagToggleButtonEventFunction, func)

#endif  // INCLUDE_TAG_TOGGLE_PANEL_H
