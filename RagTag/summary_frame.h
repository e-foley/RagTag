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

#ifndef INCLUDE_SUMMARY_FRAME_H
#define INCLUDE_SUMMARY_FRAME_H

#include "tag_map.h"
#include <optional>
#include <vector>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/window.h>

//! Window displaying all files that have been added to a RagTag project. Offers controls for
//! filtering these files and performing copying operations.
class SummaryFrame : public wxFrame {
public:
  //! Constructor.
  //! 
  //! @param parent The parent window.
  SummaryFrame(wxWindow* parent);

  //! Associates a tag map with this window such that its files and tags will displayed the next
  //! time the controls are refreshed.
  //! 
  //! @param tag_map The tag map to associate with this window.
  void setTagMap(const ragtag::TagMap& tag_map);

  //! Updates the list of files and table columns to match the currently loaded tag map.
  void refreshFileList();

  //! Updates the listing of tags in the tag filter dropdown to match the currently loaded tag map.
  void refreshTagFilter();

  //! Checks whether a given file is present in the file listing and highlights its line item if it
  //! is.
  //! 
  //! If the file is not present in the file listing, the function call has no effect.
  //! 
  //! @param path_to_highlight The path whose line item we will seek and highlight.
  void highlightFileIfPresent(const ragtag::path_t& path_to_highlight);

private:
  //! The index of the list control column that displays the file path.
  static const int PATH_COLUMN_INDEX;

  //! The index of the list control column that displays the file rating.
  static const int RATING_COLUMN_INDEX;

  //! The index of the first column used to display the state of a tag on the file.
  static const int FIRST_TAG_COLUMN_INDEX;

  //! The margin in pixels to preserve as empty space within the Path column when calculating the
  //! extent of ellipsized text.
  static const int PATH_EXTENT_MARGIN_PX;

  //! Helper struct used in sorting operations to communicate information not included directly in
  //! the list control.
  struct SortHelper {
    //! Pointer to the tag map used to generate the table.
    ragtag::TagMap* p_tag_map{ nullptr };

    //! The tag used for sorting (only relevant for columns representing tags).
    ragtag::tag_t tag{};

    //! True to sort ascending order; false to sort by descending order.
    bool sort_ascending{ false };
  };

  //! Interprets the state of the rating filter user interface as a rule for selecting files.
  //! 
  //! @returns A file qualifier representing the rating filter selections the user has made.
  ragtag::TagMap::file_qualifier_t getRuleFromRatingFilterUi();

  //! Interprets the state of the tag filter user interface as a rule for selecting files.
  //! 
  //! @returns A file qualifier representing the tag filter selections the user has made.
  ragtag::TagMap::file_qualifier_t getRuleFromTagFilterUi();

  //! Interprets the state of the file presence filter user interface as a rule for selecting files.
  //! 
  //! @returns A file qualifier representing the file presence filter selections the user has made.
  ragtag::TagMap::file_qualifier_t getRuleFromPresenceFilterUi();

  //! Composes a file selection rule from the state of all filter user interface elements.
  //! 
  //! @returns A file qualifier representing all filter selections the user has made.
  ragtag::TagMap::file_qualifier_t getOverallRuleFromFilterUi();

  //! Prompts the user to select a directory to copy selected files to.
  //! 
  //! This function does not actually perform the copy; it just collects the destination path.
  //! 
  //! @returns The path the user indicates as a suitable directory to copy files into, or an empty
  //! optional if the user exits the prompt without choosing a directory.
  std::optional<ragtag::path_t> promptCopyDestination();

  //! Enables or disables rating filter controls based on whether the user has allowed rated files
  //! to be displayed.
  void updateRatingFilterEnabledState();

  //! Modifies the enabled state of the "copy files" button based on whether files have been
  //! selected.
  void updateCopyButtonForSelections();

  //! Resets all filter UI elements to their default state.
  void resetFilters();

  //! Fills the Path column with the ellipsized path for each item.
  //! 
  //! This function also appends a text indicator when the file is not present on disk.
  //! 
  //! Paths are ellipsized to preserve the end of the path where possible.
  void populateAndEllipsizePathColumn();

  //! Retrieves the list of all paths selected in the file listing.
  //! 
  //! @returns A list of all paths corresponding to selected items within the file listing.
  std::vector<ragtag::path_t> getPathsOfSelectedFiles() const;

  //! Gets the file path for a file listing entry with given index.
  //! 
  //! @param index The file listing index to procure the corresponding path for.
  //! @returns The path of the file at the given index or an empty optional if the path cannot be
  //! determined (e.g., if the provided index indicates an entry that doesn't exist).
  std::optional<ragtag::path_t> getPathForItemIndex(int index) const;

  // EVENT FUNCTIONS ===============================================================================
  //! Invoked when the user clicks a heading of the file listing.
  //! 
  //! Sorts the files by that column or reverses the existing sort.
  //! 
  //! @param event The wxListEvent of type wxEVT_LIST_COL_CLICK describing the action.
  void OnClickHeading(wxListEvent& event);

  //! Invoked when the user resizes a column of the file listing.
  //! 
  //! @param event The wxListEvent of type wxEVT_LIST_COL_DRAGGING describing the action.
  void OnResizeColumn(wxListEvent& event);

  //! Invoked when the user checks the checkbox tied to a file in the file listing.
  //! 
  //! Notes it as a selected file for the purposes of bulk copying, removal, or deletion.
  //! 
  //! @param event The wxListEvent of type wxEVT_LIST_ITEM_CHECKED describing the action.
  void OnFileChecked(wxListEvent& event);

  //! Invoked when the user unchecks the checkbox tied to a file in the file listing.
  //! 
  //! Removes its status as a selected file for the purposes of bulk copying, removal, or deletion.
  //! 
  //! @param event The wxListEvent of type wxEVT_LIST_ITEM_UNCHECKED describing the action.
  void OnFileUnchecked(wxListEvent& event);

  //! Invoked when the user focus a file in the file listing (by clicking on it or otherwise
  //! selecting it).
  //! 
  //! Sends an event to the main window to display this file and set it as the active file.
  //! 
  //! @param event The wxListEvent of type wxEVT_LIST_ITEM_FOCUSED describing the action.
  void OnFileFocused(wxListEvent& event);

  //! Invoked when manipulating controls that adjust the file filter (for controls that don't need
  //! a more specialized action).
  //! 
  //! Applies the filter and refreshes the file list to show the results of the filter.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_CHECKBOX or wxEVT_COMBOBOX describing the
  //!     action.
  void OnFilterChangeGeneric(wxCommandEvent& event);

  //! Invoked when the user adjusts the slider controlling the minimum rating of the filter.
  //! 
  //! Refreshes the file list and modifies the max slider if needed to prevent overlap.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_SLIDER describing the action.
  void OnMinSliderMove(wxCommandEvent& event);

  //! Invoked when the user adjusts the slider controlling the maximum rating of the filter.
  //! 
  //! Refreshes the file list and modifies the min slider if needed to prevent overlap.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_SLIDER describing the action.
  void OnMaxSliderMove(wxCommandEvent& event);

  //! Invoked when the Show Rated checkbox is adjusted.
  //! 
  //! Adjusts the filter and refreshes the file list to show the results of the filter.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_CHECKBOX describing the action.
  void OnClickShowRated(wxCommandEvent& event);

  //! Invoked when the Reset Filters button is clicked or otherwise activated.
  //! 
  //! Sets all filters to their default configuration, then refreshes the file list to show the
  //! results of the filter.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_BUTTON describing the action.
  void OnResetFilters(wxCommandEvent& event);

  //! Invoked when the Select All Files button is clicked or otherwise activated.
  //! 
  //! Checks the boxes of all files in the file listing and considers all files selected for the
  //! purpose of bulk copying, removal, or deletion.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_BUTTON describing the action.
  void OnSelectAllFiles(wxCommandEvent& event);

  //! Invoked when the Deselect All Files button is clicked or otherwise activated.
  //! 
  //! Unchecks the boxes of all files in the file listing and considers all files not selected for
  //! the purpose of bulk copying, removal, or deletion.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_BUTTON describing the action.
  void OnDeselectAllFiles(wxCommandEvent& event);

  //! Invoked when the Copy Selected Files to Directory button is clicked or otherwise activated.
  //! 
  //! Prompts the user for a destination to copy selected files to, and performs the copy if the
  //! user selects a destination.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_BUTTON describing the action.
  void OnCopySelections(wxCommandEvent& event);

  //! Invoked when the Delete Selected Files button is clicked or otherwise activated.
  //! 
  //! Prompts the user to confirm their intent to delete these files, then performs the deletion if
  //! the user assents.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_BUTTON describing the action.
  void OnDeleteFiles(wxCommandEvent& event);

  //! Invoked when the Remove Selected Files from Project button is clicked or otherwise activated.
  //! 
  //! Prompts the user to confirm their intent to remove files from the project, then performs the
  //! removal if the user assents.
  //! 
  //! @param event The wxCommandEvent of type wxEVT_BUTTON describing the action.
  void OnRemoveFromProject(wxCommandEvent& event);

  //! Processes keyboard input.
  //! 
  //! Action is only taken through this function if the user presses F5, in which case a refresh of
  //! the file listing is performed, or the user presses Ctrl+W, which is equivalent to "closing"
  //! (for us, hiding) the project summary window.
  void OnKeyPressed(wxKeyEvent& event);

  //! Invoked when the user attempts to close the window.
  //! 
  //! Hides the window, but does not actually destroy it.
  //! 
  //! @param event The wxCloseEvent of type wxEVT_CLOSE_WINDOW describing the action.
  void OnClose(wxCloseEvent& event);

  // SORTING FUNCTIONS =============================================================================
  // Implementation note: The signature of these functions is required by wxListCtrl::SortItems().
  
  //! Logic for sorting files by their path.
  //! 
  //! @param item1 Pointer to user-defined data associated with the first list control item (which
  //!     by convention we've set to be the path for that item).
  //! @param item2 Pointer to user-defined data associated with the second list control item (which
  //!     by convention we've set to be the path for that item).
  //! @param sort_data Additional context used for sorting. As we've chosen to implement this
  //!     function, this must be a pointer to a properly populated instance of SortHelper.
  static int wxCALLBACK pathSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data);

  //! Logic for sorting files by the state of a given tag.
  //! 
  //! The tag to sort by must be defined in the SortHelper object the caller supplies by pointer to
  //! this function via the `sort_data` parameter.
  //! 
  //! @param item1 Pointer to user-defined data associated with the first list control item (which
  //!     by convention we've set to be the path for that item).
  //! @param item2 Pointer to user-defined data associated with the second list control item (which
  //!     by convention we've set to be the path for that item).
  //! @param sort_data Additional context used for sorting. As we've chosen to implement this
  //!     function, this must be a pointer to a properly populated instance of SortHelper.
  static int wxCALLBACK tagSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data);

  //! Logic for sorting files by their rating.
  //! 
  //! @param item1 Pointer to user-defined data associated with the first list control item (which
  //!     by convention we've set to be the path for that item).
  //! @param item2 Pointer to user-defined data associated with the second list control item (which
  //!     by convention we've set to be the path for that item).
  //! @param sort_data Additional context used for sorting. As we've chosen to implement this
  //!     function, this must be a pointer to a properly populated instance of SortHelper.
  static int wxCALLBACK ratingSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data);

  //! Tag map used as the ground truth for this window's display of files, tags, etc.
  ragtag::TagMap tag_map_{};

  //! Collection of tags referenced as user data by elements of the tag filter dropdown control.
  //! 
  //! Indices within this vector correspond to equivalent indices within `dd_tag_selection_`.
  std::vector<ragtag::tag_t> tags_{};

  //! Collection of file paths referenced as user data by file listing entries.
  //! 
  //! Indices within this vector correspond to equivalent indices within `lc_summary_`.
  std::vector<ragtag::path_t> file_paths_{};  // Indices to match those of lc_summary_

  // USER INTERFACE ELEMENTS =======================================================================
  //! Slider controlling minimum rating bound for rating filter.
  wxSlider* sl_min_rating_{};
  //! Slider controlling maximum rating bound for rating filter.
  wxSlider* sl_max_rating_{};
  //! Checkbox controlling whether rated files are included in the file listing.
  wxCheckBox* cb_show_rated_{};
  //! Checkbox controlling whether unrated files are included in the file listing.
  wxCheckBox* cb_show_unrated_{};
  //! Dropdown box of tags to use for filtering, establishing the "active filter tag."
  wxComboBox* dd_tag_selection_{};
  //! Checkbox controlling whether files for which the active filter tag is committed as YES should
  //! be included in the file listing.
  wxCheckBox* cb_show_yes_{};
  //! Checkbox controlling whether files for which the active filter tag is committed as NO should
  //! be included in the file listing.
  wxCheckBox* cb_show_no_{};
  //! Checkbox controlling whether files for which the active filter tag is UNCOMMITTED should be
  //! included in the file listing.
  wxCheckBox* cb_show_uncommitted_{};
  //! Checkbox controlling whether files present on disk should be included in the file listing.
  wxCheckBox* cb_show_present_{};
  //! Checkbox controlling whether files not present on disk should be included in the file listing.
  wxCheckBox* cb_show_missing_{};
  //! Text displaying the count of selected files and count of total files in the project.
  wxStaticText* st_filtered_file_count_{};
  //! List control representing the "file listing," containing all project files and the status of
  //! all tags on these files.
  wxListCtrl* lc_summary_{};
  //! Button allowing the user to delete selected files.
  wxButton* b_delete_files_{};
  //! Button allowing the user to remove selected files from the project.
  wxButton* b_remove_from_project_{};
  //! Button allowing the user to copy selected files to a destination on disk.
  wxButton* b_copy_selections_{};
};

// Forward-declare SummaryFrameEvent so that wxWidgets' wxDECLARE_EVENT macro can do what it needs.
class SummaryFrameEvent;
wxDECLARE_EVENT(SUMMARY_FRAME_EVENT, SummaryFrameEvent);

//! Extension of wxCommandEvent that communicates details about the action the user takes within the
//! project summary window.
//! 
//! Approach adapted from https://wiki.wxwidgets.org/Custom_Events#Subclassing_wxCommandEvent.
class SummaryFrameEvent : public wxCommandEvent {
public:
  //! The fundamental action this event describes.
  enum class Action {
    NONE,          //!< No action.
    SELECT_FILE,   //!< The user selects a file.
    REMOVE_FILES,  //!< The user removes files.
    DELETE_FILES   //!< The user deletes files.
  };

  // Implementation note: wxCommandEvent allows communication of a specific ID. Until we have use
  // for that, we will default it to 0.
  //! Constructor.
  //! 
  //! @param paths File paths relevant to the user's action, e.g., the files to delete for events of
  //!     type DELETE_FILES.
  //! @param action The action the user performs. (See Action documentation.)
  SummaryFrameEvent(const std::vector<ragtag::path_t>& paths, Action action)
    : wxCommandEvent(SUMMARY_FRAME_EVENT, 0), paths_(paths), action_(action) {
  }

  //! Copy constructor.
  //!
  //! @param event The event to copy.
  SummaryFrameEvent(const SummaryFrameEvent& event) : wxCommandEvent(event) {
    paths_ = event.paths_;
    action_ = event.action_;
  }

  //! Duplicates this SummaryFrameEvent as a new SummaryFrameEvent instance with identical state.
  //! 
  //! @returns A pointer to a new SummaryFrameEvent instance with state matching the object on which
  //!     this function was invoked.
  wxEvent* Clone() const {
    return new SummaryFrameEvent(*this);
  }

  //! Retrieves the file paths associated with this event.
  //! 
  //! @returns The file paths associated with this event.
  std::vector<ragtag::path_t> getPaths() const {
    return paths_;
  }

  //! Associates file paths with this event.
  //! 
  //! @param paths The paths to associate with this event.
  void setPaths(const std::vector<ragtag::path_t>& paths) {
    paths_ = paths;
  }

  //! Retrieves the user action associated with this event.
  //! 
  //! @returns The user action associated with this event.
  Action getAction() const {
    return action_;
  }

  //! Associates a user action with this event.
  //! 
  //! @param action The action to associate with this event.
  void setAction(Action action) {
    action_ = action;
  }

private:
  //! File paths associated with this event.
  std::vector<ragtag::path_t> paths_{};

  //! User action this event describes.
  Action action_{ Action::NONE };
};

//! Event handler function type as required by custom events in wxWidgets.
typedef void (wxEvtHandler::* SummaryFrameEventFunction)(SummaryFrameEvent&);
#define SummaryFrameEventHandler(func) wxEVENT_HANDLER_CAST(SummaryFrameEventFunction, func)

#endif  // INCLUDE_SUMMARY_FRAME_H
