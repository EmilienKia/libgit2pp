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

#ifndef _INDEX_HPP_
#define _INDEX_HPP_

#include <git2.h>

#include <memory>

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
	 * This entry is calculated from the entrie's flag
	 * attribute like this:
	 *
	 *	(entry->flags & GIT_IDXENTRY_STAGEMASK) >> GIT_IDXENTRY_STAGESHIFT
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
class Index
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
    Index(const Index& index);

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
     * Create a new tree object from the index
     *
     * @throws LibQGit2::Exception
     */
// TODO only available since v0.18
//    OId createTree();

    /**
     * Clear the contents (all the entries) of an index object.
     * This clears the index object in memory; changes must be manually
     * written to disk for them to take effect.
     */
    void clear();

    /**
     * Update the contents of an existing index object in memory
     * by reading from the hard disk.
     *
     * @throws Exception
     */
    void read() const;

    /**
     * Write an existing index object from memory back to disk
     * using an atomic file lock.
     *
     * @throws Exception
     */
    void write();

    /**
     * Find the first index of any entires which point to given
     * path in the Git index.
     *
     * @param path path to search
     * @return an index >= 0 if found, -1 otherwise
     */
    bool find(const std::string& path);

	/**
	 * Remove all entries with equal path except last added
	 */
	void uniq();
	
	/**
	 * Add or update an index entry from a file in disk
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
     * @param path filename to add
	 * @param stage stage for the entry 
     * @throws Exception
	 */
	void add(const std::string& path, int stage);
	
    /**
     * Insert an entry into the index.
     * A full copy (including the 'path' string) of the given
     * 'source_entry' will be inserted on the index; if the index
     * already contains an entry for the same path, the entry
     * will be updated.
     *
     * @param entry new entry object
     * @throws Exception
     */
    void add(const IndexEntry& entry);

	/**
	 * Add (append) an index entry from a file in disk
	 *
	 * A new entry will always be inserted into the index;
	 * if the index already contains an entry for such
	 * path, the old entry will **not** be replaced.
	 *
	 * The file `path` must be relative to the repository's
	 * working folder and must be readable.
	 *
	 * This method will fail in bare index instances.
	 * 
     * @param path filename to add
	 * @param stage stage for the entry 
     * @throws Exception
	 */
	void append(const std::string& path, int stage);

	/**
	 * Add (append) an index entry from an in-memory struct
	 *
	 * A new entry will always be inserted into the index;
	 * if the index already contains an entry for the path
	 * in the `entry` struct, the old entry will **not** be
	 * replaced.
	 *
	 * A full copy (including the 'path' string) of the given
	 * 'source_entry' will be inserted on the index.
	 * 
     * @param entry new entry object
     * @throws Exception
     */
    void append(const IndexEntry& entry);
	
    /**
     * Remove an entry from the index given the position
     *
     * @param position position of the entry to remove
     * @throws Exception
     */
    void remove(int position);

    /**
     * Get a pointer to one of the entries in the index
     *
     * This entry can be modified, and the changes will be written
     * back to disk on the next write() call.
     *
     * @param n the position of the entry
     * @return The entry
     */
    IndexEntry get(unsigned int n) const;

    /**
     * Get the count of entries currently in the index
     *
     * @return integer of count of current entries
     */
    unsigned int entryCount() const;


	// TODO add methods related to unmerged entries
	// git_index_entrycount_unmerged, git_index_get_unmerged_bypath, git_index_get_unmerged_byindex

	/**
	 * Read a tree into the index file
	 *
	 * The current index contents will be replaced by the specified tree.
	 *
	 * @param tree tree to read
     * @throws Exception
	 */
	void readTree(Tree& tree);
	
    git_index* data() const;
    const git_index* constData() const;

private:
    typedef std::shared_ptr<git_index> ptr_type;
    ptr_type _index;
};

} // namespace git2
#endif // _INDEX_HPP_

