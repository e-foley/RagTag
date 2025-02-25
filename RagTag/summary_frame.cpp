#include "summary_frame.h"
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

// The font used by wxWidgets does not display half-star characters as of writing.
// #define HALF_STAR_GLYPH_SUPPORTED

const wxString SummaryFrame::GLYPH_CHECKED = L"\U00002611";  // U+2611 is a checkmark in a box.
const wxString SummaryFrame::GLYPH_UNCOMMITTED = L"\U00002012";  // U+2012 is a figure dash.
const wxString SummaryFrame::GLYPH_UNCHECKED = L"\U00002610";  // U+2610 is an empty checkbox.
const wxString SummaryFrame::GLYPH_RATING_FULL_STAR = L"\U00002605";  // U+2605 is a full star.
#ifdef HALF_STAR_GLYPH_SUPPORTED
const wxString SummaryFrame::GLYPH_RATING_HALF_STAR = L"\U00002BE8";  // U+2BE8 is a half star.
#else
const wxString SummaryFrame::GLYPH_RATING_HALF_STAR = L"\U000000BD";  // U+00BD is a half fraction.
#endif
const int SummaryFrame::MAX_STARS = 5;
const int SummaryFrame::PATH_COLUMN_INDEX = 0;
const int SummaryFrame::RATING_COLUMN_INDEX = 1;
const int SummaryFrame::FIRST_TAG_COLUMN_INDEX = 2;

SummaryFrame::SummaryFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, "Project Summary",
  wxDefaultPosition, wxSize(1280, 768))
{
  CreateStatusBar();

  wxPanel* p_main = new wxPanel(this, wxID_ANY);
  wxBoxSizer* sz_main = new wxBoxSizer(wxVERTICAL);
  p_main->SetSizer(sz_main);

  wxPanel* p_filters = new wxPanel(p_main, wxID_ANY);
  wxSizer* sz_filters = new wxBoxSizer(wxHORIZONTAL);
  p_filters->SetSizer(sz_filters);
  wxPanel* p_rating_filter = new wxPanel(p_filters, wxID_ANY);
  // Note: When using StaticBoxSizer, items added to the panel actually need the sizer's StaticBox
  // to be their parent instead of the panel like other sizers would have. I don't know exactly why
  // this is, but wxWidgets is consistent at advising us to do this, and who are we to question it?
  wxStaticBoxSizer* sz_rating_filter = new wxStaticBoxSizer(wxVERTICAL, p_rating_filter,
    "Rating Filter");
  p_rating_filter->SetSizer(sz_rating_filter);

  wxPanel* p_sliders = new wxPanel(sz_rating_filter->GetStaticBox(), wxID_ANY);
  wxFlexGridSizer* sz_sliders = new wxFlexGridSizer(2);
  p_sliders->SetSizer(sz_sliders);
  wxStaticText* min_rating_label = new wxStaticText(p_sliders, wxID_ANY, "Min:");
  sz_sliders->Add(min_rating_label, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 5);
  sl_min_rating_ = new wxSlider(p_sliders, wxID_ANY, 0, 0, 5,
    wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_VALUE_LABEL);
  sl_min_rating_->Bind(wxEVT_SLIDER, &SummaryFrame::OnFilterChange, this);
  sz_sliders->Add(sl_min_rating_, 0, wxEXPAND | wxALL, 5);
  wxStaticText* max_rating_label = new wxStaticText(p_sliders, wxID_ANY, "Max:");
  sz_sliders->Add(max_rating_label, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 5);
  sl_max_rating_ = new wxSlider(p_sliders, wxID_ANY, 5, 0, 5,
    wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_VALUE_LABEL);
  sl_max_rating_->Bind(wxEVT_SLIDER, &SummaryFrame::OnFilterChange, this);
  sz_sliders->Add(sl_max_rating_, 0, wxEXPAND | wxALL, 5);
  sz_rating_filter->Add(p_sliders, 0, wxEXPAND | wxALL, 0);

  cb_include_unrated_ = new wxCheckBox(sz_rating_filter->GetStaticBox(), wxID_ANY,
    "Include unrated", wxDefaultPosition, wxDefaultSize);
  cb_include_unrated_->SetValue(wxCHK_CHECKED);
  cb_include_unrated_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChange, this);
  sz_rating_filter->Add(cb_include_unrated_, 0, wxEXPAND | wxALL, 5);
  sz_rating_filter->AddStretchSpacer(1);  // Empty space at bottom to top-align
  sz_filters->Add(p_rating_filter, 0, wxEXPAND | wxALL, 5);

  wxPanel* p_tag_filter = new wxPanel(p_filters, wxID_ANY);
  wxStaticBoxSizer* sz_tag_filter = new wxStaticBoxSizer(wxVERTICAL, p_tag_filter,
    "Tag Filter");
  p_tag_filter->SetSizer(sz_tag_filter);
  wxArrayString options = { "[No filter]" };
  dd_tag_selection_ = new wxComboBox(sz_tag_filter->GetStaticBox(), wxID_ANY, "[No filter]",
    wxDefaultPosition, wxDefaultSize, options, wxCB_READONLY | wxCB_DROPDOWN);
  dd_tag_selection_->Bind(wxEVT_COMBOBOX, &SummaryFrame::OnFilterChange, this);
  sz_tag_filter->Add(dd_tag_selection_, 0, wxEXPAND | wxALL, 5);
  cb_show_yes_ = new wxCheckBox(sz_tag_filter->GetStaticBox(), wxID_ANY, "Show yes",
    wxDefaultPosition, wxDefaultSize);
  cb_show_yes_->SetValue(wxCHK_CHECKED);
  cb_show_yes_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChange, this);
  sz_tag_filter->Add(cb_show_yes_, 0, wxEXPAND | wxALL, 5);
  cb_show_no_ = new wxCheckBox(sz_tag_filter->GetStaticBox(), wxID_ANY, "Show no",
    wxDefaultPosition, wxDefaultSize);
  cb_show_no_->SetValue(wxCHK_CHECKED);
  cb_show_no_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChange, this);
  sz_tag_filter->Add(cb_show_no_, 0, wxEXPAND | wxALL, 5);
  cb_show_uncommitted_ = new wxCheckBox(sz_tag_filter->GetStaticBox(), wxID_ANY,
    "Show uncommitted", wxDefaultPosition, wxDefaultSize);
  cb_show_uncommitted_->SetValue(wxCHK_CHECKED);
  cb_show_uncommitted_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChange, this);
  sz_tag_filter->Add(cb_show_uncommitted_, 0, wxEXPAND | wxALL, 5);

  sz_filters->Add(p_tag_filter, 0, wxEXPAND | wxALL, 5);
  sz_filters->AddStretchSpacer(1);
  sz_main->Add(p_filters, 0, wxEXPAND | wxALL, 0);

  lc_summary_ = new wxListCtrl(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL);
  lc_summary_->Bind(wxEVT_LIST_COL_CLICK, &SummaryFrame::OnClickHeading, this);
  sz_main->Add(lc_summary_, 1, wxEXPAND | wxALL, 5);

  wxPanel* p_summary_buttons = new wxPanel(p_main, wxID_ANY);
  wxBoxSizer* sz_summary_buttons = new wxBoxSizer(wxHORIZONTAL);
  p_summary_buttons->SetSizer(sz_summary_buttons);
  sz_summary_buttons->AddStretchSpacer(1);  // Stretch spacer at left to right-align buttons
  wxButton* b_refresh_window = new wxButton(p_summary_buttons, wxID_ANY, "Refresh");
  b_refresh_window->Bind(wxEVT_BUTTON, &SummaryFrame::OnRefreshWindow, this);
  sz_summary_buttons->Add(b_refresh_window, 0, wxALL, 5);
  wxButton* b_copy_selections = new wxButton(p_summary_buttons, wxID_ANY,
    "Copy Selected Files to Directory...");
  b_copy_selections->Bind(wxEVT_BUTTON, &SummaryFrame::OnCopySelections, this);
  sz_summary_buttons->Add(b_copy_selections, 0, wxALL, 5);
  sz_main->Add(p_summary_buttons, 0, wxEXPAND | wxALL, 0);
}

void SummaryFrame::setTagMap(const ragtag::TagMap& tag_map) {
  tag_map_ = tag_map;
}

void SummaryFrame::refreshFileList()
{
  lc_summary_->ClearAll();
  lc_summary_->AppendColumn("Path", wxLIST_FORMAT_LEFT, 500);
  lc_summary_->AppendColumn("Rating", wxLIST_FORMAT_LEFT, 65);
  const auto all_tags = tag_map_.getAllTags();
  for (const auto& tag : all_tags) {
    // TODO: Figure out a way to get autosizing to play nicely upon refresh.
    lc_summary_->AppendColumn(tag.first, wxLIST_FORMAT_CENTER, /*wxLIST_AUTOSIZE_USEHEADER*/ 80);
  }
  file_paths_ = tag_map_.selectFiles(getOverallRuleFromFilterUi());
  for (int i = 0; i < file_paths_.size(); ++i) {
    lc_summary_->InsertItem(i, file_paths_[i].generic_wstring());
     
    // Associate user data with the wxListCtrl item by giving it a pointer--in this case, to the
    // path we've cached within file_paths_. (It's not ideal, but we play along.)
    lc_summary_->SetItemPtrData(i, reinterpret_cast<wxUIntPtr>(&file_paths_[i]));

    // Show rating...
    auto rating = tag_map_.getRating(file_paths_[i]);
    lc_summary_->SetItem(i, 1, rating.has_value() ? getStarTextForRating(*rating) : wxString("--"));
    // Show state of tags...
    for (int j = 0; j < all_tags.size(); ++j) {  
      wxString tag_state_glyph = wxEmptyString;
      auto tag_setting = tag_map_.getTagSetting(file_paths_[i], all_tags[j].first);
      if (!tag_setting.has_value() || *tag_setting == ragtag::TagSetting::UNCOMMITTED) {
        tag_state_glyph = GLYPH_UNCOMMITTED;
      }
      else if (*tag_setting == ragtag::TagSetting::YES) {
        tag_state_glyph = GLYPH_CHECKED;
      }
      else if (*tag_setting == ragtag::TagSetting::NO) {
        tag_state_glyph = GLYPH_UNCHECKED;
      }

      // Offset edited column because of Path and Rating columns taking indices 0 and 1.
      lc_summary_->SetItem(i, j + FIRST_TAG_COLUMN_INDEX, tag_state_glyph, -1);
    }
  }
}

void SummaryFrame::refreshTagFilter()
{
  std::optional<ragtag::tag_t> last_tag_selection;
  int last_tag_selection_index = dd_tag_selection_->GetSelection();
  // Exclude 0 index, which always has "[No filter]" text, and offset by 1 accordingly.
  if (last_tag_selection_index != wxNOT_FOUND && last_tag_selection_index != 0) {
    last_tag_selection = tags_[last_tag_selection_index - 1];
  }

  const auto all_tags = tag_map_.getAllTags();
  tags_.resize(all_tags.size());
  for (int i = 0; i < all_tags.size(); ++i) {
    tags_[i] = all_tags[i].first;
  }

  dd_tag_selection_->Clear();
  dd_tag_selection_->Append("[No filter]");
  int current_tag_selection_index = 0;
  for (int i = 0; i < tags_.size(); ++i) {
    dd_tag_selection_->Append(tags_[i]);
    if (last_tag_selection.has_value() && tags_[i] == *last_tag_selection) {
      // The tag label matches the label of the tag that was most recently selected, so let's
      // reselect that item as a convenience to the user. (Offset by 1 to acccount for [No filter].)
      current_tag_selection_index = i + 1;
    }
  }
  dd_tag_selection_->SetSelection(current_tag_selection_index);
}

ragtag::TagMap::file_qualifier_t SummaryFrame::getRuleFromRatingFilterUi()
{
  const int min_rating = sl_min_rating_->GetValue();
  const int max_rating = sl_max_rating_->GetValue();
  const bool include_unrated = cb_include_unrated_->IsChecked();
  return [=](const ragtag::TagMap::FileInfo& info) {
    if (info.rating.has_value()) {
      return *info.rating >= min_rating && *info.rating <= max_rating;
    }
    else {
      return include_unrated;
    }
    };
}

ragtag::TagMap::file_qualifier_t SummaryFrame::getRuleFromTagFilterUi()
{
  const int selection_index = dd_tag_selection_->GetSelection();
  if (selection_index == 0) {
    // The selected item is the one representing no filter, so include all the files.
    return [](const ragtag::TagMap::FileInfo& info) {return true;};
  }

  // -1 accounts for first option being the default "no filter" option, which isn't tied to a tag.
  const ragtag::tag_t tag = tags_[dd_tag_selection_->GetSelection() - 1];
  const bool include_yes = cb_show_yes_->IsChecked();
  const bool include_no = cb_show_no_->IsChecked();
  const bool include_uncommitted = cb_show_uncommitted_->IsChecked();
  return [=](const ragtag::TagMap::FileInfo& info) {
    const ragtag::TagSetting setting = info.f_tag_setting(tag);
    return setting == ragtag::TagSetting::YES && include_yes ||
      setting == ragtag::TagSetting::NO && include_no ||
      setting == ragtag::TagSetting::UNCOMMITTED && include_uncommitted;
    };
}

ragtag::TagMap::file_qualifier_t SummaryFrame::getOverallRuleFromFilterUi()
{
  // Composes the rating filter and the tag filter (but can be expanded for other filters as we
  // implement them).
  return [=](const ragtag::TagMap::FileInfo& info) {
    return getRuleFromRatingFilterUi()(info) && getRuleFromTagFilterUi()(info);
    };
}

void SummaryFrame::OnRefreshWindow(wxCommandEvent& event)
{
  refreshTagFilter();
  refreshFileList();
}

void SummaryFrame::OnCopySelections(wxCommandEvent& event)
{
}

void SummaryFrame::OnClickHeading(wxListEvent& event)
{
  const int column = event.GetColumn();
  if (column == -1) {
    // User clicked the header bar away from a column. Ignore.
    return;
  }

  // This is a pain, but because wxListCtrl's sort operation requires a free-floating function
  // (not a member function that has access to object internals), we have to manually supply it a
  // snapshot of the information we would like to sort. We do this by sending it a pointer to an
  // instance of a helper struct we populate with information the sorting function requires.
  SortHelper sort_helper;
  sort_helper.p_tag_map = &tag_map_;

  // GetSortIndicator() returns the column in which the current sort indicator is shown, or -1.
  // When a new column is clicked, we prefer to sort descending first (except for text), which
  // is the opposite of the default behavior.
  bool ascending = column == PATH_COLUMN_INDEX;
  if (lc_summary_->GetSortIndicator() == column) {
    // Same column is re-clicked.
    ascending = lc_summary_->GetUpdatedAscendingSortIndicator(column);
  }
  sort_helper.sort_ascending = ascending;

  if (column == PATH_COLUMN_INDEX) {
    lc_summary_->SortItems(&SummaryFrame::pathSort, reinterpret_cast<wxIntPtr>(&sort_helper));
  }
  else if (column == RATING_COLUMN_INDEX) {
    lc_summary_->SortItems(&SummaryFrame::ratingSort, reinterpret_cast<wxIntPtr>(&sort_helper));
  }
  else if (column >= FIRST_TAG_COLUMN_INDEX) {
    // TODO: Consider caching all tags when refreshing so we don't need to re-procure the list here.
    const auto all_tags = tag_map_.getAllTags();
    sort_helper.tag = all_tags[column - FIRST_TAG_COLUMN_INDEX].first;
    lc_summary_->SortItems(&SummaryFrame::tagSort, reinterpret_cast<wxIntPtr>(&sort_helper));
  }

  lc_summary_->ShowSortIndicator(column, ascending);
}

void SummaryFrame::OnFilterChange(wxCommandEvent& event)
{
  refreshFileList();
}

wxString SummaryFrame::getStarTextForRating(float rating)
{
  wxString returning = wxEmptyString;
  for (int i = 1; i <= MAX_STARS; ++i) {
    if (rating >= static_cast<float>(i)) {
      returning.Append(GLYPH_RATING_FULL_STAR);
    }
    else if (rating >= static_cast<float>(i) - 0.5f) {
      returning.Append(GLYPH_RATING_HALF_STAR);
    }
  }

  return returning;
}

int wxCALLBACK SummaryFrame::pathSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data)
{
  // It's understood that the items to sort are paths and that `sort_data` is a pointer to our
  // sorting helper struct.
  const ragtag::path_t path1 = *reinterpret_cast<ragtag::path_t*>(item1);
  const ragtag::path_t path2 = *reinterpret_cast<ragtag::path_t*>(item2);
  const SortHelper sort_helper = *reinterpret_cast<SortHelper*>(sort_data);
  return sort_helper.sort_ascending ? path1.compare(path2) : -path1.compare(path2);
}

int wxCALLBACK SummaryFrame::tagSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data)
{
  // It's understood that the items to sort are paths and that `sort_data` is a pointer to our
  // sorting helper struct.
  const ragtag::path_t path1 = *reinterpret_cast<ragtag::path_t*>(item1);
  const ragtag::path_t path2 = *reinterpret_cast<ragtag::path_t*>(item2);
  const SortHelper sort_helper = *reinterpret_cast<SortHelper*>(sort_data);
  const ragtag::TagMap& tag_map = *sort_helper.p_tag_map;  // Alias for convenience
  auto setting1 = tag_map.getTagSetting(path1, sort_helper.tag);
  auto setting2 = tag_map.getTagSetting(path2, sort_helper.tag);

  if (!setting1.has_value()) {
    setting1 = ragtag::TagSetting::UNCOMMITTED;
  }
  if (!setting2.has_value()) {
    setting2 = ragtag::TagSetting::UNCOMMITTED;
  }

  int natural_sort = 0;
  if (*setting1 == *setting2) {
    natural_sort = 0;
  }
  else if (*setting1 == ragtag::TagSetting::YES
    || (*setting1 == ragtag::TagSetting::UNCOMMITTED && *setting2 == ragtag::TagSetting::NO)) {
    natural_sort = 1;
  }
  else {
    natural_sort = -1;
  }

  return sort_helper.sort_ascending ? natural_sort : -natural_sort;
}

int wxCALLBACK SummaryFrame::ratingSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr sort_data)
{
  const ragtag::path_t path1 = *reinterpret_cast<ragtag::path_t*>(item1);
  const ragtag::path_t path2 = *reinterpret_cast<ragtag::path_t*>(item2);
  const SortHelper sort_helper = *reinterpret_cast<SortHelper*>(sort_data);
  const ragtag::TagMap& tag_map = *sort_helper.p_tag_map;  // Alias for convenience
  auto rating1 = tag_map.getRating(path1);
  auto rating2 = tag_map.getRating(path2);
  
  int natural_sort = 0;
  if (!rating1.has_value() && !rating2.has_value()) {
    natural_sort = 0;
  }
  else if (rating1.has_value() && !rating2.has_value()) {
    natural_sort = 1;
  }
  else if (!rating1.has_value() && rating2.has_value()) {
    natural_sort = -1;
  }
  else {
    natural_sort = *rating1 > *rating2 ? 1 :
                   *rating1 < *rating2 ? -1 : 0;
  }

  return sort_helper.sort_ascending ? natural_sort : -natural_sort;
}
