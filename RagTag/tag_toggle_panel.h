#ifndef INCLUDE_TAG_TOGGLE_PANEL_H
#define INCLUDE_TAG_TOGGLE_PANEL_H

#include "tag_map.h"
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/window.h>

//! Control for a single tag, offering the user the ability to apply it or remove it from a file.
//! Also includes buttons to modify or delete the tag itself.
//! 
//! This class uses a one-to-one translation convention between visual checkbox state and
//! TagSetting. A checked box corresponds to TagSetting::YES; an unchecked box corresponds to
//! TagSetting::NO; a box in its third, "undetermined" state corresponds to TagSetting::UNCOMMITTED.
//! 
//! This class itself does not perform modifications to the actively loaded project; it merely
//! conveys the user's intent via events. (See TagToggleEvent.)
class TagTogglePanel : public wxPanel {
public:
  //! Constructor.
  //! 
  //! @param parent The parent window.
  //! @param tag The tag this panel represents and manipulates.
  //! @param label The text displayed in this panel. (This is not required to match the tag.)
  //! @param hotkey The hotkey that activates this tag when the application is in command mode.
  //!     Supply an empty optional if this tag is not to have a hotkey. If supplied, the hotkey must
  //!     be a single capital alphabetical character between A and Z.
  TagTogglePanel(wxWindow* parent, ragtag::tag_t tag, wxString label,
    std::optional<ragtag::rtchar_t> hotkey);
  
  //! Destructor.
  virtual ~TagTogglePanel();

  //! Visually update the panel's checkbox to convey the given state.
  //! 
  //! This function does not emit a checkbox event.
  //! 
  //! @param state The state to convey via the checkbox (e.g., TagSetting::YES to check the box).
  void setCheckBoxState(ragtag::TagSetting state);

  //! Get the TagSetting appropriate for the current state of the checkbox.
  //! 
  //! @returns The TagSetting appropriate for the current state of the checkbox (e.g.,
  //!     TagSetting::YES if the checkbox is checked).
  ragtag::TagSetting getCheckBoxState() const;

  //! Grays out the checkbox and the hotkey text. Also disables the hotkey and prevents user
  //! manipulation of the checkbox.
  void disableCheckboxAndHotkey();

  //! Updates the display of the checkbox and hotkey text to show that the control is live.
  //! Re-enables the associated hotkey and re-allows user manipulation of the checkbox.
  void enableCheckboxAndHotkey();

  //! Function allowing the panel to act on wxKeyEvents triggered elsewhere.
  //! 
  //! Posts a TagToggleEvent to the parent if the key matches this panel's hotkey.
  //! 
  //! @param event The wxKeyEvent to process if possible.
  //! @returns True if the supplied event matches this panel's hotkey and the TagToggleEvent is
  //!     sent to the panel's parent.
  bool processKeyEvent(wxKeyEvent& event);

private:
  void OnClickEdit(wxCommandEvent& event);
  void OnClickDelete(wxCommandEvent& event);
  void OnCheckboxChange(wxCommandEvent& event);

  wxWindow* parent_{};
  ragtag::tag_t tag_{};
  std::optional<ragtag::rtchar_t> hotkey_{};
  bool hotkey_enabled_{ false };
  wxStaticText* st_hotkey_{ nullptr };
  wxCheckBox* cb_tag_toggle_{ nullptr };
};


class TagToggleEvent;
wxDECLARE_EVENT(TAG_TOGGLE_BUTTON_EVENT, TagToggleEvent);

// Extension of wxCommandEvent that communicates info specific to the tag being modified.
// Approach adapted from https://wiki.wxwidgets.org/Custom_Events#Subclassing_wxCommandEvent.
class TagToggleEvent : public wxCommandEvent {
public:
  enum class DesiredAction {
    NONE,
    EDIT_TAG,
    DELETE_TAG,
    UPDATE_TAG_STATE
  };

  // NOTE: wxCommandEvent ctrl allows communication of a specific ID. Until we have use for that, we
  // will default it to 0.
  TagToggleEvent(ragtag::tag_t tag, DesiredAction action,
    ragtag::TagSetting desired_state = ragtag::TagSetting::UNCOMMITTED)
    : wxCommandEvent(TAG_TOGGLE_BUTTON_EVENT, 0), tag_(tag), desired_state_(desired_state),
    desired_action_(action) {}

  TagToggleEvent(const TagToggleEvent& event) : wxCommandEvent(event) {
    tag_ = event.tag_;
    desired_state_ = event.desired_state_;
    desired_action_ = event.desired_action_;
  }

  wxEvent* Clone() const {
    return new TagToggleEvent(*this);
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

typedef void (wxEvtHandler::* TagToggleButtonEventFunction)(TagToggleEvent&);
#define TagToggleButtonEventHandler(func) wxEVENT_HANDLER_CAST(TagToggleButtonEventFunction, func)

#endif  // INCLUDE_TAG_TOGGLE_PANEL_H
