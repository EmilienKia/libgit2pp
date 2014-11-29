/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * libgit2pp
 * Copyright (C) 2013 Émilien Kia <emilien.kia@gmail.com>
 * 
 * libgit2pp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libgit2pp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#ifndef _GIT2PP_STATUS_HPP_
#define _GIT2PP_STATUS_HPP_

#include <git2.h>

#include <memory>
#include <string>

namespace git2
{

#ifdef libgit_v0_18_0

/**
 * You will find a complete description of libgit status values in git2/status.h
 * in the libgit2 code tree.
 * Remember that Git status refers both to index and workdir, i.e. to modifications
 * you already staged (index) and to modifications made in the local workdir and not yet staged (workdir).
 */
class Status
{
public:
    Status(const git_status_t status_flags);

    Status(const Status& status);

    ~Status();

    /**
     * Returns true if no changes are pending
     */
    bool isCurrent() const;

    /**
     * Returns true if the file is new in the index
     */
    bool isNewInIndex() const;

    /**
     * Returns true if the file has been modified in the index
     */
    bool isModifiedInIndex() const;

    /**
     * Returns true if the file has been deleted in the index
     */
    bool isDeletedInIndex() const;

    /**
     * Returns true if the file has been renamed in the index
     */
    bool isRenamedInIndex() const;

    /**
     * Returns true if the file type has been changed in the index
     */
    bool isTypeChangedInIndex() const;

    /**
     * Returns true if the file is new in the workdir
     */
    bool isNewInWorkdir() const;

    /**
     * Returns true if the file has been modified in the workdir
     */
    bool isModifiedInWorkdir() const;

    /**
     * Returns true if the file has been deleted in the workdir
     */
    bool isDeletedInWorkdir() const;

    /**
     * Returns true if the file has been renamed in the workdir
     */
    bool isRenamedInWorkdir() const;

    /**
     * Returns true if the file type has been changed in the workdir
     */
    bool isTypeChangedInWorkdir() const;

    unsigned int data() const;

private:
    git_status_t _status;
};

#endif // libgit_v0_18_0


/**
 * You will find a complete description of status flags and options in git2/status.h
 * in the libgit2 code tree. This object avoid the coder to use the libgit2 defines to
 * get and set states and show modes.
 */
class StatusOptions
{
public:

    enum ShowFlag {
        ShowIndexAndWorkdir = GIT_STATUS_SHOW_INDEX_AND_WORKDIR,
        ShowOnlyIndex = GIT_STATUS_SHOW_INDEX_ONLY,
        ShowOnlyWorkdir = GIT_STATUS_SHOW_WORKDIR_ONLY
    };

	typedef unsigned int ShowFlags; //!< Combination of ShowFlag

    enum StatusFlag {
        IncludeUntracked = GIT_STATUS_OPT_INCLUDE_UNTRACKED,
        IncludeIgnored = GIT_STATUS_OPT_INCLUDE_IGNORED,
        IncludeUnmodified = GIT_STATUS_OPT_INCLUDE_UNMODIFIED,
        RecurseUntrackedDirs = GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS,

// TODO defined in v0.18.0
//        ExcludeSubmodules = GIT_STATUS_OPT_EXCLUDE_SUBMODULES,
//        DisablePathspecMatch = GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH,
//        RecurseIgnoredDirs = GIT_STATUS_OPT_RECURSE_IGNORED_DIRS,

// TODO defined in v0.19.0
//        RenamesHeadToIndex = GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX,
//        RenamesIdexToWorkdir = GIT_STATUS_OPT_RENAMES_INDEX_TO_WORKDIR,
//        SortCaseSensitively = GIT_STATUS_OPT_SORT_CASE_SENSITIVELY,
//        SortCaseInsensitively = GIT_STATUS_OPT_SORT_CASE_INSENSITIVELY
    };

	typedef unsigned int StatusFlags; //!< Combination of StatusFlag

    StatusOptions();

    StatusOptions(git_status_options statusOptions);

    StatusOptions(const StatusOptions& statusOptions);

    ~StatusOptions();

    ShowFlags showFlags() const;

    void setShowFlags(StatusOptions::ShowFlags sf);

    StatusFlags statusFlags() const;

    void setStatusFlags(StatusOptions::StatusFlags sf);

    git_status_options data() const;
    const git_status_options constData() const;

private:
    git_status_options _statusOptions;

    ShowFlags _showFlags;
    StatusFlags _statusFlags;
};

#ifdef libgit_v0_19_0

/**
 * Represents a status entry in a Git repository, that is a Git status linked to a file name.
 * Actually the status entry encompasses two file names, to take renames into account.
 */
class StatusEntry
{
public:

    StatusEntry(const git_status_entry *entry);

    StatusEntry(const StatusEntry& entry);

    ~StatusEntry();

    /**
     * Return the status of the entry
     */
    Status status() const;

    /**
     * Returns the old path if set, otherwise an empty string
     */
    std::string oldPath() const;

    /**
     * Returns the new path if set, otherwise an empty string
     */
    std::string newPath() const;

    /**
     * Returns the path if set, otherwise an empty string
     */
    std::string path() const;

private:
    const git_status_entry* _entry; //!< Internal pointer to the libgit2 status entry
};


/**
 * Represents a list of status entries in a Git repository. This is not a simple QList of StatusEntry,
 * it wraps the underlying libgit2 functions.
 */
class StatusList
{
public:
    StatusList(git_status_list *statusList = NULL);

    StatusList(const StatusList& statusList);

    ~StatusList();

    /**
     * Returns the number of entries in the status list.
     */
    size_t entryCount();

    /**
     * Returns the entry with the given index.
     */
    const StatusEntry entryByIndex(size_t idx);

    git_status_list* data() const;
    const git_status_list* constData() const;

private:
    typedef std::shared_ptr<git_status_list> ptr_type;
    ptr_type _statusList;

};

#endif // libgit_v0_19_0


} // namespace git2
#endif // _GIT2PP_STATUS_HPP_

