#ifndef INCLUDE_MAIN_FRAME_H
#define INCLUDE_MAIN_FRAME_H

#include "summary_frame.h"
#include "tag_map.h"
#include "tag_toggle_panel.h"
#include <filesystem>
#include <optional>
#include <wx/checkbox.h>
#include <wx/colour.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/mediactrl.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/tglbtn.h>

//! Primary UI window for the RagTag application that provides the user essential controls for their
//! project, such as viewing media, assigning ratings, and attaching tags.
//! 
//! An important concept is "Command Mode," which allows the user to assign/remove tags and navigate
//! the current directory with minimal modifier key usage and without a mouse.
class MainFrame : public wxFrame {
public:
  //! Constructor.
  MainFrame();

private:
  //! Type of function that enforces criteria for including a file path within grouping contexts.
  typedef std::function<bool(const ragtag::path_t&)> file_qualifier_t;

  //! Enumeration of IDs used to differentiate various in-window controls for the purposes of
  //! assigning functions to them.
  // Implementation note: IDs of 0 and 1 are not allowed per wxWidgets documentation:
  // https://docs.wxwidgets.org/latest/overview_windowids.html
  enum {
    ID_NONE = 2,
    ID_NEW_PROJECT,
    ID_OPEN_PROJECT,
    ID_SAVE_PROJECT,
    ID_SAVE_PROJECT_AS,
    ID_LOAD_FILE,
    ID_ENTER_COMMAND_MODE,
    ID_FOCUS_DIRECTORY_VIEW,
    ID_FOCUS_TAGS,
    ID_REFRESH_FILE_VIEW,
    ID_NEXT_FILE,
    ID_PREVIOUS_FILE,
    ID_NEXT_UNTAGGED_FILE,
    ID_PREVIOUS_UNTAGGED_FILE,
    ID_SHOW_SUMMARY,
    ID_CLEAR_TAGS_FROM_FILE,
    ID_SET_TAGS_TO_DEFAULTS,
    ID_DEFINE_NEW_TAG,
    ID_MEDIA_CTRL,
    ID_STOP_MEDIA,
    ID_PLAY_PAUSE_MEDIA,
    ID_TOGGLE_AUTOPLAY,
    ID_TOGGLE_LOOPING,
    ID_TOGGLE_MUTE,
    ID_NO_RATING,
    ID_RATING_0,
    ID_RATING_MAX = ID_RATING_0 + 5  // Implied ratings 1-5
  };

  //! User's intended action based on their response to a dialog.
  enum class UserIntention {
    NONE = 0,
    SAVE,
    DONT_SAVE,
    CANCEL,
  };

  //! Columns displayed within the directory viewer.
  enum FileViewColumn {
    COLUMN_FILENAME,
    COLUMN_TAG_COVERAGE,
    COLUMN_RATING
  };

  //! Color used for the background of list items for files that have been fully tagged.
  static const wxColour BACKGROUND_COLOR_FULLY_TAGGED;

  //! Color used for the background of list items for files that have been partly tagged (files that
  //! have some tag coverage).
  static const wxColour BACKGROUND_COLOR_PARTLY_TAGGED;

  //! Color used for the background of list items for files that are untagged (files that have no
  //! tag coverage).
  static const wxColour BACKGROUND_COLOR_FULLY_UNTAGGED;

  //! Starting position for the vertical divider as a proportion of the window width.
  static const double LEFT_PANE_STARTING_PROPORTION;

  //! Minimum position for the vertical divider as a proportion of the window width.
  static const double LEFT_PANE_MINIMUM_PROPORTION;

  //! Horizontal adjustment of the vertical divider as a proportion of window width changes.
  static const double LEFT_PANE_GRAVITY;

  //! Starting position of the media/directory divider as a proportion of the window height.
  static const double MEDIA_PANE_STARTING_PROPORTION;

  //! Minimum position of the media/directory divider as a proportion of the window height.
  static const double MEDIA_PANE_MINIMUM_PROPORTION;

  //! Vertical adjustment of the media/directory divider as a proportion of window height changes.
  static const double MEDIA_PANE_GRAVITY;

  // FUNCTIONS UPDATING VIEW TO MATCH MODEL ========================================================
  //! Immediately update the visual display of tag toggles to match the model, including adding or
  //! removing entries and enabling or disabling checkboxes.
  void refreshTagToggles();

  //! Immediately update the display of files within the directory to match the model.
  void refreshFileView();

  //! Immediately update the display of buttons used for rating files to match the model,
  //! potentially enabling or disabling them.
  void refreshRatingButtons();

  //! Immediately update the project summary window to match the model.
  void refreshSummary();

  //! Immediately update the window's title bar to display the path of the current project and
  //! denote whether the file has been modified.
  void refreshTitleBar();

  //! Immediately update the window's status bar to display active mode (e.g., command mode) and
  //! whether the project has been modified.
  void refreshStatusBar();

  // PROMPTS =======================================================================================
  //! Prompts the user to save their work.
  //! 
  //! This function does not perform any saving action; it just collects the user's intention.
  //! Returns the user to Command Mode afterward if they were in it prior to the prompt.
  //! 
  //! @returns The response indicated by the user.
  UserIntention promptUnsavedChanges();

  //! Prompts the user to select a path at which to save the project.
  //! 
  //! This function does not actually write the file; it just collects the path. Returns the user
  //! to Command Mode if they were in it prior to the prompt.
  //! 
  //! @returns The path the user indicates or an empty optional if the user cancels the prompt.
  std::optional<ragtag::path_t> promptSaveProjectAs();

  //! Prompts the user to select a path of a RagTag-compatible project on disk.
  //! 
  //! Valid files are either a RagTag project or a RagTag project backup. This function does not
  //! actually load the file; it just collects the path. Returns the user to Command Mode if they
  //! were in it prior to the prompt.
  //! 
  //! @returns The project path indicated by the user or an empty optional if the user cancels the
  //! prompt.
  std::optional<ragtag::path_t> promptOpenProject();

  //! Prompts the user to select the path of a file to add to their project.
  //! 
  //! No restriction is placed on the type of file the user selects. This function does not actually
  //! load the file; it just collects the path. Returns the user to Command Mode if they were in it
  //! prior to the prompt.
  //! 
  //! @returns The file path indicated by the user or an empty optional if the user cancels the
  //! prompt.
  std::optional<ragtag::path_t> promptLoadFile();

  //! Prompts the user to confirm that they wish to delete a tag.
  //! 
  //! This function does not actually delete the tag; it just collects the user's intent. Returns
  //! the user to Command Mode if they were in it prior to the prompt.
  //! 
  //! @param tag The tag to confirm the user's intent to delete.
  //! @returns True if the user confirms their intent to delete the tag.
  bool promptConfirmTagDeletion(ragtag::tag_t tag);

  //! Prompts the user to confirm they wish to delete a file.
  //! 
  //! This function does not actually delete the file; it just collects the user's intent. Returns
  //! the user to Command Mode if they were in it prior to the prompt.
  //! 
  //! @param path The path of the file to confirm the user's intent to delete.
  //! @returns True if the user confirms their intent to delete the file.
  bool promptConfirmFileDeletion(const ragtag::path_t& path);

  //! Prompts the user to save their work if the project has been marked dirty and saves the project
  //! (possibly through the use of other prompts) if the user indicates a desire to save. If the
  //! project is not dirty, does nothing.
  //! 
  //! Notifies the user if the saving operation fails. Returns the user to Command Mode if they were
  //! in it prior to the prompt.
  //! 
  //! @returns True if the project is clean or the action corresponding to the user's intent (saving
  //! or not saving) is performed; false if the user cancels the prompt or a desired saving operation
  //! fails. (Essentially, true denotes that everything related to saving is wrapped up and false
  //! denotes that there is still unfinished business.)
  bool promptSaveOpportunityIfDirty();

  //! Notifies the user that the project could not be saved.
  //! 
  //! Returns the user to Command Mode if they were in it prior to the prompt.
  //! 
  //! @param path The path of the project that could not be saved.
  void notifyCouldNotSaveProject(const ragtag::path_t& path);

  //! Notifies the user that a project could not be opened.
  //! 
  //! Returns the user to Command Mode if they were in it prior to the prompt.
  //! 
  //! @param path The path of the project that could not be opened.
  void notifyCouldNotOpenProject(const ragtag::path_t& path);

  // FUNDAMENTAL LOW-LEVEL PROJECT COMMANDS ========================================================
  //! Marks the project as dirty and updates user interface elements to this effect.
  void markDirty();

  //! Marks the project as clean and updates user interface elements to this effect.
  void markClean();

  //! Creates a new project and loads it in place of any actively loaded project.
  void newProject();

  //! Attempts to saves the active project and a backup copy.
  //!
  //! @returns True if the project and its backup are saved successfully; false if either saving
  //! operation fails.
  bool saveProject();

  //! Attempts to save the active project and a backup copy at a given path.
  //! 
  //! The name of the backup copy is generated from the supplied path.
  //! 
  //! @param path The filename to use for the project.
  //! @returns True if the project and its backup are saved successfully; false if either saving
  //! operation fails.
  bool saveProjectAs(const ragtag::path_t& path);

  //! Loads a file, adds it to the active project if necessary, attempts to display it, and updates
  //! all user controls accordingly. Also establishes the file's directory as the active directory.
  //! 
  //! Displays the media file as if by displayMediaFile() and enables media controls based on the
  //! filetype inferred from the file's extension.
  //! 
  //! If the file is new to the active project, assigns default tags to it.
  //! 
  //! Enables all directory navigation controls if the file is loaded.
  //! 
  //! Refreshes other user interface elements by refreshTagToggles(), refreshFileView(), 
  //! refreshRatingButtons(), and refreshSummary().
  //! 
  //! Highlights the file in the summary window's file listing if the file is present there.
  //! 
  //! If the path provided matches the currently loaded file, this function does nothing.
  //! 
  //! @param path The path of the file to load and make active.
  //! @returns True if the file is successfully loaded and incorporated into the project or if the
  //! provided path matches the presently loaded file; false if the file cannot be loaded, it cannot
  //! be added to the project (if necessary) or default tags cannot be asserted for the file (if
  //! necessary).
  bool loadFileAndSetAsActive(const ragtag::path_t& path);

  //! Clears the actively loaded file and returns user interface elements to their default state.
  void resetActiveFile();

  //! Attempts to open a project from disk and updates user interface elements if successful.
  //! 
  //! The loaded project will be marked clean by default (see markClean()).
  //! 
  //! @param path The path of the project to open.
  //! @returns True if the project is successfully opened.
  bool openProject(const ragtag::path_t& path);

  //! Loads a file into the media control.
  //! 
  //! Note that the act of loading typically invokes the OnMediaLoaded event.
  //! 
  //! @param path The path of the file to display.
  //! @return True if the file is loaded by the media control.
  bool displayMediaFile(const ragtag::path_t& path);

  //! Attempts to play the file currently loaded in the media control.
  //! 
  //! @returns True if the internal call to wxMediaCtrl::Play() is successful.
  bool playMedia();

  //! Attempts to pause the file currently loaded in the media control.
  //! 
  //! @returns True if the internal call to wxMediaCtrl::Pause() is successful.
  bool pauseMedia();

  //! Attempts to stop the file currently loaded in the media control.
  //! 
  //! @returns True if the internal call to wxMediaCtrl::Stop() is successful.
  bool stopMedia();

  //! Attempts to remove the rating from the active file and update user interface elements
  //! accordingly.
  //! 
  //! @returns True if the rating is removed from the active file within the active project.
  bool clearRatingOfActiveFile();

  //! Attemps to set the rating on the active file and update user interface elements accordingly.
  //! 
  //! @param rating The rating to assign the file.
  //! @returns True if the rating is set on the active file within the active project.
  bool setRatingOfActiveFile(ragtag::rating_t rating);

  //! Attempts to load the file following the currently loaded file in the directory listing, then
  //! display it and set it as the new active file.
  //! 
  //! @returns True if the next file can be determined and that file is successfully loaded and set
  //! as the active file.
  bool loadNextFile();

  //! Attempts to load the file preceding the currently loaded file in the directory listing, then
  //! display it and set it as the new active file.
  //! 
  //! @returns True if the previous file can be determined and that file is successfully loaded and
  //! set as the active file.
  bool loadPreviousFile();

  //! Attempts to load the nearest untagged file following the currently loaded file in the
  //! directory listing, then display it and set it as the new active file.
  //! 
  //! Untagged in this context refers to files for which not all tags are committed to YES or NO.
  //! 
  //! @returns True if the next untagged file can be determined and that file is successfully loaded
  //! and set as the active file.
  bool loadNextUntaggedFile();

  //! Attempts to load the nearest untagged file preceding the currently loaded file in the
  //! directory listing, then display it and set it as the new active file.
  //! 
  //! Untagged in this context refers to files for which not all tags are committed to YES or NO.
  //! 
  //! @returns True if the next untagged file can be determined and that file is successfully loaded
  //! and set as the active file.
  bool loadPreviousUntaggedFile();

  //! Enters Command Mode, enabling keyboard shortcuts without modifiers.
  void enterCommandMode();

  //! Leaves Command Mode, disabling keyboard shortcuts without modifiers.
  void exitCommandMode();

  //! Identifies the nearest file in the directory that meets specified criteria.
  //! 
  //! Ordering is based on default directory iterator ordering.
  //! 
  //! @param reference The file to look forward or backward from.
  //! @param qualifier The criteria to match.
  //! @param find_next If true, look forward; if false, look backward.
  //! @returns A path to a file satisfying the criteria or an empty optional if no such file can be
  //! found.
  static std::optional<ragtag::path_t> qualifiedFileNavigator(
    const ragtag::path_t& reference, const file_qualifier_t& qualifier, bool find_next);

  //! Retrieves the index of the item representing a path within the directory view list control.
  //! 
  //! @param path The path whose index we seek.
  //! @returns The index of the path within the directory view list control or an empty optional if
  //! the path is not present.
  std::optional<long> getPathListCtrlIndex(const ragtag::path_t& path) const;

  // MENU EVENTS ===================================================================================
  // All functions are invoked upon selecting them via the window's menu or executing the
  // corresponding accelerator. Function signature is dictated by wxEvtHandler::Bind() requirements
  // for events of type wxEVT_MENU.

  //! Invoked when New Project is selected from the menu or activated using its accelerator.
  //! 
  //! Offers the user the opportunity to save their work if appropriate before creating a new
  //! project and setting it as the active one, refreshing all UI elements accordingly.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnNewProject(wxCommandEvent& event);

  //! Invoked when Open Project is selected from the menu or activated using its accelerator.
  //! 
  //! Offers the user the opportunity to save their work if appropriate before prompting the user
  //! to locate a project to load from disk. Upon selection and successful loading, it will be set
  //! as the active project and all UI elements will be updated accordingly.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnOpenProject(wxCommandEvent& event);

  //! Invoked when Save Project is selected from the menu or activated using its accelerator.
  //! 
  //! Saves the project, prompting the user to select a path if the project doesn't have one or if
  //! the current project was built from a backup file. The project is marked clean.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnSaveProject(wxCommandEvent& event);

  //! Invoked when Save Project As is selected from the menu or activated using its accelerator.
  //! 
  //! Saves the project at a user-selected path. The project is marked clean.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnSaveProjectAs(wxCommandEvent& event);

  //! Invoked when Create Tag is selected from the menu, activated using its accelerator, or
  //! activated through its button.
  //! 
  //! Opens a dialog window allowing the user to define a new tag and its properties. If the dialog
  //! is confirmed, the tag is added to the project and UI elements are updated accordingly.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnDefineNewTag(wxCommandEvent& event);

  //! Invoked when Clear Tags from Active File is selected from the menu, activated using its
  //! accelerator, or activated through the Clear File Tags button.
  //! 
  //! Sets all tags to UNCOMMITTED on the current file.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnClearTagsFromFile(wxCommandEvent& event);

  //! Invoked when Default Tags on Active File is selected from the menu, activated using its
  //! accelerator, or activated through the Default File Tags button.
  //! 
  //! Sets all tags to their default state on the current file. (These defaults are properties of
  //! the tags assigned during tag creation.)
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnSetTagsToDefaults(wxCommandEvent& event);

  //! Invoked when Enter Command Mode is selected from the menu or activated using its accelerator.
  //! 
  //! Enters Command Mode, allowing simple keyboard-based directory navigation and tag assignment.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnEnterCommandMode(wxCommandEvent& event);

  //! Invoked when Focus Directory View is selected from the menu or activated using its
  //! accelerator.
  //! 
  //! Exits Command Mode (if active) and places focus on the directory listing.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnFocusDirectoryView(wxCommandEvent& event);

  //! Invoked when Focus Tags is selected from the menu or activated using its accelerator.
  //! 
  //! Exits Command Mode (if active) and places focus on the tag listing.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnFocusTags(wxCommandEvent& event);

  //! Invoked when Show Summary is selected from the menu or activated using its accelerator.
  //! 
  //! Shows the project summary window or focuses it if it is already showhn. 
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnShowSummary(wxCommandEvent& event);

  //! Invoked when Load File is selected from the menu or activated using its accelerator.
  //! 
  //! Prompts the user to load a file. If they do, the active directory is changed, the file is set
  //! as the active file, and the file is added to the current project with default tag settings. If
  //! the file is a media file, it is displayed.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnLoadFile(wxCommandEvent& event);

  //! Invoked when Refresh File View is selected from the menu or activated using its accelerator.
  //! 
  //! Traverses the active directory and updates the directory view with any changes that have been
  //! made. (Perhaps the user added files to the directory and would like to see them reflected in
  //! the app's file listing.)
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnRefreshFileView(wxCommandEvent& event);

  //! Invoked when Next File is selected from the menu or activated using its accelerator.
  //! 
  //! Proceeds to the file following the current one within the current directory. This file is then
  //! considered the active file, displayed in the media viewer, and (if not part of the project)
  //! added to the project with default tag settings.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnNextFile(wxCommandEvent& event);

  //! Invoked when Previous File is selected from the menu or activated using its accelerator.
  //! 
  //! Navigates to the file preceding the current one within the current directory. This file is
  //! then considered the active file, displayed in the media viewer, and (if not part of the
  //! project) added to the project with default tag settings.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnPreviousFile(wxCommandEvent& event);

  //! Invoked when Toggle Mute is selected from the menu or activated using its accelerator.
  //! 
  //! Mutes or unmutes media. The setting is retained between files but not between app sessions.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnToggleMuteMenu(wxCommandEvent& event);

  //! Invoked when Exit is selected from the menu or activated using its accelerator.
  //! 
  //! Offers the user the opportunity to save their project if appropriate, then exits the
  //! application.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnExit(wxCommandEvent& event);

  //! Invoked when About is selected from the menu or activated using its accelerator.
  //! 
  //! Shows information about the application and its developer.
  //! 
  //! @param event The wxCommandEvent describing the user's action.
  void OnAbout(wxCommandEvent& event);

  // CONTROL EVENTS =================================================================================
  // These functions are invoked by the user's interactions with controls in the window.
  // Events of type wxEVT_BUTTON

  void OnTagToggleButtonClick(TagToggleEvent& event);
  void OnStopMedia(wxCommandEvent& event);
  void OnPlayPauseMedia(wxCommandEvent& event);
  void OnPreviousUntaggedFile(wxCommandEvent& event);
  void OnNextUntaggedFile(wxCommandEvent& event);
  void OnClickRatingButton(wxCommandEvent& event);
  // Events of type wxEVT_CHECKBOX
  void OnToggleAutoplay(wxCommandEvent& event);
  void OnToggleLooping(wxCommandEvent& event);
  void OnToggleMuteBox(wxCommandEvent& event);
  // Events of type wxEVT_LIST_ITEM_FOCUSED
  void OnFocusFile(wxListEvent& event);

  // MEDIA EVENTS ==================================================================================
  // Events related to display of the media file.
  void OnMediaLoaded(wxMediaEvent& event);    // wxEVT_MEDIA_LOADED
  void OnMediaStop(wxMediaEvent& event);      // wxEVT_MEDIA_STOP
  void OnMediaFinished(wxMediaEvent& event);  // wxEVT_MEDIA_FINISHED
  void OnMediaPlay(wxMediaEvent& event);      // wxEVT_MEDIA_PLAY
  void OnMediaPause(wxMediaEvent& event);     // wxEVT_MEDIA_PAUSE

  // WINDOW EVENTS =================================================================================
  // Events related to the handling of top-level windows themselves.
  void OnClose(wxCloseEvent& event);      // wxEVT_CLOSE_WINDOW
  void OnKillFocus(wxFocusEvent& event);  // wxEVT_KILL_FOCUS
  // Custom event handler for actions taken within the project summary. (See SummaryFrameEvent.)
  void OnSummaryFrameAction(SummaryFrameEvent& event);

  // KEYBOARD EVENTS ===============================================================================
  void OnKeyDown(wxKeyEvent& event);  // wxEVT_CHAR_DOWN

  // USER INTERFACE ELEMENTS =======================================================================
  //! The "File" menu.
  wxMenu* m_file_{ nullptr };
  //! The "Project" menu.
  wxMenu* m_project_{ nullptr };
  //! The "Media" menu.
  wxMenu* m_media_{ nullptr };
  //! The "Tags" menu.
  wxMenu* m_tags_{ nullptr };
  //! The "Window" menu.
  wxMenu* m_window_{ nullptr };
  //! The "Help" menu.
  wxMenu* m_help_{ nullptr };
  //! The panel used as a container for tag toggle controls.
  wxScrolledWindow* p_tag_toggles_{ nullptr };
  //! The sizer that positions tag toggle controls.
  wxBoxSizer* sz_tag_toggles_{ nullptr };
  //! The media control used to display images and video.
  wxMediaCtrl* mc_media_display_{ nullptr };
  //! The button used to stop the currently playing media.
  wxButton* b_stop_media_{ nullptr };
  //! The button used to play and to paus the currently playing media.
  wxButton* b_play_pause_media_{ nullptr };
  //! The checkbox used to toggle whether the media should automatically begin playing upon load.
  wxCheckBox* cb_autoplay_{ nullptr };
  //! The checkbox used to toggle whether the media should restart after it finishes.
  wxCheckBox* cb_loop_{ nullptr };
  //! The checkbox used to toggle whether the media plays sound.
  wxCheckBox* cb_mute_{ nullptr };
  //! The button used to denote that a file should have no rating. (Note, this is different than 0.)
  wxToggleButton* b_no_rating_{ nullptr };
  //! Buttons used to assign ratings of 0 through 5 stars to the file.
  std::vector<wxToggleButton*> b_ratings_{6};  // 6 ratings: 0 through 5 inclusive
  //! Button that clears tags from the active file by marking them all as uncommitted.
  wxButton* b_clear_tags_from_file_{ nullptr };
  //! Button that sets all tags on the file to the tags' defaults.
  wxButton* b_set_tags_to_defaults_{ nullptr };
  //! Text displaying the directory of the active file.
  wxStaticText* st_current_directory_{ nullptr };
  //! List control displaying the files in the directory of the active file.
  wxListCtrl* lc_files_in_directory_{ nullptr };
  //! Button that refreshes the control displaying the files in the directory of the active file.
  wxButton* b_refresh_file_view_{ nullptr };
  //! Button to change the active file to the previous untagged file within the directory.
  wxButton* b_previous_untagged_file_{ nullptr };
  //! Button to change the active file to the next untagged file within the directory.
  wxButton* b_next_untagged_file_{ nullptr };
  //! The "Project Summary" window.
  SummaryFrame* f_summary_{ nullptr };

  // ADDITIONAL DATA MEMBERS =======================================================================
  //! Array of full paths to files presented by lc_files_in_directory_.
  // Implementation note: Maintaining a parallel array like this isn't ideal, but it's perhaps the
  // least bad way of caching data that items in the list control can refer to by generic pointer as
  // required by the wxListCtrl interface.
  std::vector<ragtag::path_t> file_paths_{};
  //! The tag map defining the active project.
  ragtag::TagMap tag_map_{};
  //! The file path of the current project.
  std::optional<ragtag::path_t> project_path_{};
  //! The file path of the active file.
  std::optional<ragtag::path_t> active_file_{};
  //! Collection of panel UI elements that represent toggle-able tags.
  std::vector<TagTogglePanel*> tag_toggle_panels_{};
  //! Whether the project has been modified since its last save (as governed by makeDirty()).
  bool is_dirty_{ false };
  //! Whether the user requested the media to stop (as opposed to the media automatically stopping).
  bool user_initiated_stop_media_{ false };
  //! Whether changes are being made to the directory view at this instant.
  bool file_view_modification_in_progress_{ false };
  //! Whether Command Mode is active.
  bool command_mode_active_{ true };
};

#endif  // INCLUDE_MAIN_FRAME_H
