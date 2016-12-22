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

#include "common.hpp"

#include "diff.hpp"

namespace git2
{


/**
 * You will find a complete description of libgit status values in git2/status.h
 * in the libgit2 code tree.
 * Remember that Git status refers both to index and workdir, i.e. to modifications
 * you already staged (index) and to modifications made in the local workdir and not yet staged (workdir).
 */
class Status
{
public:
    Status(unsigned int status_flags);

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
    unsigned int _status;
};

typedef std::function<bool(const std::string& path, Status status_flags)> StatusCallbackFunction;

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

#if 0 // Removed for upgrading to 0.24.0

	/**
	 * Return the delta between head and index
	 */
	DiffDelta headToIndexDelta();
	
	/**
	 * Return the delta between index and workdir
	 */
	DiffDelta indexToWorkdirDelta();

#endif // Removed for upgrading to 0.24.0
        
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
class StatusList  : public helper::Git2PtrWrapper<git_status_list, git_status_list_free>
{
public:
    StatusList(git_status_list *statusList = NULL);

    StatusList(const StatusList& other);

    /**
     * Returns the number of entries in the status list.
     */
    size_t entryCount()const;

    /**
     * Returns the entry with the given index.
     */
    StatusEntry entryByIndex(size_t idx)const;

};


} // namespace git2
#endif // _GIT2PP_STATUS_HPP_

