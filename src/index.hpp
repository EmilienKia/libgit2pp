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

#ifndef _GIT2PP_INDEX_HPP_
#define _GIT2PP_INDEX_HPP_

#include <git2.h>

#include <memory>

#include "common.hpp"

namespace git2
{

class OId;
class Tree;

/**
 * Represents a Git index/stage entry.
 */
class IndexEntry
{
public:
    /**
     * Create a new Git index entry object
     */
    explicit IndexEntry(const git_index_entry *entry);

    /**
     * Copy constructor
     */
    IndexEntry(const IndexEntry& entry);

    /**
     * Destruct an existing index object.
     */
    ~IndexEntry();

    /**
     * Get the id of an index entry.
     */
    OId id() const;

    /**
     * Get the path of the index entry, represented by a string
     */
    std::string path() const;

    /**
     * Get the size of the file
     */
    int64_t fileSize() const;

	/**
	 * Return the stage number from a git index entry
	 *
	 * @returns the stage number
	 */
	int stage() const;
	
    const git_index_entry *constData() const;

private:
    const git_index_entry *_entry;
};


/**
 * Represents a Git index a.k.a "the stage".
 */
class Index : public helper::Git2PtrWrapper<git_index, git_index_free>
{
public:

    /**
     * Creates a Index that points to 'index'. The pointer 'index' becomes managed by
     * this Index, and must not be passed to another Index or freed outside this
     * object.
     */
    Index(git_index *index = NULL);

    /**
     * Copy constructor; creates a copy of the object, sharing the same underlaying data
     * structure.
     */
    Index(const Index& other);

    /**
     * Destruct an existing index object.
     */
    ~Index();

    /**
     * Create a index object as a memory representation
     * of the Git index file in 'indexPath', without a repository
     * to back it.
     *
     * Since there is no ODB behind this index, any Index methods
     * which rely on the ODB (e.g. index_add) will fail with the
     * GIT_EBAREINDEX error code.
     *
     * @param index_path the path to the index file in disk
     * @throws Exception
     */
    void open(const std::string& indexPath);

	/**
	 * Read index capabilities flags.
	 * 
	 * @return A combination of GIT_INDEXCAP values
	 */
	unsigned int getCapabilities()const;

	/**
	 * Set index capabilities flags.
	 *
	 * If you pass `GIT_INDEXCAP_FROM_OWNER` for the caps, then the
	 * capabilities will be read from the config of the owner object,
	 * looking at `core.ignorecase`, `core.filemode`, `core.symlinks`.
	 * 
	 * @param caps A combination of GIT_INDEXCAP values
	 */
	void setCapabilities(unsigned int caps);

    /**
     * Update the contents of an existing index object in memory
     * by reading from the hard disk.
     * 
     * If force is true, this performs a "hard" read that discards in-memory
     * changes and always reloads the on-disk index data.
     * If there is no on-disk version, the index will be cleared.
     * If force is false, this does a "soft" read that reloads the index data
     * from disk only if it has changed since the last time it was loaded.
     * Purely in-memory index data will be untouched.
     * Be aware: if there are changes on disk,
     * unwritten in-memory changes are discarded.
     *
     * @param force if true, always reload, vs. only read if file has changed.
     * @throws Exception
     */
    void read(bool force);

    /**
     * Write an existing index object from memory back to disk
     * using an atomic file lock.
     *
     * @throws Exception
     */
    void write();

	/**
	 * Read a tree into the index file
	 *
	 * The current index contents will be replaced by the specified tree.
	 *
	 * @param tree tree to read
     * @throws Exception
	 */
	void readTree(Tree& tree);
	
	/**
	 * Write the index as a tree
	 * 
	 * This method will scan the index and write a representation
	 * of its current state back to disk; it recursively creates
	 * tree objects for each of the subtrees stored in the index,
	 * but only returns the OID of the root tree. This is the OID
	 * that can be used e.g. to create a commit.
	 *
	 * The index instance cannot be bare, and needs to be associated
	 * to an existing repository.
	 *
	 * The index must not contain any file in conflict.
	 * 
	 * @return Written tree OID
	 */
	OId writeTree();

/** @name Raw Index Entry Functions
 *
 * These functions work on index entries, and allow for raw manipulation
 * of the entries.
 */
/**@{*/

    /**
     * Get the count of entries currently in the index
     *
     * @return count of current entries
     */
    size_t entryCount() const;
    
    /**
     * Clear the contents (all the entries) of an index object.
     * This clears the index object in memory; changes must be manually
     * written to disk for them to take effect.
     */
    void clear();
        
    /**
     * Get an entry in the index
     *
     * @param n the position of the entry
     * @return The entry
     */
    IndexEntry get(size_t n) const;
    
    /**
     * Get an entry in the index
     *
     * @param path Path to search
     * @param stage Stage to search
     * @return The entry
     */
    IndexEntry get(const std::string& path, int stage) const;

    /**
     * Remove an entry from the index given the position
     *
     * @param path Path to search
     * @param stage Stage to search
     * @throws Exception
     */
    void remove(const std::string& path, int stage);

	/**
	 * Remove all entries from the index under a given directory
	 * 
	 * @param dir container directory path
	 * @param stage stage to search
	 */
	void removeDirectory(const std::string& dir, int stage);
	
	/**
	 * Add or update an index entry from an in-memory struct.
	 * 
	 * If a previous index entry exists that has the same path and stage
	 * as the given entry, it will be replaced.  Otherwise, the
	 * entry will be added.
	 * 
	 * A full copy (including the 'path' string) of the given
	 * 'entry' will be inserted on the index.
	 * 
	 * @param entry new entry object
	 */
	void add(const IndexEntry& entry);

/**@}*/

/** @name Workdir Index Entry Functions
 *
 * These functions work on index entries specifically in the working
 * directory (ie, stage 0).
 */
/**@{*/

	/**
	 * Add or update an index entry from a file on disk
	 *
	 * The file `path` must be relative to the repository's
	 * working folder and must be readable.
	 *
	 * This method will fail in bare index instances.
	 *
	 * This forces the file to be added to the index, not looking
	 * at gitignore rules.  Those rules can be evaluated through
	 * the git_status APIs (in status.h) before calling this.
	 *
	 * If this file currently is the result of a merge conflict, this
	 * file will no longer be marked as conflicting.  The data about
	 * the conflict will be moved to the "resolve undo" (REUC) section.
	 * 
	 * @param path filename to add
	 */
	void add(const std::string& path);
	
	/**
	 * Remove an index entry corresponding to a file on disk
	 *
	 * The file `path` must be relative to the repository's
	 * working folder.  It may exist.
	 *
	 * If this file currently is the result of a merge conflict, this
	 * file will no longer be marked as conflicting.  The data about
	 * the conflict will be moved to the "resolve undo" (REUC) section.
	 * 
	 * @param path filename to remove
	 */
	void remove(const std::string& path);

	// TODO add methods dealing with callbacks (git_index_add_all, git_index_remove_all, git_index_update_all)

    /**
     * Find the first index of any entires which point to given
     * path in the Git index.
     *
     * @param path path to search
     * @return an index >= 0 if found, -1 otherwise
     */
    bool find(const std::string& path);
    
/**@}*/

/** @name Conflict Index Entry Functions
 *
 * These functions work on conflict index entries specifically (ie, stages 1-3)
 */
/**@{*/

	/**
	 * Add or update index entries to represent a conflict
	 *
	 * The entries are the entries from the tree included in the merge.  Any
	 * entry may be null to indicate that that file was not present in the
	 * trees during the merge.  For example, ancestor_entry may be NULL to
	 * indicate that a file was added in both branches and must be resolved.
	 *
	 * @param ancestor the entry data for the ancestor of the conflict
	 * @param our the entry data for our side of the merge conflict
	 * @param their the entry data for their side of the merge conflict
	 */
	void addConflict(const IndexEntry& ancestor, const IndexEntry& our, const IndexEntry& their);

	/**
	 * Get the index entries that represent a conflict of a single file.
	 * 
	 * @param ancestor the entry data for the ancestor of the conflict
	 * @param our the entry data for our side of the merge conflict
	 * @param their the entry data for their side of the merge conflict
	 * @param path  path to search
	 */
	void getConflict(IndexEntry& ancestor, IndexEntry& our, IndexEntry& their, const std::string& path);
	
	/**
	 * Removes the index entries that represent a conflict of a single file.
	 * 
	 * @param path  path to search
	 */
	void removeConflict(const std::string& path);

	/**
	 * Remove all conflicts in the index (entries with a stage greater than 0.)
	 */
	void cleanupConflict();
	
	/**
	 * Determine if the index contains entries representing file conflicts.
	 */
	bool hasConflicts()const;

	// TODO add functions related to conflict iterators.
	
/**@}*/
};


} // namespace git2

#endif // _GIT2PP_INDEX_HPP_

