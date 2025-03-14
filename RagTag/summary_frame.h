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

class SummaryFrame : public wxFrame {
public:
  SummaryFrame(wxWindow* parent);
  void setTagMap(const ragtag::TagMap& tag_map);
  void refreshFileList();
  void refreshTagFilter();
  void highlightFileIfPresent(const ragtag::path_t& path_to_highlight);

private:
  static const int PATH_COLUMN_INDEX;
  static const int RATING_COLUMN_INDEX;
  static const int FIRST_TAG_COLUMN_INDEX;
  static const int PATH_EXTENT_MARGIN_PX;

  struct SortHelper {
    ragtag::TagMap* p_tag_map{ nullptr };
    ragtag::tag_t tag{};
    bool sort_ascending{ false };
  };

  ragtag::TagMap::file_qualifier_t getRuleFromRatingFilterUi();
  ragtag::TagMap::file_qualifier_t getRuleFromTagFilterUi();
  ragtag::TagMap::file_qualifier_t getRuleFromPresenceFilterUi();
  ragtag::TagMap::file_qualifier_t getOverallRuleFromFilterUi();

  void OnClickHeading(wxListEvent& event);
  void OnResizeColumn(wxListEvent& event);
  void OnFileChecked(wxListEvent& event);
  void OnFileUnchecked(wxListEvent& event);
  void OnFileFocused(wxListEvent& event);
  void OnFilterChangeGeneric(wxCommandEvent& event);
  void OnMinSliderMove(wxCommandEvent& event);
  void OnMaxSliderMove(wxCommandEvent& event);
  void OnClickShowRated(wxCommandEvent& event);
  void OnResetFilters(wxCommandEvent& event);
  void OnSelectAllFiles(wxCommandEvent& event);
  void OnDeselectAllFiles(wxCommandEvent& event);
  void OnCopySelections(wxCommandEvent& event);
  void OnDeleteFiles(wxCommandEvent& event);
  void OnRemoveFromProject(wxCommandEvent& event);
  void OnKeyPressed(wxKeyEvent& event);
  void OnClose(wxCloseEvent& event);

  std::optional<ragtag::path_t> promptCopyDestination();
  void updateRatingFilterEnabledState();
  void updateCopyButtonTextForSelections();
  void resetFilters();
  void populateAndEllipsizePathColumn();
  std::vector<ragtag::path_t> getPathsOfSelectedFiles() const;
  std::optional<ragtag::path_t> getPathForItemIndex(int index) const;

  static int wxCALLBACK pathSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data);
  static int wxCALLBACK tagSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data);
  static int wxCALLBACK ratingSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data);

  ragtag::TagMap tag_map_{};

  // Helper caches for linking controls to client data.
  std::vector<ragtag::tag_t> tags_{};  // Indices to match those of dd_tag_selection_
  std::vector<ragtag::path_t> file_paths_{};  // Indices to match those of lc_summary_

  // Dynamic controls
  wxSlider* sl_min_rating_{};
  wxSlider* sl_max_rating_{};
  wxCheckBox* cb_show_rated_{};
  wxCheckBox* cb_show_unrated_{};
  wxComboBox* dd_tag_selection_{};
  wxCheckBox* cb_show_yes_{};
  wxCheckBox* cb_show_no_{};
  wxCheckBox* cb_show_uncommitted_{};
  wxCheckBox* cb_show_present_{};
  wxCheckBox* cb_show_missing_{};
  wxStaticText* st_filtered_file_count_{};
  wxListCtrl* lc_summary_{};
  wxButton* b_delete_files_{};
  wxButton* b_remove_from_project_{};
  wxButton* b_copy_selections_{};
};

class SummaryFrameEvent;
wxDECLARE_EVENT(SUMMARY_FRAME_EVENT, SummaryFrameEvent);

// Extension of wxCommandEvent that communicates info about the user's summary frame action.
// Approach adapted from https://wiki.wxwidgets.org/Custom_Events#Subclassing_wxCommandEvent.
class SummaryFrameEvent : public wxCommandEvent {
public:
  enum class Action {
    NONE,
    SELECT_FILE,
    REMOVE_FILES,
    DELETE_FILES
  };

  // NOTE: wxCommandEvent ctrl allows communication of a specific ID. Until we have use for that, we
  // will default it to 0.
  SummaryFrameEvent(const std::vector<ragtag::path_t>& paths, Action action)
    : wxCommandEvent(SUMMARY_FRAME_EVENT, 0), paths_(paths), action_(action) {
  }

  SummaryFrameEvent(const SummaryFrameEvent& event) : wxCommandEvent(event) {
    paths_ = event.paths_;
    action_ = event.action_;
  }

  wxEvent* Clone() const {
    return new SummaryFrameEvent(*this);
  }

  std::vector<ragtag::path_t> getPaths() const {
    return paths_;
  }

  void setPaths(const std::vector<ragtag::path_t>& path) {
    paths_ = path;
  }

  Action getAction() const {
    return action_;
  }

  void setAction(Action action) {
    action_ = action;
  }

private:
  std::vector<ragtag::path_t> paths_{};
  Action action_{ Action::NONE };
};

typedef void (wxEvtHandler::* SummaryFrameEventFunction)(SummaryFrameEvent&);
#define SummaryFrameEventHandler(func) wxEVENT_HANDLER_CAST(SummaryFrameEventFunction, func)

#endif  // INCLUDE_SUMMARY_FRAME_H
