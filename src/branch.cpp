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

namespace
{

struct GitReferenceDeleter{
	void operator()(git_reference *object){
		git_reference_free(object);
	}
};

}

//
// Branch
//

Branch::Branch(git_reference *ref):
_ref(ref, GitReferenceDeleter())
{
}

Branch::Branch(const Branch& ref):
_ref(ref._ref)
{
}

Branch::~Branch()
{
}

void Branch::del()
{
	Exception::assert(git_branch_delete(data()));
}

bool Branch::isHead()const
{
	return git_branch_is_head(data()) != 0;
}

void Branch::move(const std::string& branchName, bool force)
{
	git_reference *out;
	Exception::assert(git_branch_move(&out, data(), branchName.c_str(), force?1:0));
	_ref = ptr_type(out, GitReferenceDeleter());
}

std::string Branch::name() const
{
	const char *out;
	Exception::assert(git_branch_name(&out, data()));
	return std::string(out);
}

Branch Branch::upstream() const
{
	git_reference *out;
	Exception::assert(git_branch_upstream(&out, data()));
	return Branch(out);
}

void Branch::setUpstream(const std::string& upstreamName)
{
	Exception::assert(git_branch_set_upstream(data(), upstreamName.c_str()));
}


git_reference* Branch::data() const
{
    return _ref.get();
}

const git_reference* Branch::constData() const
{
    return _ref.get();
}



} // namespace git2

