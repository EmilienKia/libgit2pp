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
#include "branch.hpp"

#include "exception.hpp"
#include "oid.hpp"
#include "repository.hpp"
#include "ref.hpp"
#include "signature.hpp"


namespace git2
{

//
// Branch
//

Branch::Branch(git_reference *ref):
_Class(ref)
{
}

Branch::Branch(const Branch& other):
_Class(other.data())
{
}

void Branch::del()
{
	Exception::git2_assert(git_branch_delete(data()));
}

bool Branch::isHead()const
{
	return git_branch_is_head(data()) != 0;
}

void Branch::move(const std::string& branchName, bool force)
{
	git_reference *out;
	Exception::git2_assert(git_branch_move(&out, data(), branchName.c_str(), force?1:0));
	*this = Branch(out);
}

std::string Branch::name() const
{
	const char *out;
	Exception::git2_assert(git_branch_name(&out, data()));
	return std::string(out);
}

Branch Branch::upstream() const
{
	git_reference *out;
	Exception::git2_assert(git_branch_upstream(&out, data()));
	return Branch(out);
}

void Branch::setUpstream(const std::string& upstreamName)
{
	Exception::git2_assert(git_branch_set_upstream(data(), upstreamName.c_str()));
}

} // namespace git2
