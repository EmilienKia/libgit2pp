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

#ifndef _GIT2PP_REVWALK_HPP_
#define _GIT2PP_REVWALK_HPP_

#include <git2.h>

#include <memory>
#include <string>

#include "common.hpp"

namespace git2
{

class Commit;
class Exception;
class OId;
class Reference;
class Repository;


/**
  * The revision walker can be used to traverse Git commit history.
  * It features sorting abilities and more.
  */
class  RevWalk : public helper::Git2PtrWrapper<git_revwalk, git_revwalk_free>
{
public:
    /**
     * Defines the sort mode when walking revisions.
     */
    enum SortMode
    {
        None = GIT_SORT_NONE,
        Time = GIT_SORT_TIME,
        Topological = GIT_SORT_TOPOLOGICAL,
        Reverse = GIT_SORT_REVERSE
    };

    typedef unsigned int SortModes; //!< Combination of SortMode

    RevWalk(git_revwalk* revwalk);

    RevWalk( const RevWalk& other );

    /**
     * Delete a revwalk previously allocated.
     */
    ~RevWalk();

    /**
	 * Reset the revision walker for reuse.
	 *
	 * This will clear all the pushed and hidden commits, and
	 * leave the walker in a blank state (just like at
	 * creation) ready to receive new commit pushes and
	 * start a new walk.
	 *
	 * The revision walk is automatically reset when a walk
	 * is over.
     */
    void reset() const;

    /**
	 * Mark a commit to start traversal from.
	 *
	 * The given OID must belong to a commit on the walked
	 * repository.
	 *
	 * The given commit will be used as one of the roots
	 * when starting the revision walk. At least one commit
	 * must be pushed the repository before a walk can
	 * be started.
     *
     * @param oid the oid of the commit to start from.
     * @throws Exception
     */
    void push(const OId& oid) const;

    /**
     * Mark the given commit as a starting point.
     *
     * This method adds the given commit as one
     * of the starting points for the revision traversal.
     *
     * @param commit the commit to start from.
     * @throws Exception
     */
    void push(const Commit& commit) const;

    /**
	 * Push matching references
	 *
	 * The OIDs pinted to by the references that match the given glob
	 * pattern will be pushed to the revision walker.
	 *
	 * A leading 'refs/' is implied it not present as well as a trailing
	 * '/ *' if the glob lacks '?', '*' or '['.
     *
     * @param reference the reference to strat from.
     * @throws Exception
     */
    void push(const Reference& reference) const;

	/**
	 * Push the OID pointed to by a reference
	 *
	 * The reference must point to a commit.
	 */
	void pushRef(const std::string& refname);

    /**
     * Mark the references matching the given glob as a starting point.
     *
     * This method adds the references which name matches the given
     * glob as starting points for the revision traversal.
     *
     * @param glob the glob that reference names shall match.
     * @throws Exception
     */
    void push(const std::string& glob) const;

    /**
	 * Push the repository's HEAD
     *
     * This method adds HEAD as a starting point for the revision traversal.
     *
     * @throws LibQGit2::Exception
     */
    void pushHead() const;

    /**
	 * Push and hide the respective endpoints of the given range.
	 *
	 * The range should be of the form <commit>..<commit>
	 * The left-hand commit will be hidden and the right-hand commit pushed.
     */
    void pushRange(const std::string& range) const;

    /**
     * Hide the commit with the given oid and its ancestors from the walker.
     *
     * This method hides the commit with the given id from
     * the output of the revision traversal.
     *
     * @param oid the oid of the commit to start from.
     * @throws Exception
     */
    void hide(const OId& oid) const;

    /**
     * Hide the given commit and its ancestors from the walker.
     *
     * This method hides the given commit from
     * the output of the revision traversal.
     *
     * @param commit the commit to start from.
     * @throws Exception
     */
    void hide(const Commit& commit) const;

    /**
     * Hide the given reference and its ancestors from the walker.
     *
     * This method hides the given reference from
     * the output of the revision traversal.
     *
     * @param reference the reference to strat from.
     * @throws Exception
     */
    void hide(const Reference& reference) const;

	/**
	 * Hide the OID pointed to by a reference
	 *
	 * The reference must point to a commit.
	 */
	void hideRef(const std::string& refname);

    /**
     * Hide the references matching the given glob and its ancestors from the walker.
     *
     * This method hides the references which name matches the given
     * glob from the output of the revision traversal.
     *
     * @param glob the glob that reference names shall match.
     * @throws Exception
     */
    void hide(const std::string& glob) const;

    /**
     * Hide HEAD from the walker.
     *
     * This method hides HEAD and its ancestors from the output of the revision traversal.
     *
     * @throws Exception
     */
    void hideHead() const;

    /**
     * Get the oid of the next commit from the revision traversal.
     *
     * @param oid The oid of the next object in the revisions tree if it was found; otherwise it is undefined.
     * @return True if the object was found.
     */
    bool next(OId& oid) const;

    /**
     * Change the sorting mode when iterating through the
     * repository's contents.
     * Changing the sorting mode resets the walker.
     *
     * @param sortMode The sorting mode @see SortModes.
     */
    void setSorting(SortModes sortMode);
};


} // namespace git2
#endif // _GIT2PP_REVWALK_HPP_

