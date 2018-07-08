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

#ifndef _GIT2PP_BRANCH_HPP_
#define _GIT2PP_BRANCH_HPP_

#include "common.hpp"


namespace git2
{

class OId;
class Repository;
class Reference;
class RefLog;
class RefLogEntry;
class Signature;

/**
 * Represents a Git branch.
 * Internally, branches are references.
 */
class Branch : public helper::Git2PtrWrapper<git_reference, git_reference_free>
{
public:

    /**
     * Create an new branch object
     */
    Branch(git_reference *ref = NULL);

    /**
     * Copy constructor
     */
    Branch(const Branch& other);

	/**
	 * Delete an existing branch reference.
	 */
	void del();
	
	// TODO implement foreach function (git_branch_foreach)
	
	/**
	 * Determine if the current local branch is pointed at by HEAD.
	 */
	bool isHead()const;

	/**
	 * Move/rename an existing local branch reference.
	 * 
	 * @param branchName Target name of the branch once the move is performed;
	 * this name is validated for consistency.
	 * @param force Overwrite existing branch.
	 */
	void move(const std::string& branchName, bool force);

	/**
	 * Return the name of the given local or remote branch.
	 */
	std::string name() const;
	
	/**
	 * Return the remote tracking branch, given a local branch reference.
	 */
	Branch upstream() const;

	/**
	 * Set the upstream configuration for a given local branch.
	 * 
	 * @param upstreamName remote-tracking or local branch to set as upstream.
	 * Pass empty to unset.
	 */
	void setUpstream(const std::string& upstreamName);

};

} // namespace git2
#endif // _GIT2PP_BRANCH_HPP_

