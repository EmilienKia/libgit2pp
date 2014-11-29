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

#ifndef _GIT2PP_TREE_HPP_
#define _GIT2PP_TREE_HPP_

#include <git2.h>

#include "object.hpp"

#include <string>

namespace git2
{

class OId;
class Object;
class Repository;

/**
 * Represents a Git child tree entry, that can either point to another tree object or a blob.
 */
class TreeEntry
{
public:
    explicit TreeEntry(const git_tree_entry* treeEntry);
    TreeEntry(const TreeEntry& treeEntry);
    ~TreeEntry();

    /**
      * @return true when internal pointer is 0; otherwise false
      */
    bool isNull() const;

    /**
     * Get the UNIX file attributes of a tree entry
     * @return attributes as an integer
     */
    // TODO unsigned int attributes() const;

    /**
     * Get the filename of a tree entry
     * @return the name of the file
     */
    std::string name() const;

    /**
     * Get the id of the object pointed by the entry
     * @return the oid of the object
     */
    OId oid() const;

    /**
     * Convert a tree entry to the Object it points too.
     *
     * @param object pointer to the converted object
     * @return a reference to the pointed object in the repository
     * @throws Exception
     */
    Object toObject(const Repository& repo);

    const git_tree_entry* data() const;

private:
    const git_tree_entry *_entry;
};

/**
 * Represents a Git tree object.
 */
class Tree : public Object
{
public:
    /**
     * Creates a Tree that points to tree. The pointer object becomes managed by
     * this Tree, and must not be passed to another Tree or closed outside this
     * object.
     */
    explicit Tree(git_tree *tree = 0);

    /**
     * Copy constructor; creates a copy of the object, sharing the same underlaying data
     * structure.
     */
    Tree(const Tree& tree);

    /**
     * Destroys the object.
     */
    ~Tree();

    /**
     * * Get the id of a tree.
     * * @return object identity for the tree.
     * */
    OId oid();

    /**
     * Get the number of entries listed in a tree
     * @return the number of entries in the tree
     */
    size_t entryCount();

    /**
     * Lookup a tree entry by its filename
     * @param filename the filename of the desired entry
     * @return the tree entry; NULL if not found
     */
    TreeEntry entryByName(const std::string& fileName) const;

    /**
     * Lookup a tree entry by its position in the tree
     * @param idx the position in the entry list
     * @return the tree entry; NULL if not found
     */
    TreeEntry entryByIndex(int idx) const;

    git_tree* data() const;
    const git_tree* constData() const;
};

} // namespace git2
#endif // _GIT2PP_TREE_HPP_

