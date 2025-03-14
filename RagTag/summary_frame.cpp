#include "rag_tag_util.h"
#include "summary_frame.h"
#include <filesystem>
#include <functional>
#include <wx/dcclient.h>
#include <wx/dirdlg.h> 
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbox.h>

wxDEFINE_EVENT(SUMMARY_FRAME_EVENT, SummaryFrameEvent);

const int SummaryFrame::PATH_COLUMN_INDEX = 0;
const int SummaryFrame::RATING_COLUMN_INDEX = 1;
const int SummaryFrame::FIRST_TAG_COLUMN_INDEX = 2;
const int SummaryFrame::PATH_EXTENT_MARGIN_PX = 30;

SummaryFrame::SummaryFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, "Project Summary",
  wxDefaultPosition, wxSize(1280, 768))
{
  wxPanel* p_main = new wxPanel(this, wxID_ANY);
  wxBoxSizer* sz_main = new wxBoxSizer(wxVERTICAL);
  p_main->SetSizer(sz_main);

  wxPanel* p_filters = new wxPanel(p_main, wxID_ANY);
  wxSizer* sz_filters = new wxBoxSizer(wxHORIZONTAL);
  p_filters->SetSizer(sz_filters);
  wxPanel* p_rating_filter = new wxPanel(p_filters, wxID_ANY);
  wxStaticBoxSizer* sz_rating_filter = new wxStaticBoxSizer(wxVERTICAL, p_rating_filter,
    "Rating Filter");
  p_rating_filter->SetSizer(sz_rating_filter);

  wxPanel* p_sliders = new wxPanel(sz_rating_filter->GetStaticBox(), wxID_ANY);
  wxFlexGridSizer* sz_sliders = new wxFlexGridSizer(4);
  p_sliders->SetSizer(sz_sliders);
  wxStaticText* min_rating_label = new wxStaticText(p_sliders, wxID_ANY, "Min:");
  sz_sliders->Add(min_rating_label, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 5);
  sl_min_rating_ = new wxSlider(p_sliders, wxID_ANY, 0, 0, 5,
    wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_VALUE_LABEL);
  sl_min_rating_->Bind(wxEVT_SLIDER, &SummaryFrame::OnMinSliderMove, this);
  sz_sliders->Add(sl_min_rating_, 0, wxEXPAND | wxALL, 5);
  wxStaticText* max_rating_label = new wxStaticText(p_sliders, wxID_ANY, "Max:");
  sz_sliders->Add(max_rating_label, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 5);
  sl_max_rating_ = new wxSlider(p_sliders, wxID_ANY, 5, 0, 5,
    wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_VALUE_LABEL);
  sl_max_rating_->Bind(wxEVT_SLIDER, &SummaryFrame::OnMaxSliderMove, this);
  sz_sliders->Add(sl_max_rating_, 0, wxEXPAND | wxALL, 5);
  sz_rating_filter->Add(p_sliders, 0, wxEXPAND | wxALL, 0);

  cb_show_rated_ = new wxCheckBox(sz_rating_filter->GetStaticBox(), wxID_ANY, "Show rated",
    wxDefaultPosition, wxDefaultSize);
  cb_show_rated_->SetValue(wxCHK_CHECKED);
  cb_show_rated_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnClickShowRated, this);
  sz_rating_filter->Add(cb_show_rated_, 0, wxEXPAND | wxALL, 5);
  cb_show_unrated_ = new wxCheckBox(sz_rating_filter->GetStaticBox(), wxID_ANY, "Show unrated",
    wxDefaultPosition, wxDefaultSize);
  cb_show_unrated_->SetValue(wxCHK_CHECKED);
  cb_show_unrated_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChangeGeneric, this);
  sz_rating_filter->Add(cb_show_unrated_, 0, wxEXPAND | wxALL, 5);
  sz_rating_filter->AddStretchSpacer(1);  // Empty space at bottom to top-align
  sz_filters->Add(p_rating_filter, 0, wxEXPAND | wxALL, 5);

  wxPanel* p_tag_filter = new wxPanel(p_filters, wxID_ANY);
  wxStaticBoxSizer* sz_tag_filter = new wxStaticBoxSizer(wxVERTICAL, p_tag_filter,
    "Tag Filter");
  p_tag_filter->SetSizer(sz_tag_filter);
  wxArrayString options = { "(None)" };
  dd_tag_selection_ = new wxComboBox(sz_tag_filter->GetStaticBox(), wxID_ANY, "(None)",
    wxDefaultPosition, wxDefaultSize, options, wxCB_READONLY | wxCB_DROPDOWN);
  dd_tag_selection_->Bind(wxEVT_COMBOBOX, &SummaryFrame::OnFilterChangeGeneric, this);
  sz_tag_filter->Add(dd_tag_selection_, 0, wxEXPAND | wxALL, 5);
  cb_show_yes_ = new wxCheckBox(sz_tag_filter->GetStaticBox(), wxID_ANY, "Show yes",
    wxDefaultPosition, wxDefaultSize);
  cb_show_yes_->SetValue(wxCHK_CHECKED);
  cb_show_yes_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChangeGeneric, this);
  sz_tag_filter->Add(cb_show_yes_, 0, wxEXPAND | wxALL, 5);
  cb_show_no_ = new wxCheckBox(sz_tag_filter->GetStaticBox(), wxID_ANY, "Show no",
    wxDefaultPosition, wxDefaultSize);
  cb_show_no_->SetValue(wxCHK_UNCHECKED);
  cb_show_no_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChangeGeneric, this);
  sz_tag_filter->Add(cb_show_no_, 0, wxEXPAND | wxALL, 5);
  cb_show_uncommitted_ = new wxCheckBox(sz_tag_filter->GetStaticBox(), wxID_ANY,
    "Show uncommitted", wxDefaultPosition, wxDefaultSize);
  cb_show_uncommitted_->SetValue(wxCHK_UNCHECKED);
  cb_show_uncommitted_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChangeGeneric, this);
  sz_tag_filter->Add(cb_show_uncommitted_, 0, wxEXPAND | wxALL, 5);
  sz_filters->Add(p_tag_filter, 0, wxEXPAND | wxALL, 5);

  wxPanel* p_presence_filter = new wxPanel(p_filters, wxID_ANY);
  wxStaticBoxSizer* sz_presence_filter = new wxStaticBoxSizer(wxVERTICAL, p_presence_filter,
    "Presence Filter");
  p_presence_filter->SetSizer(sz_presence_filter);
  cb_show_present_ = new wxCheckBox(sz_presence_filter->GetStaticBox(), wxID_ANY, "Show present",
    wxDefaultPosition, wxDefaultSize);
  cb_show_present_->SetValue(wxCHK_CHECKED);
  cb_show_present_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChangeGeneric, this);
  sz_presence_filter->Add(cb_show_present_, 0, wxEXPAND | wxALL, 5);
  cb_show_missing_ = new wxCheckBox(sz_presence_filter->GetStaticBox(), wxID_ANY, "Show missing",
    wxDefaultPosition, wxDefaultSize);
  cb_show_missing_->SetValue(wxCHK_CHECKED);
  cb_show_missing_->Bind(wxEVT_CHECKBOX, &SummaryFrame::OnFilterChangeGeneric, this);
  sz_presence_filter->Add(cb_show_missing_, 0, wxEXPAND | wxALL, 5);
  sz_filters->Add(p_presence_filter, 0, wxEXPAND | wxALL, 5);

  sz_filters->AddStretchSpacer(1);
  sz_main->Add(p_filters, 0, wxEXPAND | wxALL, 0);

  wxPanel* p_filter_info = new wxPanel(p_main, wxID_ANY);
  wxBoxSizer* sz_filter_info = new wxBoxSizer(wxHORIZONTAL);
  p_filter_info->SetSizer(sz_filter_info);
  wxButton* b_reset_filters = new wxButton(p_filter_info, wxID_ANY, "Reset Filters");
  b_reset_filters->Bind(wxEVT_BUTTON, &SummaryFrame::OnResetFilters, this);
  sz_filter_info->Add(b_reset_filters, 0, wxEXPAND | wxALL, 5);
  st_filtered_file_count_ = new wxStaticText(p_filter_info, wxID_ANY, wxEmptyString);
  sz_filter_info->Add(st_filtered_file_count_, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxALL , 5);
  sz_main->Add(p_filter_info, 0, wxEXPAND | wxALL, 0);

  lc_summary_ = new wxListCtrl(p_main, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    wxLC_REPORT | wxLC_SINGLE_SEL);
  lc_summary_->EnableCheckBoxes();
  lc_summary_->Bind(wxEVT_LIST_COL_CLICK, &SummaryFrame::OnClickHeading, this);
  lc_summary_->Bind(wxEVT_LIST_ITEM_CHECKED, &SummaryFrame::OnFileChecked, this);
  lc_summary_->Bind(wxEVT_LIST_ITEM_UNCHECKED, &SummaryFrame::OnFileUnchecked, this);
  lc_summary_->Bind(wxEVT_LIST_ITEM_FOCUSED, &SummaryFrame::OnFileFocused, this);
  lc_summary_->Bind(wxEVT_LIST_COL_DRAGGING, &SummaryFrame::OnResizeColumn, this);
  sz_main->Add(lc_summary_, 1, wxEXPAND | wxALL, 5);

  wxPanel* p_summary_buttons = new wxPanel(p_main, wxID_ANY);
  wxBoxSizer* sz_summary_buttons = new wxBoxSizer(wxHORIZONTAL);
  p_summary_buttons->SetSizer(sz_summary_buttons);
  wxButton* b_select_all_files = new wxButton(p_summary_buttons, wxID_ANY, "Select All Files");
  b_select_all_files->Bind(wxEVT_BUTTON, &SummaryFrame::OnSelectAllFiles, this);
  sz_summary_buttons->Add(b_select_all_files, 0, wxALL, 5);
  wxButton* b_deselect_all_files = new wxButton(p_summary_buttons, wxID_ANY, "Deselect All Files");
  b_deselect_all_files->Bind(wxEVT_BUTTON, &SummaryFrame::OnDeselectAllFiles, this);
  sz_summary_buttons->Add(b_deselect_all_files, 0, wxALL, 5);
  sz_summary_buttons->AddStretchSpacer(1);  // Stretch spacer at center to separate button groups
  b_delete_files_ = new wxButton(p_summary_buttons, wxID_ANY, "Delete Selected Files");
  b_delete_files_->Bind(wxEVT_BUTTON, &SummaryFrame::OnDeleteFiles, this);
  sz_summary_buttons->Add(b_delete_files_, 0, wxALL, 5);
  b_remove_from_project_ = new wxButton(p_summary_buttons, wxID_ANY,
    "Remove Selected Files from Project");
  b_remove_from_project_->Bind(wxEVT_BUTTON, &SummaryFrame::OnRemoveFromProject, this);
  sz_summary_buttons->Add(b_remove_from_project_, 0, wxALL, 5);
  b_copy_selections_ = new wxButton(p_summary_buttons, wxID_ANY,
    "Copy Selected Files to Directory...");
  b_copy_selections_->Bind(wxEVT_BUTTON, &SummaryFrame::OnCopySelections, this);
  sz_summary_buttons->Add(b_copy_selections_, 0, wxALL, 5);
  sz_main->Add(p_summary_buttons, 0, wxEXPAND | wxALL, 0);

  Bind(wxEVT_CLOSE_WINDOW, &SummaryFrame::OnClose, this);
  Bind(wxEVT_CHAR_HOOK, &SummaryFrame::OnKeyPressed, this);

  resetFilters();
}

void SummaryFrame::setTagMap(const ragtag::TagMap& tag_map) {
  tag_map_ = tag_map;
}

void SummaryFrame::refreshFileList()
{
  // Cache items that have checkboxes marked so that we can re-check the relevant items after
  // populating the list. We do this as a convenience for our users so that an innocent act like
  // changing one tag on a file doesn't deselect every single file in the project.
  // Note: We do this by path rather than matching text displayed in the control just in case two
  // different paths evaluate to the same wxString (if that's even possible).
  std::vector<ragtag::path_t> previously_checked_items;
  for (int i = 0; i < lc_summary_->GetItemCount(); ++i) {
    if (lc_summary_->IsItemChecked(i)) {
      const auto path = getPathForItemIndex(i);
      if (path.has_value()) {
        previously_checked_items.push_back(*path);
      }
    }
  }

  // Determine whether we need to redraw columns. We do this by seeing whether the tags in the tag
  // map we're tasked with displaying are different from the tags currently displayed in the table.
  // By only re-creating the columns when needed, we preserve any custom width modifications the
  // user has made to them--a nice little quality of life feature.
  bool redraw_columns = false;
  const auto all_tags = tag_map_.getAllTags();
  // +2 accounts for Path and Rating columns.
  if (lc_summary_->GetColumnCount() != all_tags.size() + 2) {
    redraw_columns = true;
  }
  else {
    for (int i = 0; i < all_tags.size(); ++i) {
      bool match = false;
      // Here again, 2 accounts for Path and Rating columns.
      for (int j = 2; j < lc_summary_->GetColumnCount(); ++j) {
        wxListItem column;
        column.SetMask(wxLIST_MASK_TEXT);
        if (!lc_summary_->GetColumn(j, column)) { // `column` is in/out argument
          continue;
        }
        if (all_tags[i].first == column.GetText()) {
          match = true;
          break;
        }
      }
      if (!match) {
        redraw_columns = true;
        break;
      }
    }
  }

  if (redraw_columns) {
    lc_summary_->DeleteAllColumns();
    lc_summary_->AppendColumn("Path", wxLIST_FORMAT_LEFT, 500);
    lc_summary_->AppendColumn("Rating", wxLIST_FORMAT_LEFT, 65);
    Freeze();  // If we don't freeze here, we get flickering.
    for (const auto& tag : all_tags) {
      // This width is temporary. If we use wxLIST_AUTOSIZE_USEHEADER here, then the first column that
      // we add will get stretched to the remaining width of the list control. Instead, we temporarily
      // set the width to a placeholder and fix it right after.
      lc_summary_->AppendColumn(tag.first, wxLIST_FORMAT_CENTER, 0);
    }
    lc_summary_->AppendColumn("");  // Add temp column so that last real column isn't stretched.
    for (int i = 2; i < lc_summary_->GetColumnCount(); ++i) {
      lc_summary_->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
    }
    lc_summary_->DeleteColumn(lc_summary_->GetColumnCount() - 1);  // Delete temporary column.
    Thaw();
  }

  lc_summary_->DeleteAllItems();
  file_paths_ = tag_map_.selectFiles(getOverallRuleFromFilterUi());
  for (int i = 0; i < file_paths_.size(); ++i) {
    // Supply empty string, which will be replaced later by populateAndEllipsizePathColumn().
    lc_summary_->InsertItem(i, wxEmptyString);
     
    // Associate user data with the wxListCtrl item by giving it a pointer--in this case, to the
    // path we've cached within file_paths_. (It's not ideal, but we play along.)
    lc_summary_->SetItemPtrData(i, reinterpret_cast<wxUIntPtr>(&file_paths_[i]));

    // Show rating...
    auto rating = tag_map_.getRating(file_paths_[i]);
    lc_summary_->SetItem(i, 1, rating.has_value() ?
      RagTagUtil::getStarTextForRating(*rating) : wxString("--"));
    // Show state of tags...
    for (int j = 0; j < all_tags.size(); ++j) {  
      wxString tag_state_glyph = wxEmptyString;
      auto tag_setting = tag_map_.getTagSetting(file_paths_[i], all_tags[j].first);
      if (!tag_setting.has_value() || *tag_setting == ragtag::TagSetting::UNCOMMITTED) {
        tag_state_glyph = RagTagUtil::GLYPH_UNCOMMITTED;
      }
      else if (*tag_setting == ragtag::TagSetting::YES) {
        tag_state_glyph = RagTagUtil::GLYPH_CHECKED;
      }
      else if (*tag_setting == ragtag::TagSetting::NO) {
        tag_state_glyph = RagTagUtil::GLYPH_UNCHECKED;
      }

      // Offset edited column because of Path and Rating columns taking indices 0 and 1.
      lc_summary_->SetItem(i, j + FIRST_TAG_COLUMN_INDEX, tag_state_glyph, -1);
    }

    // Check the item if it was previously checked.
    // This is O(m*n) time, but we're dealing with small lists and very fast vector operations.
    for (int j = 0; j < previously_checked_items.size(); ++j) {
      if (file_paths_[i] == previously_checked_items[j]) {
        lc_summary_->CheckItem(i, true);
        break;
      }
    }
  }

  populateAndEllipsizePathColumn();

  st_filtered_file_count_->SetLabel("Current filters: " + std::to_string(file_paths_.size()) + "/" +
    std::to_string(tag_map_.numFiles()) + " project files");
  updateCopyButtonTextForSelections();

  Refresh();
}

void SummaryFrame::refreshTagFilter()
{
  std::optional<ragtag::tag_t> last_tag_selection;
  int last_tag_selection_index = dd_tag_selection_->GetSelection();
  // Exclude 0 index, which always has "(None)" text, and offset by 1 accordingly.
  if (last_tag_selection_index != wxNOT_FOUND && last_tag_selection_index != 0) {
    last_tag_selection = tags_[last_tag_selection_index - 1];
  }

  const auto all_tags = tag_map_.getAllTags();
  tags_.resize(all_tags.size());
  for (int i = 0; i < all_tags.size(); ++i) {
    tags_[i] = all_tags[i].first;
  }

  dd_tag_selection_->Clear();
  dd_tag_selection_->Append("(None)");
  int current_tag_selection_index = 0;
  for (int i = 0; i < tags_.size(); ++i) {
    dd_tag_selection_->Append(tags_[i]);
    if (last_tag_selection.has_value() && tags_[i] == *last_tag_selection) {
      // The tag label matches the label of the tag that was most recently selected, so let's
      // reselect that item as a convenience to the user. (Offset by 1 to acccount for "(None)".)
      current_tag_selection_index = i + 1;
    }
  }
  dd_tag_selection_->SetSelection(current_tag_selection_index);
}

void SummaryFrame::highlightFileIfPresent(const ragtag::path_t& path_to_highlight)
{
  for (int i = 0; i < lc_summary_->GetItemCount(); ++i) {
    const auto path_at_entry = getPathForItemIndex(i);
    if (path_at_entry.has_value() && *path_at_entry == path_to_highlight) {
      lc_summary_->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    }
    else {
      lc_summary_->SetItemState(i, 0, wxLIST_STATE_SELECTED);
    }
  }
}

ragtag::TagMap::file_qualifier_t SummaryFrame::getRuleFromRatingFilterUi()
{
  const int min_rating = sl_min_rating_->GetValue();
  const int max_rating = sl_max_rating_->GetValue();
  const bool show_rated = cb_show_rated_->IsChecked();
  const bool show_unrated = cb_show_unrated_->IsChecked();
  return [=](const ragtag::TagMap::FileInfo& info) {
    if (info.rating.has_value()) {
      return show_rated && *info.rating >= min_rating && *info.rating <= max_rating;
    }
    else {
      return show_unrated;
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

ragtag::TagMap::file_qualifier_t SummaryFrame::getRuleFromPresenceFilterUi()
{
  const bool include_present = cb_show_present_->IsChecked();
  const bool include_missing = cb_show_missing_->IsChecked();
  return [=](const ragtag::TagMap::FileInfo& info) {
    const bool is_file_present = std::filesystem::exists(info.path);
    return include_present && is_file_present || include_missing && !is_file_present;
  };
}

ragtag::TagMap::file_qualifier_t SummaryFrame::getOverallRuleFromFilterUi()
{
  // Composes the rating filter and the tag filter (but can be expanded for other filters as we
  // implement them).
  return [=](const ragtag::TagMap::FileInfo& info) {
    return getRuleFromRatingFilterUi()(info) && getRuleFromTagFilterUi()(info)
      && getRuleFromPresenceFilterUi()(info);
    };
}

void SummaryFrame::OnCopySelections(wxCommandEvent& event)
{
  const std::vector<ragtag::path_t> files_to_copy = getPathsOfSelectedFiles();

  if (files_to_copy.empty()) {
    // Nothing to copy...
    wxMessageDialog dialog(this, "Please select one or more files to copy, then try again.",
      "No Files to Copy");
    dialog.ShowModal();
    return;
  }

  auto directory = promptCopyDestination();
  if (!directory.has_value()) {
    // User canceled dialog; don't do anything.
    return;
  }

  int num_files_successfully_copied = 0;
  for (const auto& file : files_to_copy) {
    // TODO: Somehow prompt whether we'd like to overwrite files.
    bool success = std::filesystem::copy_file(file, *directory / file.filename(),
      std::filesystem::copy_options::skip_existing);
    if (success) {
      ++num_files_successfully_copied;
    }
  }

  if (num_files_successfully_copied == files_to_copy.size()) {
    // TODO: Make custom dialog that allows user to choose whether to show the folder to which the
    // files were copied.
    const std::string file_plural = num_files_successfully_copied == 1 ? "file was" : "files were";
    wxMessageDialog dialog(this, std::to_string(num_files_successfully_copied) + " " + file_plural
      + " copied successfully.", "Copy Success");
    dialog.ShowModal();
  }
  else {
    // TODO: Be more specific about which files were not copied.
    wxMessageDialog dialog(this, "Not all files were copied successfully.\n\n"
      "Perhaps a file is missing or the directory contains a file sharing the name of a file you"
      " are attempting to copy." " Otherwise, there may be a permissions issue.", "Copy Incomplete",
      wxOK | wxCENTER | wxICON_WARNING);
    dialog.ShowModal();
  }
  ShellExecute(NULL, L"open", directory->c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void SummaryFrame::OnDeleteFiles(wxCommandEvent& event)
{
  const std::vector<ragtag::path_t> paths_to_delete = getPathsOfSelectedFiles();
  const std::string path_plural_1 = paths_to_delete.size() == 1 ? "" : "s";
  const std::string path_plural_2 = paths_to_delete.size() == 1 ? "This file" : "These files";
  wxMessageDialog dialog(this, "Are you sure you want to delete "
    + std::to_string(paths_to_delete.size()) + " file" + path_plural_1 + " from your machine?\n\n"
    + path_plural_2 + " will be deleted:\n"
    + wxString(RagTagUtil::getPathsAsNewlineDelineatedString(paths_to_delete)),
    "Confirm File Deletion", wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  const SummaryFrameEvent sending(getPathsOfSelectedFiles(),
    SummaryFrameEvent::Action::DELETE_FILES);
  wxPostEvent(GetParent(), sending);
  event.Skip();
}

void SummaryFrame::OnRemoveFromProject(wxCommandEvent& event)
{
  const std::vector<ragtag::path_t> paths_to_remove = getPathsOfSelectedFiles();
  const std::string path_plural_1 = paths_to_remove.size() == 1 ? "" : "s";
  const std::string path_plural_2 = paths_to_remove.size() == 1 ? "this file" : "these files";
  wxMessageDialog dialog(this, "Are you sure you want to remove "
    + std::to_string(paths_to_remove.size()) + " file" + path_plural_1 + " from this project?\n\n"
    + "All tags will be removed from " + path_plural_2 + ":\n"
    + wxString(RagTagUtil::getPathsAsNewlineDelineatedString(paths_to_remove)),
    "Confirm File Removal", wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING);
  if (dialog.ShowModal() != wxID_OK) {
    return;
  }

  const SummaryFrameEvent sending(getPathsOfSelectedFiles(),
    SummaryFrameEvent::Action::REMOVE_FILES);
  wxPostEvent(GetParent(), sending);
  event.Skip();
}

void SummaryFrame::OnKeyPressed(wxKeyEvent& event)
{
  if (event.GetKeyCode() == WXK_F5) {
    refreshTagFilter();
    refreshFileList();
  } else if (event.GetUnicodeKey() == 'W' && event.GetModifiers() == wxMOD_CONTROL) {
    Close();
  } else {
    event.Skip();
  }
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

void SummaryFrame::OnResizeColumn(wxListEvent& event)
{
  populateAndEllipsizePathColumn();
}

void SummaryFrame::OnFileChecked(wxListEvent& event)
{
  updateCopyButtonTextForSelections();
}

void SummaryFrame::OnFileUnchecked(wxListEvent& event)
{
  updateCopyButtonTextForSelections();
}

void SummaryFrame::OnFileFocused(wxListEvent& event)
{
  const auto path = getPathForItemIndex(event.GetIndex());
  if (!path.has_value()) {
    return;
  }
  std::vector<ragtag::path_t> path_container(1, *path);
  SummaryFrameEvent sending(path_container, SummaryFrameEvent::Action::SELECT_FILE);
  wxPostEvent(GetParent(), sending);
  event.Skip();
}

void SummaryFrame::OnFilterChangeGeneric(wxCommandEvent& event)
{
  refreshFileList();
}

void SummaryFrame::OnMinSliderMove(wxCommandEvent& event) {
  if (sl_min_rating_->GetValue() > sl_max_rating_->GetValue()) {
    sl_max_rating_->SetValue(sl_min_rating_->GetValue());
  }
  refreshFileList();
}

void SummaryFrame::OnMaxSliderMove(wxCommandEvent& event) {
  if (sl_max_rating_->GetValue() < sl_min_rating_->GetValue()) {
    sl_min_rating_->SetValue(sl_max_rating_->GetValue());
  }
  refreshFileList();
}

void SummaryFrame::OnClickShowRated(wxCommandEvent& event)
{
  updateRatingFilterEnabledState();
  refreshFileList();
}

void SummaryFrame::OnResetFilters(wxCommandEvent& event)
{
  resetFilters();
  updateRatingFilterEnabledState();
  refreshFileList();
}

void SummaryFrame::OnSelectAllFiles(wxCommandEvent& event)
{
  for (int i = 0; i < lc_summary_->GetItemCount(); ++i) {
    lc_summary_->CheckItem(i, true);
  }
}

void SummaryFrame::OnDeselectAllFiles(wxCommandEvent& event)
{
  for (int i = 0; i < lc_summary_->GetItemCount(); ++i) {
    lc_summary_->CheckItem(i, false);
  }
}

void SummaryFrame::OnClose(wxCloseEvent& event)
{
  // For convenience of synchronization between the main frame and the summary frame, it's more
  // convenient if we don't flat-out destroy the summary frame when it's closed but merely hide it.
  Hide();
}

std::optional<ragtag::path_t> SummaryFrame::promptCopyDestination()
{
  wxString wx_path = wxDirSelector("Select Directory to Copy To", wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON,
    wxDefaultPosition, this);
  if (wx_path.empty()) {
    // User canceled the dialog.
    return {};
  }

  return ragtag::path_t(wx_path.ToStdWstring());
}

void SummaryFrame::updateRatingFilterEnabledState() {
  if (cb_show_rated_->IsChecked()) {
    sl_min_rating_->Enable();
    sl_max_rating_->Enable();
  }
  else {
    sl_min_rating_->Disable();
    sl_max_rating_->Disable();
  }
}

void SummaryFrame::updateCopyButtonTextForSelections()
{
  const int selected_item_count = getPathsOfSelectedFiles().size();
  if (selected_item_count == 0) {
    b_delete_files_->Disable();
    b_remove_from_project_->Disable();
    b_copy_selections_->Disable();
  }
  else {
    b_delete_files_->Enable();
    b_remove_from_project_->Enable();
    b_copy_selections_->Enable();
  }
}

void SummaryFrame::resetFilters()
{
  sl_min_rating_->SetValue(0);
  sl_max_rating_->SetValue(5);
  cb_show_rated_->SetValue(wxCHK_CHECKED);
  cb_show_unrated_->SetValue(wxCHK_CHECKED);
  dd_tag_selection_->SetSelection(0);
  cb_show_yes_->SetValue(wxCHK_CHECKED);
  cb_show_no_->SetValue(wxCHK_UNCHECKED);
  cb_show_uncommitted_->SetValue(wxCHK_UNCHECKED);
  cb_show_present_->SetValue(wxCHK_CHECKED);
  cb_show_missing_->SetValue(wxCHK_CHECKED);
}

void SummaryFrame::populateAndEllipsizePathColumn()
{
  for (int i = 0; i < lc_summary_->GetItemCount(); ++i) {
    const auto path = getPathForItemIndex(i);
    if (!path.has_value()) {
      // Really shouldn't happen.
      continue;
    }

    std::wstring path_displayed = path->generic_wstring();
    if (!std::filesystem::exists(*path)) {
      path_displayed.append(L" [???]");
    }

    const auto ellipsized = lc_summary_->Ellipsize(path_displayed, wxWindowDC(lc_summary_),
      wxELLIPSIZE_START, lc_summary_->GetColumnWidth(PATH_COLUMN_INDEX) - PATH_EXTENT_MARGIN_PX);
    lc_summary_->SetItem(i, PATH_COLUMN_INDEX, ellipsized);
  }
}

std::vector<ragtag::path_t> SummaryFrame::getPathsOfSelectedFiles() const
{
  std::vector<ragtag::path_t> returning;
  for (int i = 0; i < lc_summary_->GetItemCount(); ++i) {
    if (lc_summary_->IsItemChecked(i)) {
      const auto path = getPathForItemIndex(i);
      if (path.has_value()) {
        returning.push_back(*path);
      }
    }
  }
  return returning;
}

std::optional<ragtag::path_t> SummaryFrame::getPathForItemIndex(int index) const
{
  // This isn't as simple as invoking file_paths_[i], since list control indices shift around during
  // sorting operations. Thankfully, the item data (where we placed a pointer to the actual path)
  // moves along with the item.
  wxUIntPtr p_data_nominal = lc_summary_->GetItemData(index);
  if (p_data_nominal == 0) {
    return {};
  }
  
  return *reinterpret_cast<ragtag::path_t*>(p_data_nominal);
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
