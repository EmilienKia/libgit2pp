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

#include "revwalk.hpp"

#include "commit.hpp"
#include "exception.hpp"
#include "oid.hpp"
#include "ref.hpp"
#include "repository.hpp"

namespace git2
{

RevWalk::RevWalk(git_revwalk* revwalk):
_Class(revwalk)
{
}

RevWalk::RevWalk( const RevWalk& other ):
_Class(other.data())
{
}

RevWalk::~RevWalk()
{
}

void RevWalk::reset() const
{
    git_revwalk_reset(data());
}

void RevWalk::push(const OId& oid) const
{
    Exception::git2_assert(git_revwalk_push(data(), oid.constData()));
}

void RevWalk::push(const Commit& commit) const
{
    Exception::git2_assert(git_revwalk_push(data(), commit.oid().constData()));
}

void RevWalk::push(const Reference& reference) const
{
    Exception::git2_assert(git_revwalk_push_glob(data(), reference.name().c_str()));
}

void RevWalk::pushRef(const std::string& refname)
{
	Exception::git2_assert(git_revwalk_push_ref(data(), refname.c_str()));
}

void RevWalk::push(const std::string& glob) const
{
    Exception::git2_assert(git_revwalk_push_glob(data(), glob.c_str()));
}

void RevWalk::pushHead() const
{
    Exception::git2_assert(git_revwalk_push_head(data()));
}

void RevWalk::pushRange(const std::string& range) const
{
    Exception::git2_assert(git_revwalk_push_range(data(), range.c_str()));
}

void RevWalk::hide(const OId& oid) const
{
    Exception::git2_assert(git_revwalk_hide(data(), oid.constData()));
}

void RevWalk::hide(const Commit& commit) const
{
    Exception::git2_assert(git_revwalk_hide(data(), commit.oid().constData()));
}

void RevWalk::hide(const Reference& reference) const
{
    Exception::git2_assert(git_revwalk_hide_glob(data(), reference.name().c_str()));
}

void RevWalk::hideRef(const std::string& refname)
{
	Exception::git2_assert(git_revwalk_hide_ref(data(), refname.c_str()));
}

void RevWalk::hide(const std::string& glob) const
{
    Exception::git2_assert(git_revwalk_hide_glob(data(), glob.c_str()));
}

void RevWalk::hideHead() const
{
    Exception::git2_assert(git_revwalk_hide_head(data()));
}

bool RevWalk::next(OId& oid) const
{
    int err = git_revwalk_next(oid.data(), data());
    return (err == GIT_OK);
}

void RevWalk::setSorting(SortModes sm)
{
    git_revwalk_sorting(data(), sm);
}


} // namespace git2

