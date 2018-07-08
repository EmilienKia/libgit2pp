/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * libgit2pp
 * Copyright (C) 2014 Émilien Kia <emilien.kia@gmail.com>
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

#include "diff.hpp"

#include "exception.hpp"
#include "oid.hpp"
#include "repository.hpp"
#include "ref.hpp"
#include "signature.hpp"

namespace git2
{


//
// DiffFile
//

DiffFile::DiffFile(const git_diff_file *diff_file):
_oid(diff_file!=NULL ? &diff_file->id : NULL),
_path(diff_file!=NULL ? diff_file->path : ""),
_size(diff_file!=NULL ? diff_file->size : 0),
_flags(diff_file!=NULL ? diff_file->flags : 0),
_mode(diff_file!=NULL ? diff_file->mode : 0)
{
}

DiffFile::DiffFile(const DiffFile& other):
_oid(other._oid),
_path(other._path),
_size(other._size),
_flags(other._flags),
_mode(other._mode)
{
}

#if 0 // Removed for upgrading to 0.24.0


//
// DiffDelta
//

DiffDelta::DiffDelta(const git_diff_delta *diff_delta):
_old(diff_delta!=NULL ? &diff_delta->old_file : NULL),
_new(diff_delta!=NULL ? &diff_delta->new_file : NULL),
_status(diff_delta!=NULL ? diff_delta->status : GIT_DELTA_UNMODIFIED),
_similarity(diff_delta!=NULL ? diff_delta->similarity : 0),
_flags(diff_delta!=NULL ? diff_delta->flags : 0)
{
}

DiffDelta::DiffDelta(const DiffDelta& other):
_old(other._old),
_new(other._new),
_status(other._status),
_similarity(other._similarity),
_flags(other._flags)
{
}

//
// DiffList
//

DiffList::DiffList(git_diff_list* list):
_Class(list)
{
}

DiffList::DiffList(const DiffList& other):
_Class(other.data())
{
}

void DiffList::merge(const DiffList& from)
{
	Exception::git2_assert(git_diff_merge(data(), from.data()));
}

bool DiffList::findSimilar()
{
	return git_diff_find_similar(data(), NULL) == 0;
}

bool DiffList::findSimilar(uint32_t flags, uint16_t renameThreshold,
		uint16_t renameFromRewriteThreshold, uint16_t copyThreshold,
		uint16_t breakRewriteThreshold, size_t renameLimit)
{
	git_diff_find_options options = {
		GIT_DIFF_FIND_OPTIONS_VERSION,
		flags,
		renameThreshold,
		renameFromRewriteThreshold,
		copyThreshold,
		breakRewriteThreshold,
		renameLimit,
		NULL
	};
	return git_diff_find_similar(data(), &options) == 0;
}

bool DiffList::foreach(DiffFileCallbackFunction fileCallback, DiffHunkCallbackFunction hunkCallback,
		DiffDataCallbackFunction lineCallback)
{
	auto file_cb = [](const git_diff_delta *delta, float progress, void *payload)->int
	{
		void** ptr = (void**)payload;
		DiffFileCallbackFunction* callback = (DiffFileCallbackFunction*)ptr[0];
		if(callback!=nullptr)
			return (*callback)(DiffDelta(delta), progress);
		else
			return 0;
	};
	
	auto hunk_cb = [](const git_diff_delta *delta, const git_diff_range *range, const char *header, size_t header_len, void *payload)->int
	{
		void** ptr = (void**)payload;
		DiffHunkCallbackFunction* callback = (DiffHunkCallbackFunction*)ptr[1];
		if(callback!=nullptr)
			return (*callback)(DiffDelta(delta), range, std::string(header, header_len));
		else
			return 0;
	};
	
	auto line_cb = [](const git_diff_delta *delta, const git_diff_range *range, char line_origin, const char *content, size_t content_len, void *payload)->int
	{
		void** ptr = (void**)payload;
		DiffDataCallbackFunction* callback = (DiffDataCallbackFunction*)ptr[2];
		if(callback!=nullptr)
			return (*callback)(DiffDelta(delta), range, line_origin, std::string(content, content_len));
		else
			return 0;
	};
	
	void* ptr[3] = {(void*)&file_cb, (void*)&hunk_cb, (void*)&line_cb};
	
	int res = git_diff_foreach(data(), file_cb, hunk_cb, line_cb, (void*)ptr);
	if(res==GIT_OK)
		return true;
	else if(res==GIT_EUSER)
		return false;
	else
		Exception::git2_assert(res);
}

bool DiffList::printCompact(DiffDataCallbackFunction printCallback)
{
	auto print_cb = [](const git_diff_delta *delta, const git_diff_range *range, char line_origin, const char *content, size_t content_len, void *payload)->int
	{
		DiffDataCallbackFunction* callback = (DiffDataCallbackFunction*)payload;
		if(callback!=nullptr)
			return (*callback)(DiffDelta(delta), range, line_origin, std::string(content, content_len));
		else
			return 0;
	};
	int res = git_diff_print_compact(data(), print_cb, (void*)&printCallback);
	if(res==GIT_OK)
		return true;
	else if(res==GIT_EUSER)
		return false;
	else
		Exception::git2_assert(res);
}

bool DiffList::printRaw(DiffDataCallbackFunction printCallback)
{
	auto print_cb = [](const git_diff_delta *delta, const git_diff_range *range, char line_origin, const char *content, size_t content_len, void *payload)->int
	{
		DiffDataCallbackFunction* callback = (DiffDataCallbackFunction*)payload;
		if(callback!=nullptr)
			return (*callback)(DiffDelta(delta), range, line_origin, std::string(content, content_len));
		else
			return 0;
	};
	int res = git_diff_print_raw(data(), print_cb, (void*)&printCallback);
	if(res==GIT_OK)
		return true;
	else if(res==GIT_EUSER)
		return false;
	else
		Exception::git2_assert(res);
}

bool DiffList::printPatch(DiffDataCallbackFunction printCallback)
{
	auto print_cb = [](const git_diff_delta *delta, const git_diff_range *range, char line_origin, const char *content, size_t content_len, void *payload)->int
	{
		DiffDataCallbackFunction* callback = (DiffDataCallbackFunction*)payload;
		if(callback!=nullptr)
			return (*callback)(DiffDelta(delta), range, line_origin, std::string(content, content_len));
		else
			return 0;
	};
	int res = git_diff_print_patch(data(), print_cb, (void*)&printCallback);
	if(res==GIT_OK)
		return true;
	else if(res==GIT_EUSER)
		return false;
	else
		Exception::git2_assert(res);
}

size_t DiffList::deltaCount()const
{
	return git_diff_num_deltas(data());
}

size_t DiffList::deltaCount(git_delta_t type)const
{
	return git_diff_num_deltas_of_type(data(), type);
}

DiffPatch DiffList::patch(size_t idx)
{
	git_diff_patch *out;
	Exception::git2_assert(git_diff_get_patch(&out, NULL, data(), idx));
	return DiffPatch(out);
}

DiffDelta DiffList::delta(size_t idx)
{
	const git_diff_delta *out;
	Exception::git2_assert(git_diff_get_patch(NULL, &out, data(), idx));
	return DiffDelta(out);
}


//
// DiffPatch
//

DiffPatch::DiffPatch(git_diff_patch* patch):
_Class(patch)
{
}

DiffPatch::DiffPatch(const DiffPatch& other):
_Class(other.data())
{
}


DiffDelta DiffPatch::delta()const
{
	return DiffDelta(git_diff_patch_delta(data()));
}

size_t DiffPatch::hunkCount()const
{
	return git_diff_patch_num_hunks(data());
}

void DiffPatch::lineStats(size_t *total_context, size_t *total_additions, size_t *total_deletions)const
{
	Exception::git2_assert(git_diff_patch_line_stats(total_context, total_additions, total_deletions, data()));
}

bool DiffPatch::hunk(size_t idx, const git_diff_range **range, std::string* header, size_t *linesInHunk)
{
	const char *buffer;
	size_t len;
	
	int res = git_diff_patch_get_hunk(range, header!=NULL ? &buffer : NULL, header!=NULL ? &len : NULL, linesInHunk, data(), idx);
	if(res==GIT_OK)
	{
		if(header!=NULL)
		{
			header->assign(buffer, len);
		}
		return true;
	}
	else if(res==GIT_ENOTFOUND)
	{
		return false;
	}
	else
	{
		Exception::git2_assert(res);
	}
}

int DiffPatch::hunkLineCount(size_t idx)
{
	return git_diff_patch_num_lines_in_hunk(data(), idx);
}

void DiffPatch::hunkLine(size_t idx, size_t line, char *lineOrigin, std::string* content, int *oldLineno, int *newLineno)
{
	const char *buffer;
	size_t content_len;
	Exception::git2_assert(git_diff_patch_get_line_in_hunk(lineOrigin, &buffer, &content_len, oldLineno, newLineno, data(), idx, line));
	if(content!=NULL)
		content->assign(buffer, content_len);
}

bool DiffPatch::print(DiffDataCallbackFunction callback)
{
	auto print_cb = [](const git_diff_delta *delta, const git_diff_range *range, char line_origin, const char *content, size_t content_len, void *payload)->int
	{
		DiffDataCallbackFunction* callback = (DiffDataCallbackFunction*)payload;
		if(callback!=nullptr)
			return (*callback)(DiffDelta(delta), range, line_origin, std::string(content, content_len));
		else
			return 0;
	};
	int res = git_diff_patch_print(data(), print_cb, (void*)&callback);
	if(res==GIT_OK)
		return true;
	else if(res==GIT_EUSER)
		return false;
	else
		Exception::git2_assert(res);
}
#endif // Removed for upgrading to 0.24.0

} // namespace git2