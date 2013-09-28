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

#include "index.hpp"

#include "exception.hpp"
#include "oid.hpp"

namespace git2
{

namespace
{

struct GitIndexDeleter{
	void operator()(git_index *index){
		git_index_free(index);
	}
};

}

//
// IndexEntry
//

IndexEntry::IndexEntry(const git_index_entry *entry):
_entry(entry)
{
}

IndexEntry::IndexEntry(const IndexEntry& entry):
_entry(entry._entry)
{
}

IndexEntry::~IndexEntry()
{
}

OId IndexEntry::id() const
{
    return OId(&_entry->oid);
}

std::string IndexEntry::path() const
{
    return std::string(_entry->path);
}

int64_t IndexEntry::fileSize() const
{
    return _entry->file_size;
}

const git_index_entry *IndexEntry::constData() const
{
    return _entry;
}

//
// Index
//

Index::Index(git_index *index)
    : _index(index, GitIndexDeleter())
{
}

Index::Index(const Index& index)
    : _index(index._index)
{
}

Index::~Index()
{
}

void Index::open(const std::string& indexPath)
{
    _index.reset();
    git_index *index = NULL;
    Exception::assert(git_index_open(&index, indexPath.c_str()));
    _index = ptr_type(index, GitIndexDeleter());
}

// TODO only available since v0.18
/*OId Index::createTree()
{
    OId oid;
    Exception::assert(git_index_write_tree(oid.data(), data()));
    return oid;
}*/

void Index::clear()
{
    return git_index_clear(data());
}

void Index::read() const
{
    Exception::assert(git_index_read(data()));
}

void Index::write()
{
    Exception::assert(git_index_write(data()));
}

int Index::find(const std::string& path)
{
    return git_index_find(data(), path.c_str());
}

// TODO only availabel from v0.18
/*void Index::addByPath(const std::string& path)
{
    Exception::assert(git_index_add_bypath(data(), path.c_str()));
}*/

void Index::remove(int position)
{
    Exception::assert(git_index_remove(data(), position));
}

void Index::add(const IndexEntry &source_entry, int stage)
{
    Exception::assert(git_index_add(data(), source_entry.path().c_str(), stage));
}


// TODO only availabel from v0.18
/*IndexEntry Index::getByIndex(int n) const
{
    return IndexEntry(git_index_get_byindex(data(), n));
}*/

unsigned int Index::entryCount() const
{
    return git_index_entrycount(data());
}

git_index* Index::data() const
{
    return _index.get();
}

const git_index* Index::constData() const
{
    return _index.get();
}



} // namespace git2

