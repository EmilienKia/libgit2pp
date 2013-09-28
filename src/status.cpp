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

#ifdef libgit_v0_18_0

//
// Status
//

Status::Status(const git_status_t _statusFlags):
_status(_statusFlags)
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
    return (unsigned int)_status;
}


#endif // libgit_v0_18_0

//
// StatusOptions
//

StatusOptions::StatusOptions()
{
// TODO only available from v0.18.0
//    _statusOptions.version = GIT_STATUS_OPTIONS_VERSION;
}

StatusOptions::StatusOptions(git_status_options statusOptions):
_statusOptions(statusOptions)
{
}

StatusOptions::StatusOptions(const StatusOptions &statusOptions):
_statusOptions(statusOptions._statusOptions)
{
}

StatusOptions::~StatusOptions()
{
}

StatusOptions::ShowFlags StatusOptions::showFlags() const
{
    return _showFlags;
}

void StatusOptions::setShowFlags(StatusOptions::ShowFlags sf)
{
    _showFlags = sf;
    int i = _showFlags;
    _statusOptions.show = (git_status_show_t)i;

}

StatusOptions::StatusFlags StatusOptions::statusFlags() const
{
    return _statusFlags;
}

void StatusOptions::setStatusFlags(StatusOptions::StatusFlags sf)
{
    _statusFlags = sf;
    _statusOptions.flags = _statusFlags;
}

git_status_options StatusOptions::data() const
{
    return _statusOptions;
}

const git_status_options StatusOptions::constData() const
{
    return _statusOptions;
}


//
// StatusEntry
//

#ifdef libgit_v0_19_0

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
_statusList(statusList, git_status_list_free)
{
}

StatusList::StatusList(const StatusList &statusList):
_statusList(statusList._statusList)
{
}

StatusList::~StatusList()
{
}

size_t StatusList::entryCount()
{
    return git_status_list_entrycount(data());
}

const StatusEntry StatusList::entryByIndex(size_t idx)
{
    return StatusEntry(git_status_byindex(data(), idx));
}

git_status_list* StatusList::data() const
{
    return _statusList.get();
}

const git_status_list* StatusList::constData() const
{
    return _statusList.get();
}

#endif // libgit_v0_19_0


} // namespace git2

