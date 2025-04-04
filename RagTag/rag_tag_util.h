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

#ifndef INCLUDE_RAG_TAG_UTIL_H
#define INCLUDE_RAG_TAG_UTIL_H

#include "tag_map.h"
#include <wx/string.h>
#include <string>
#include <vector>

//! Collection of shared variables and functions related to RagTag.
namespace RagTagUtil {
//! Glyph used in a textual context to denote that a tag has been set to "yes."
extern const wxString GLYPH_CHECKED;
//! Glyph used in a textual context to denote that a tag is "uncommitted."
extern const wxString GLYPH_UNCOMMITTED;
//! Glyph used in a textual context to denote that a tag has been set to "no."
extern const wxString GLYPH_UNCHECKED;
//! Glyph used in a textual context to communicate a full star of rating.
extern const wxString GLYPH_RATING_FULL_STAR;
//! Glyph used in a textual context to communicate a half star of rating.
extern const wxString GLYPH_RATING_HALF_STAR;
//! Maximum quantity of stars used to present a rating as a repeated sequence of stars.
extern const int MAX_STARS;
//! Default extension for RagTag project files.
extern const ragtag::path_t DEFAULT_TAG_MAP_FILE_EXTENSION;
//! Default extension for RagTag project file backups.
extern const ragtag::path_t BACKUP_TAG_MAP_FILE_EXTENSION;
//! Major version of the RagTag app per semantic versioning conventions.
extern const int RAGTAG_APP_VERSION_MAJOR;
//! Minor version of the RagTag app per semantic versioning conventions.
extern const int RAGTAG_APP_VERSION_MINOR;
//! Patch version of the RagTag app per semantic versioning conventions.
extern const int RAGTAG_APP_VERSION_PATCH;

//! Produces a string of repeated stars representing the given rating.
//! 
//! @param rating The rating to present as a string of repeated stars.
//! @returns The string of repeated stars for the given rating.
wxString getStarTextForRating(float rating);

//! Infers whether a file is a static image for which "play," "stop," and similar controls don't
//! make sense.
//! 
//! The function checks the file extension to make its determination.
//! 
//! @param path The file to check.
//! @returns True if we characterize the file as static media.
bool isStaticMedia(const ragtag::path_t& path);

//! Produces a string where each of the paths in the given vector is placed on a separate line.
//! 
//! @param paths The vector of paths to present as a newline-delimited string.
//! @returns The newline-delimited string of the paths.
std::wstring getPathsAsNewlineDelimitedString(const std::vector<ragtag::path_t>& paths);

//! Attempts to move the given file to the Windows Recycle Bin without a prompt.
//! 
//! @param path The path of the file to move to the Recycle Bin.
//! @returns Whether the operation was successful.
bool deleteFile(const ragtag::path_t& path);

//! Gets the app version as a string.
//! 
//! The version is presented as period-delimited collection of numbers prefaced by "v".
//! 
//! @returns A string containing the version of the RagTag app.
std::wstring getRagTagAppVersionString();

//! Produces a suitable path to use to save a backup version of a project.
//! 
//! This function encodes date information into the filename and gives the file a different
//! extension.
//! 
//! @param nominal_path The path of the main project.
//! @returns A suitable path for a project backup file.
ragtag::path_t getBackupPath(const ragtag::path_t& nominal_path);
}  // namespace RagTagUtil

#endif  // INCLUDE_RAG_TAG_UTIL_H
