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


RevWalk::RevWalk(const Repository& repository)
{
    git_revwalk_new(&_revwalk, repository.data());
    _repo = &repository;
}

RevWalk::RevWalk( const RevWalk& revwalk ):
_revwalk(revwalk._revwalk)
{
}

RevWalk::~RevWalk()
{
    git_revwalk_free(_revwalk);
}

void RevWalk::reset() const
{
    git_revwalk_reset(_revwalk);
}

void RevWalk::push(const OId& oid) const
{
    Exception::git2_assert(git_revwalk_push(_revwalk, oid.constData()));
}

void RevWalk::push(const Commit& commit) const
{
    Exception::git2_assert(git_revwalk_push(_revwalk, commit.oid().constData()));
}

void RevWalk::push(const Reference& reference) const
{
    Exception::git2_assert(git_revwalk_push_glob(_revwalk, reference.name().c_str()));
}

void RevWalk::push(const std::string& glob) const
{
    Exception::git2_assert(git_revwalk_push_glob(_revwalk, glob.c_str()));
}

void RevWalk::pushHead() const
{
    Exception::git2_assert(git_revwalk_push_head(_revwalk));
}

// TODO only available from v0.18.0
/*void RevWalk::pushRange(const std::string& range) const
{
    Exception::git2_assert(git_revwalk_push_range(_revwalk, range.c_str()));
}*/

void RevWalk::hide(const OId& oid) const
{
    Exception::git2_assert(git_revwalk_hide(_revwalk, oid.constData()));
}

void RevWalk::hide(const Commit& commit) const
{
    Exception::git2_assert(git_revwalk_hide(_revwalk, commit.oid().constData()));
}

void RevWalk::hide(const Reference& reference) const
{
    Exception::git2_assert(git_revwalk_hide_glob(_revwalk, reference.name().c_str()));
}

void RevWalk::hide(const std::string& glob) const
{
    Exception::git2_assert(git_revwalk_hide_glob(_revwalk, glob.c_str()));
}

void RevWalk::hideHead() const
{
    Exception::git2_assert(git_revwalk_hide_head(_revwalk));
}

bool RevWalk::next(OId& oid) const
{
    int err = git_revwalk_next(oid.data(), _revwalk);
    return (err == GIT_OK);
}

bool RevWalk::next(Commit& commit)
{
    OId oid;
    int err = git_revwalk_next(oid.data(), _revwalk);

    if ( (err != GIT_OK) || !oid.isValid() )
        commit = Commit();
    else
        commit = constRepository()->lookupCommit(oid);

    return !commit.isNull();
}

void RevWalk::setSorting(SortModes sm)
{
    git_revwalk_sorting(_revwalk, sm);
}

Repository* RevWalk::repository()
{
    Repository* repo = new Repository(git_revwalk_repository(_revwalk));
    return repo;
}

const Repository* RevWalk::constRepository()
{
    return _repo;
}

git_revwalk* RevWalk::data() const
{
    return _revwalk;
}

const git_revwalk* RevWalk::constData() const
{
    return _revwalk;
}



} // namespace git2

