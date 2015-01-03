/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * libgit2pp
 * Copyright (C) 2013-2014 Émilien Kia <emilien.kia@gmail.com>
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

#include "commit.hpp"

#include "exception.hpp"
#include "oid.hpp"
#include "signature.hpp"
#include "tree.hpp"

#include <string>
#include <regex>

namespace git2
{

Commit::Commit(git_commit *commit):
Object(reinterpret_cast<git_object*>(commit))
{
}

Commit::Commit(const Object& object):
Object(object)
{
}

Commit::Commit(const Commit& other):
Object(other)
{
}

OId Commit::oid() const
{
    return OId(git_commit_id(data()));
}

std::string Commit::message() const
{
    return std::string(git_commit_message(data()));
}

std::string Commit::shortMessage(size_t maxLen) const
{
	std::string msg = message().substr(0, maxLen);
	size_t idx = msg.find_first_of("\r\n");
	if(idx!=std::string::npos)
	{
		return msg.substr(0, idx);
	}
	else
	{
		return msg;
	}
}

std::string Commit::messageEncoding() const
{
	return std::string(git_commit_message_encoding(data()));
}

time_t Commit::time() const
{
    return git_commit_time(data());
}

int Commit::timeOffset() const
{
    return git_commit_time_offset(data());
}

Signature Commit::committer() const
{
    return Signature(git_commit_committer(data()));
}

Signature Commit::author() const
{
    return Signature(git_commit_author(data()));
}

Tree Commit::tree() const
{
    git_tree *tree;
    Exception::git2_assert(git_commit_tree(&tree, data()));
    return Tree(tree);
}

OId Commit::treeId() const
{
	return OId(git_commit_tree_id(data()));
}

unsigned int Commit::parentCount() const
{
    return git_commit_parentcount(data());
}
Commit Commit::parent(unsigned n) const
{
    git_commit *parent;
    Exception::git2_assert(git_commit_parent(&parent, data(), n));
    return Commit(parent);
}

OId Commit::parentId(unsigned n) const
{
    return OId(git_commit_parent_id(data(), n));
}

Commit Commit::getNthGenAncestor(unsigned n) const
{
    git_commit *ancestor;
    Exception::git2_assert(git_commit_nth_gen_ancestor(&ancestor, data(), n));
    return Commit(ancestor);
}

git_commit* Commit::data()const
{
	return reinterpret_cast<git_commit*>(Object::data());
}

} // namespace git2

