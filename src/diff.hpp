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

#ifndef _GIT2PP_DIFF_HPP_
#define _GIT2PP_DIFF_HPP_

#if 0 // Removed for upgrading to 0.24.0

#include <git2.h>

#include <memory>

#include "common.hpp"

#include "oid.hpp"

namespace git2
{

class DiffFile;
class DiffDelta;
class DiffList;
class DiffPatch;



/**
 * Diff notification callback function.
 *
 * The callback will be called for each file, just before the `git_delta_t`
 * gets inserted into the diff list.
 *
 * When the callback:
 * - returns < 0, the diff process will be aborted.
 * - returns > 0, the delta will not be inserted into the diff list, but the
 *		diff process continues.
 * - returns 0, the delta is inserted into the diff list, and the diff process
 *		continues.
 * 
 * @param list List to which the delta will be inserted
 * @param delta Delta to insert in the list
 * @param matchedPathspec Path spec
 */
typedef std::function<int(const DiffList& list, const DiffDelta& delta, const std::string& matchedPathspec)> DiffNotifyCallbackFunction;

/**
 * When iterating over a diff, callback that will be made per file.
 *
 * @param delta Delta data for the file
 * @param progress Goes from 0 to 1 over the diff list
 * @return true to continue iteration, false to stop iteration
 */
typedef std::function<bool(const DiffDelta& delta, float progress)> DiffFileCallbackFunction;

/**
 * When iterating over a diff, callback that will be made per hunk.
 */
typedef std::function<bool(const DiffDelta& delta, const git_diff_range *range, const std::string& header)> DiffHunkCallbackFunction;


/**
 * When iterating over a diff, callback that will be made per text diff
 * line. In this context, the provided range will be NULL.
 *
 * When printing a diff, callback that will be made to output each line
 * of text.  This uses some extra GIT_DIFF_LINE_... constants for output
 * of lines of file and hunk headers.
 */
typedef std::function<bool(const DiffDelta& delta, const git_diff_range *range, char line_origin,const std::string& content)> DiffDataCallbackFunction;

/**
 * Description of one side of a diff entry.
 *
 * Although this is called a "file", it may actually represent a file, a
 * symbolic link, a submodule commit id, or even a tree (although that only
 * if you are tracking type changes or ignored/untracked directories).
 *
 * The `oid` is the OID of the item.  If the entry represents an
 * absent side of a diff (e.g. the `old_file` of a `GIT_DELTA_ADDED` delta),
 * then the oid will be zeroes.
 *
 * `path` is the NUL-terminated path to the entry relative to the working
 * directory of the repository.
 *
 * `size` is the size of the entry in bytes.
 *
 * `flags` is a combination of the `git_diff_flag_t` types
 *
 * `mode` is, roughly, the stat() `st_mode` value for the item.  This will
 * be restricted to one of the `git_filemode_t` values.
 */
class DiffFile
{
public:
    /**
     * Create an new diff file object
     */
    DiffFile(const git_diff_file *diff_file = NULL);

    /**
     * Copy constructor
     */
    DiffFile(const DiffFile& other);

	OId oid()const{return _oid;}
	const std::string& path()const{return _path;}
	size_t size()const{return _size;}
	uint32_t flags()const{return _flags;}
	uint16_t mode()const{return _mode;}

protected:
	OId _oid;
	std::string _path;
	size_t _size;
	uint32_t _flags;
	uint16_t _mode;
};


/**
 * Description of changes to one entry.
 *
 * When iterating over a diff list object, this will be passed to most
 * callback functions and you can use the contents to understand exactly
 * what has changed.
 *
 * The `old_file` represents the "from" side of the diff and the `new_file`
 * represents to "to" side of the diff.  What those means depend on the
 * function that was used to generate the diff and will be documented below.
 * You can also use the `GIT_DIFF_REVERSE` flag to flip it around.
 *
 * Although the two sides of the delta are named "old_file" and "new_file",
 * they actually may correspond to entries that represent a file, a symbolic
 * link, a submodule commit id, or even a tree (if you are tracking type
 * changes or ignored/untracked directories).
 *
 * Under some circumstances, in the name of efficiency, not all fields will
 * be filled in, but we generally try to fill in as much as possible.  One
 * example is that the "flags" field may not have either the `BINARY` or the
 * `NOT_BINARY` flag set to avoid examining file contents if you do not pass
 * in hunk and/or line callbacks to the diff foreach iteration function.  It
 * will just use the git attributes for those files.
 *
 * The similarity score is zero unless you call `git_diff_find_similar()`
 * which does a similarity analysis of files in the diff.  Use that
 * function to do rename and copy detection, and to split heavily modified
 * files in add/delete pairs.  After that call, deltas with a status of
 * GIT_DELTA_RENAMED or GIT_DELTA_COPIED will have a similarity score
 * between 0 and 100 indicating how similar the old and new sides are.
 *
 * If you ask `git_diff_find_similar` to find heavily modified files to
 * break, but to not *actually* break the records, then GIT_DELTA_MODIFIED
 * records may have a non-zero similarity score if the self-similarity is
 * below the split threshold.  To display this value like core Git, invert
 * the score (a la `printf("M%03d", 100 - delta->similarity)`).
 */
class DiffDelta
{
public:
    /**
     * Create an new diff delta object
     */
    DiffDelta(const git_diff_delta *diff_delta = NULL);

    /**
     * Copy constructor
     */
    DiffDelta(const DiffDelta& other);

	DiffFile oldFile()const{return _old;}
	DiffFile newFile()const{return _new;}
	git_delta_t status()const{return _status;}
	uint32_t similarity()const{return _similarity;}
	uint32_t flags()const{return _flags;}

    const git_diff_delta* data() const;
protected:
	DiffFile    _old, _new;
	git_delta_t _status;
	uint32_t    _similarity;
	uint32_t    _flags;
};

/**
 * The diff list object that contains all individual file deltas.
 */
class DiffList : public helper::Git2PtrWrapper<git_diff_list, git_diff_list_free>
{
public:
	DiffList(git_diff_list*);
	DiffList(const DiffList& other);

	/**
	 * Merge one diff list into another.
	 *
	 * This merges items from the "from" list into the "this" list.  The
	 * resulting diff list will have all items that appear in either list.
	 * If an item appears in both lists, then it will be "merged" to appear
	 * as if the old version was from the "onto" list and the new version
	 * is from the "from" list (with the exception that if the item has a
	 * pending DELETE in the middle, then it will show as deleted).
	 */
	void merge(const DiffList& from);
	
	/**
	 * Transform a diff list marking file renames, copies, etc.
	 *
	 * This modifies a diff list in place, replacing old entries that look
	 * like renames or copies with new entries reflecting those changes.
	 * This also will, if requested, break modified files into add/remove
	 * pairs if the amount of change is above a threshold.
	 * 
	 * @param flags Combination of git_diff_find_t values (default FIND_RENAMES)
	 * @param renameThreshold Similarity to consider a file renamed (default 50)
	 * @param renameFromRewriteThreshold Similarity of modified to be eligible rename source (default 50)
	 * @param copyThreshold Similarity to consider a file a copy (default 50)
	 * @param breakRewriteThreshold Similarity to split modify into delete/add pair (default 60)
	 * @param renameLimit Maximum similarity sources to examine for a file (somewhat like
	 *  git-diff's `-l` option or `diff.renameLimit` config) (default 200)
	 * 
	 * TODO add git_diff_similarity_metric *metric
	 */
	bool findSimilar();
	bool findSimilar(uint32_t flags, uint16_t renameThreshold=50,
			uint16_t renameFromRewriteThreshold = 50, uint16_t copyThreshold = 50,
			uint16_t breakRewriteThreshold = 60, size_t renameLimit = 200);

	/**
	 * Loop over all deltas in a diff list issuing callbacks.
	 *
	 * This will iterate through all of the files described in a diff.  You
	 * should provide a file callback to learn about each file.
	 *
	 * The "hunk" and "line" callbacks are optional, and the text diff of the
	 * files will only be calculated if they are not NULL.  Of course, these
	 * callbacks will not be invoked for binary files on the diff list or for
	 * files whose only changed is a file mode change.
	 * 
	 * @param fileCallback Callback function to make per file in the diff.
	 * @param hunkCallback Optional callback to make per hunk of text diff.  This
	 *                callback is called to describe a range of lines in the
	 *                diff.  It will not be issued for binary files.
	 * @param lineCallback Optional callback to make per line of diff text.  This
	 *                same callback will be made for context lines, added, and
	 *                removed lines, and even for a deleted trailing newline.
	 * @return true if completly terminated and false if user terminated.
	 */
	bool foreach(DiffFileCallbackFunction fileCallback, DiffHunkCallbackFunction hunkCallback,
		DiffDataCallbackFunction lineCallback);

	/**
	 * Iterate over a diff generating text output like "git diff --name-status".
	 * 
	 * @return true if completly terminated and false if user terminated.
	 */
	bool printCompact(DiffDataCallbackFunction printCallback);

	/**
	 * Iterate over a diff generating text output like "git diff --raw".
	 * 
	 * @return true if completly terminated and false if user terminated.
	 */
	bool printRaw(DiffDataCallbackFunction printCallback);

	/**
	 * Iterate over a diff generating text output like "git diff".
	 *
	 * This is a super easy way to generate a patch from a diff.
	 * 
	 * @return true if completly terminated and false if user terminated.
	 */
	bool printPatch(DiffDataCallbackFunction printCallback);

	/**
	 * Query how many diff records are there in a diff list.
	 * 
	 * @return Count of number of deltas in the list
	 */
	size_t deltaCount()const;
	
	/**
	 * Query how many diff deltas are there in a diff list filtered by type.
	 * 
	 * @param type A git_delta_t value to filter the count
	 * @return Count of number of deltas matching delta_t type
	 */
	size_t deltaCount(git_delta_t type)const;

	/**
	 * Return the diff patch for an entry in the diff list.
	 * 
	 * @param idx Index into diff list
	 * @return delta patch object
	 */
	DiffPatch patch(size_t idx);
	
	/**
	 * Return the diff delta for an entry in the diff list.
	 *
	 * @param idx Index into diff list
	 * @return delta patch object
	 */
	DiffDelta delta(size_t idx);

};


/**
 * The diff patch is used to store all the text diffs for a delta.
 *
 * You can easily loop over the content of patches and get information about
 * them.
 */
class DiffPatch : public helper::Git2PtrWrapper<git_diff_patch, git_diff_patch_free>
{
public:
	DiffPatch(git_diff_patch* patch);
	DiffPatch(const DiffPatch& other);

	/** 
	 * Get the delta associated with a patch
	 */
	DiffDelta delta()const;
	
	/**
	 * Get the number of hunks in a patch
	 */
	size_t hunkCount()const;
	
	/**
	 * Get line counts of each type in a patch.
	 *
	 * This helps imitate a diff --numstat type of output.  For that purpose,
	 * you only need the `total_additions` and `total_deletions` values, but we
	 * include the `total_context` line count in case you want the total number
	 * of lines of diff output that will be generated.
	 *
	 * All outputs are optional. Pass NULL if you don't need a particular count.
	 *
	 * @param totalContext Count of context lines in output, can be NULL.
	 * @param totalAdditions Count of addition lines in output, can be NULL.
	 * @param totalDeletions Count of deletion lines in output, can be NULL.
	 */
	void lineStats(size_t *total_context, size_t *total_additions, size_t *total_deletions)const;
	
	/**
	 * Get the information about a hunk in a patch
	 *
	 * Given a patch and a hunk index into the patch, this returns detailed
	 * information about that hunk.  Any of the output pointers can be passed
	 * as NULL if you don't care about that particular piece of information.
	 * 
	 * @param idx Input index of hunk to get information about
	 * @param range Output pointer to git_diff_range of hunk
	 * @param header Output pointer to header string for hunk.
	 * @param linesInHunk Output count of total lines in this hunk
	 * @param idx Input index of hunk to get information about
	 * @return True if found, false if index is out of range.
	 */
	bool hunk(size_t idx, const git_diff_range **range = NULL, std::string* header = NULL, size_t *linesInHunk = NULL);

	/**
	 * Get the number of lines in a hunk.
	 * 
	 * @param idx Index of the hunk
	 * @return Number of lines in hunk or -1 if invalid hunk index
	 */
	int hunkLineCount(size_t idx);
	
	/**
	 * Get data about a line in a hunk of a patch.
	 *
	 * Given a patch, a hunk index, and a line index in the hunk, this
	 * will return a lot of details about that line.  If you pass a hunk
	 * index larger than the number of hunks or a line index larger than
	 * the number of lines in the hunk, this will return -1.
	 * 
	 * @param idx The index of the hunk
	 * @param line The index of the line in the hunk
	 * @param[out] lineOrigin A GIT_DIFF_LINE constant from above
	 * @param[out] content Content of diff line
	 * @param[out] oldLineno Line number in old file or -1 if line is added
	 * @param[out] newLineno Line number in new file or -1 if line is deleted
	 */
	void hunkLine(size_t idx, size_t line, char *lineOrigin, std::string* content, int *oldLineno, int *newLineno);

	/**
	 * Serialize the patch to text via callback.
	 * @return true if iterating completly, and false if terminated by user.
	 */
	bool print(DiffDataCallbackFunction callback);

};

// TODO wrap git_diff_similarity_metric
// TODO wrap misc functions git_diff_blobs git_diff_patch_from_blobs git_diff_blob_to_buffer git_diff_patch_from_blob_and_buffer


} // namespace git2

#endif // Removed for upgrading to 0.24.0
#endif // _GIT2PP_DIFF_HPP_

