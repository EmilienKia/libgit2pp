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

#ifndef _COMMIT_HPP_
#define _COMMIT_HPP_

#include <git2.h>

#include "object.hpp"

namespace git2
{


class OId;
class Repository;
class Signature;
class Tree;

/**
 * @brief Wrapper class for git_commit.
 *
 * This class represents a Git commit object.
 *
 * @ingroup LibQGit2
 * @{
 */
class Commit : public Object
{
public:

    /**
     * Creates a Commit that points to commit. The pointer object becomes managed by
     * this Commit, and must not be passed to another Commit or closed outside this
     * object.
     */
    explicit Commit(git_commit *commit = 0);

    /**
     * Copy constructor; creates a copy of the object, sharing the same underlaying data
     * structure.
     */
    Commit(const Commit& other);

    /**
     * Destroys the object.
     */
    ~Commit();

    /**
    * Get the id of a commit.
    */
    OId oid() const;

    /**
     * Get the full message of a commit.
     */
    std::string message() const;

    /**
     * Get the short commit message.
     *
     * Get the first part of the commit message (similar to
     * git log --oneline). The string is further cut when a
     * linebreak is found.
     *
     * @param maxLen maximal length of the resulting string.
     * Default is 80 characters.
     *
     * @return the short message
     */
    std::string shortMessage(size_t maxLen = 80) const;

	/**
	 * Get the encoding for the message of a commit,
	 * as a string representing a standard encoding name.
	 *
	 * The encoding may be empty if the `encoding` header
	 * in the commit is missing; in that case UTF-8 is assumed.
	 *
	 * @return Message encoding.
	 */
	std::string encoding() const;

    /**
     * Get the commit time (i.e. committer time) of this commit.
     */
    time_t time() const;

    /**
     * Get the timezone offset.
     *
     * Get the timezone offset (i.e. committer's preferred timezone)
     * of this commit.
     *
     * @return positive or negative timezone offset, in minutes from UTC
     */
    int timeOffset() const;

    /**
     * Get the committer signature of this commit.
     */
    Signature committer() const;

    /**
     * Get the author signature of this commit.
     */
    Signature author() const;

    /**
     * Get the tree pointed to by this commit.
     *
     * @throws Exception
     */
    Tree tree() const;

	/**
	 * Get the id of the tree pointed to by a commit. This differs from
	 * Commit::tree() in that no attempts are made to fetch an object
	 * from the ODB.
	 */
	OId treeId() const;


    /**
     * Get the number of parents of this commit.
     */
    unsigned int parentCount() const;

    /**
     * Get the specified parent of the commit.
     *
     * This method returns the nth parent of this commit or, if no
     * parent can be found, an empty commit.
     *
     * @param n the position of the parent
     * @return the parent commit or an empty commit
     * @throws Exception
     */
    Commit parent(unsigned n) const;

    /**
     * Get the OId of the specified parent of the commit.
     *
     * This method returns the Oid of the nth parent of this commit or,
     * if no parent can be found, an empty OId.
     *
     * @param n the position of the parent
     * @return the OId of the parent commit or an empty OId
     * @throws Exception
     */
    OId parentId(unsigned n) const;

    git_commit* data() const;
    const git_commit* constData() const;
};



} // namespace git2
#endif // _COMMIT_HPP_

