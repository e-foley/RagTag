#ifndef INCLUDE_TAG_TOGGLE_PANEL_H
#define INCLUDE_TAG_TOGGLE_PANEL_H

#include "tag_map.h"
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/window.h>

class TagTogglePanel : public wxPanel {
public:
  TagTogglePanel(wxWindow* parent, ragtag::tag_t tag);
  TagTogglePanel(wxWindow* parent, ragtag::tag_t tag, std::string label);
  virtual ~TagTogglePanel();
  void setCheckBoxState(ragtag::TagSetting state);
  ragtag::TagSetting getCheckBoxState() const;

private:
  void OnClickEdit(wxCommandEvent& event);
  void OnClickDelete(wxCommandEvent& event);

  ragtag::tag_t tag_{};
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
    DELETE_TAG
  };

  // NOTE: wxCommandEvent ctr allows communication of a specific ID. Until we have use for that, we
  // will default it to 0.
  TagToggleButtonEvent(ragtag::tag_t tag, DesiredAction action)
    : wxCommandEvent(TAG_TOGGLE_BUTTON_EVENT, 0), tag_(tag), desired_action_(action) {}

  TagToggleButtonEvent(const TagToggleButtonEvent& event) : wxCommandEvent(event) {
    tag_ = event.tag_;
    desired_action_ = event.desired_action_;
  }

  wxEvent* Clone() const {
    return new TagToggleButtonEvent(*this);
  }

  ragtag::tag_t getTag() const {
    return tag_;
  }

  DesiredAction getDesiredAction() const {
    return desired_action_;
  }

private:
  ragtag::tag_t tag_{};
  DesiredAction desired_action_{DesiredAction::NONE};
};

typedef void (wxEvtHandler::* TagToggleButtonEventFunction)(TagToggleButtonEvent&);
#define TagToggleButtonEventHandler(func) wxEVENT_HANDLER_CAST(TagToggleButtonEventFunction, func)

#endif  // INCLUDE_TAG_TOGGLE_PANEL_H
