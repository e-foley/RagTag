#ifndef INCLUDE_SUMMARY_FRAME_H
#define INCLUDE_SUMMARY_FRAME_H

#include "tag_map.h"
#include <optional>
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

private:
  static const int PATH_COLUMN_INDEX;
  static const int RATING_COLUMN_INDEX;
  static const int FIRST_TAG_COLUMN_INDEX;

  struct SortHelper {
    ragtag::TagMap* p_tag_map{ nullptr };
    ragtag::tag_t tag{};
    bool sort_ascending{ false };
  };

  ragtag::TagMap::file_qualifier_t getRuleFromRatingFilterUi();
  ragtag::TagMap::file_qualifier_t getRuleFromTagFilterUi();
  ragtag::TagMap::file_qualifier_t getOverallRuleFromFilterUi();

  void OnRefreshWindow(wxCommandEvent& event);
  void OnClickHeading(wxListEvent& event);
  void OnFileChecked(wxListEvent& event);
  void OnFileUnchecked(wxListEvent& event);
  void OnFilterChangeGeneric(wxCommandEvent& event);
  void OnMinSliderMove(wxCommandEvent& event);
  void OnMaxSliderMove(wxCommandEvent& event);
  void OnClickShowRated(wxCommandEvent& event);
  void OnResetFilters(wxCommandEvent& event);
  void OnSelectAllFiles(wxCommandEvent& event);
  void OnDeselectAllFiles(wxCommandEvent& event);
  void OnCopySelections(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

  std::optional<ragtag::path_t> promptCopyDestination();
  void updateRatingFilterEnabledState();
  void updateCopyButtonTextForSelections();
  void resetFilters();

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
  wxStaticText* st_filtered_file_count_{};
  wxListCtrl* lc_summary_{};
  wxButton* b_copy_selections_{};
};

#endif  // INCLUDE_SUMMARY_FRAME_H
