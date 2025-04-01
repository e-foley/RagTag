// Copyright (C) 2025 by Edward Foley
//
// This file is part of RagTag.
//
// RagTag is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// RagTag is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with RagTag. If not, see
// <https://www.gnu.org/licenses/>.

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
  //! Fires when the user activates the Edit button. Posts a TagToggleEvent to the panel's parent
  //! indicating DesiredAction::EDIT_TAG.
  //! 
  //! @param event The fundamental wxCommandEvent triggered by the button activation.
  void OnClickEdit(wxCommandEvent& event);

  //! Fires when the user activates the Delete button ("X"). Posts a TagToggleEvent to the panel's
  //! parent indicating DesiredAction::DELETE_TAG.
  //! 
  //! @param event The fundamental wxCommandEvent triggered by the button activation.
  void OnClickDelete(wxCommandEvent& event);

  //! Fires when the state of the panel's checkbox changes by user action (clicking or otherwise
  //! activating it). Posts a TagToggleEvent to the panel's parent indicating
  //! DesiredAction::UPDATE_TAG_STATE along with the new state the checkbox indicates.
  //! 
  //! @param event The fundamental wxCommandEvent triggered by the checkbox activation.
  void OnCheckboxChange(wxCommandEvent& event);

  //! Parent window of the panel.
  wxWindow* parent_{};

  //! Tag this panel controls.
  ragtag::tag_t tag_{};

  //! Hotkey controlling this panel.
  std::optional<ragtag::rtchar_t> hotkey_{};

  //! Whether the hotkey is enabled.
  bool hotkey_enabled_{ false };

  //! Text displaying the hotkey assigned to this tag.
  wxStaticText* st_hotkey_{ nullptr };

  //! Checkbox the user manipulates to set the state of the panel's assigned tag.
  wxCheckBox* cb_tag_toggle_{ nullptr };
};

// Forward-declare TagToggleEvent so that wxWidgets' wxDECLARE_EVENT macro can do what it needs.
class TagToggleEvent;
wxDECLARE_EVENT(TAG_TOGGLE_BUTTON_EVENT, TagToggleEvent);

//! Extension of wxCommandEvent that communicates info specific to the tag being modified.
//! 
//! Approach adapted from https://wiki.wxwidgets.org/Custom_Events#Subclassing_wxCommandEvent.
class TagToggleEvent : public wxCommandEvent {
public:
  //! The fundamental action this event describes.
  enum class DesiredAction {
    NONE,             //!< No action.
    EDIT_TAG,         //!< The user wants to edit the tag.
    DELETE_TAG,       //!< The user wants to delete the tag.
    UPDATE_TAG_STATE  //!< The user wants to change the state of the tag on the active file.
  };

  // Implementation note: wxCommandEvent allows communication of a specific ID. Until we have use
  // for that, we will default it to 0.
  //! Constructor.
  //! 
  //! @param tag The tag the event refers to.
  //! @param action The user's desired action with respect to the indicated tag.
  //! @param desired_state The desired state of the tag on the current file. (Irrelevant except for
  //!     DesiredAction::UPDATE_TAG_STATE.)
  TagToggleEvent(ragtag::tag_t tag, DesiredAction action,
    ragtag::TagSetting desired_state = ragtag::TagSetting::UNCOMMITTED)
    : wxCommandEvent(TAG_TOGGLE_BUTTON_EVENT, 0), tag_(tag), desired_state_(desired_state),
    desired_action_(action) {}

  //! Copy constructor.
  //! 
  //! @param event The event to copy.
  TagToggleEvent(const TagToggleEvent& event) : wxCommandEvent(event) {
    tag_ = event.tag_;
    desired_state_ = event.desired_state_;
    desired_action_ = event.desired_action_;
  }

  //! Duplicates this TagToggleEvent as a new TagToggleEvent instance with identical state.
  //! 
  //! @returns A pointer to a new TagToggleEvent instance with state matching the object on which
  //!     this function was invoked.
  wxEvent* Clone() const {
    return new TagToggleEvent(*this);
  }

  //! Retrieves the tag associated with this event.
  //! 
  //! @returns The tag associated with this event.
  ragtag::tag_t getTag() const {
    return tag_;
  }

  //! Changes the tag this event refers to.
  //! 
  //! @param tag The tag to change this event's tag reference to.
  void setTag(ragtag::tag_t tag) {
    tag_ = tag;
  }

  //! Gets the desired tag state conveyed by this event.
  //! 
  //! @returns The desired tag state conveyed by this event.
  ragtag::TagSetting getDesiredState() const {
    return desired_state_;
  }

  //! Modifies the desired tag state conveyed by this event.
  //! 
  //! @param state The desired tag state conveyed by this event.
  void setDesiredState(ragtag::TagSetting state) {
    desired_state_ = state;
  }

  //! Gets the user's desired action conveyed by this event.
  //! 
  //! @returns The user's desired action conveyed by this event.
  DesiredAction getDesiredAction() const {
    return desired_action_;
  }

  //! Modifies the user's desired action this event conveys.
  //! 
  //! @param action The user action this event is to convey.
  void setDesiredAction(DesiredAction action) {
    desired_action_ = action;
  }

private:
  //! Tag this event refers to.
  ragtag::tag_t tag_{};

  //! The state of this tag desired by the user.
  ragtag::TagSetting desired_state_{ragtag::TagSetting::UNCOMMITTED};

  //! The action this event describes.
  DesiredAction desired_action_{DesiredAction::NONE};
};

//! Event handler function type as required by custom events in wxWidgets.
typedef void (wxEvtHandler::* TagToggleButtonEventFunction)(TagToggleEvent&);
#define TagToggleButtonEventHandler(func) wxEVENT_HANDLER_CAST(TagToggleButtonEventFunction, func)

#endif  // INCLUDE_TAG_TOGGLE_PANEL_H
