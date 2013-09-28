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

#ifndef _REVWALK_HPP_
#define _REVWALK_HPP_

#include <git2.h>

#include <string>

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
class  RevWalk
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

    /**
     * Allocate a new revision walker to iterate through a repo.
     *
     * @param repo the repo to walk through
     */
    RevWalk(const Repository& repository);

    RevWalk( const RevWalk& revwalk );

    /**
     * Delete a revwalk previously allocated.
     */
    ~RevWalk();

    /**
     * Reset the walking machinery for reuse.
     */
    void reset() const;

    /**
     * Mark the commit with the given oid as a starting point.
     *
     * This method adds the commit with the given id as one
     * of the starting points for the revision traversal.
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
     * Mark the given reference as a starting point.
     *
     * This method adds the given reference as one
     * of the starting points for the revision traversal.
     *
     * @param reference the reference to strat from.
     * @throws Exception
     */
    void push(const Reference& reference) const;

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
     * Mark HEAD as a starting point.
     *
     * This method adds HEAD as a starting point for the revision traversal.
     *
     * @throws LibQGit2::Exception
     */
    void pushHead() const;

    /**
     * Uses the given range to perform the traversal.
     *
     * This method accepts a range in the form <commit1>..<commit2>,
     * according to Git range syntax, i.e. walk all commits that are
     * reachable from commit2 excluding those reachable from commit1.
     */
// TODO only available from v0.18.0
//    void pushRange(const std::string& range) const;

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
     * Get the next commit from the revision traversal and look it up in the owner repository.
     *
     * @param commit The next commit within the set repository, if it was found; otherwise an empty Commit.
     * @return True when the commit was found.
     */
    bool next(Commit& commit);

    /**
     * Change the sorting mode when iterating through the
     * repository's contents.
     * Changing the sorting mode resets the walker.
     *
     * @param sortMode The sorting mode @see SortModes.
     */
    void setSorting(SortModes sortMode);

    /**
     * Return a new repository object initialized to the repository
     * on which this walker is operating.
     *
     * @return a copy of the repository being walked
     */
    Repository* repository();

    /**
     * Return the const repository on which this walker is operating.
     *
     * @return the repository being walked
     */
    const Repository* constRepository();

    git_revwalk* data() const;
    const git_revwalk* constData() const;

private:
    const Repository* _repo;
    git_revwalk* _revwalk;
};


} // namespace git2
#endif // _REVWALK_HPP_

