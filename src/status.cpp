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

#include "status.hpp"


namespace git2
{

//
// Status
//

Status::Status(unsigned int statusFlags):
_status(statusFlags)
{
}

Status::Status(const Status &status):
_status(status._status)
{
}

Status::~Status()
{
}

bool Status::isCurrent() const
{
    return _status == GIT_STATUS_CURRENT;
}

bool Status::isNewInIndex() const
{
    return _status & GIT_STATUS_INDEX_NEW;
}

bool Status::isModifiedInIndex() const
{
    return _status & GIT_STATUS_INDEX_MODIFIED;
}

bool Status::isDeletedInIndex() const
{
    return _status & GIT_STATUS_INDEX_DELETED;
}

bool Status::isRenamedInIndex() const
{
    return _status & GIT_STATUS_INDEX_RENAMED;
}

bool Status::isTypeChangedInIndex() const
{
    return _status & GIT_STATUS_INDEX_TYPECHANGE;
}

bool Status::isNewInWorkdir() const
{
    return _status & GIT_STATUS_WT_NEW;
}

bool Status::isModifiedInWorkdir() const
{
    return _status & GIT_STATUS_WT_MODIFIED;
}

bool Status::isDeletedInWorkdir() const
{
    return _status & GIT_STATUS_WT_DELETED;
}

bool Status::isRenamedInWorkdir() const
{
    return _status & GIT_STATUS_WT_RENAMED;
}

bool Status::isTypeChangedInWorkdir() const
{
    return _status & GIT_STATUS_WT_TYPECHANGE;
}

unsigned int Status::data() const
{
    return _status;
}

//
// StatusEntry
//

StatusEntry::StatusEntry(const git_status_entry *entry):
_entry(entry)
{
}

StatusEntry::StatusEntry(const StatusEntry &entry):
_entry(entry._entry)
{
}

StatusEntry::~StatusEntry()
{
}

Status StatusEntry::status() const
{
    return Status(_entry->status);
}

#if 0 // Removed for upgrading to 0.24.0

DiffDelta StatusEntry::headToIndexDelta()
{
	return DiffDelta(_entry->head_to_index);
}

DiffDelta StatusEntry::indexToWorkdirDelta()
{
	return DiffDelta(_entry->index_to_workdir);
}

#endif // Removed for upgrading to 0.24.0

std::string StatusEntry::oldPath() const
{
    return std::string(_entry->index_to_workdir->old_file.path);
}

std::string StatusEntry::newPath() const
{
    return std::string(_entry->index_to_workdir->new_file.path);
}

std::string StatusEntry::path() const
{
    if (_entry->index_to_workdir->old_file.path) {
        return std::string(_entry->index_to_workdir->old_file.path);
    } else {
        return std::string(_entry->index_to_workdir->new_file.path);
    }
}


//
// StatusList
//

StatusList::StatusList(git_status_list *statusList):
_Class(statusList)
{
}

StatusList::StatusList(const StatusList &other):
_Class(other.data())
{
}

size_t StatusList::entryCount() const
{
    return git_status_list_entrycount(data());
}

StatusEntry StatusList::entryByIndex(size_t idx)const
{
    return StatusEntry(git_status_byindex(data(), idx));
}



} // namespace git2

