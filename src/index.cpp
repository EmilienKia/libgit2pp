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
#include "tree.hpp"


namespace git2
{

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

int IndexEntry::stage() const
{
	return git_index_entry_stage(constData());
}

const git_index_entry *IndexEntry::constData() const
{
    return _entry;
}

//
// Index
//

namespace
{

struct GitIndexDeleter{
	void operator()(git_index *index){
		git_index_free(index);
	}
};

}

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
    Exception::git2_assert(git_index_open(&index, indexPath.c_str()));
    _index = ptr_type(index, GitIndexDeleter());
}

unsigned int Index::getCapabilities()const
{
	return git_index_caps(constData());
}

void Index::setCapabilities(unsigned int caps)
{
	Exception::git2_assert(git_index_set_caps(data(), caps));
}

void Index::clear()
{
    return git_index_clear(data());
}

void Index::read() const
{
    Exception::git2_assert(git_index_read(data()));
}

void Index::write()
{
    Exception::git2_assert(git_index_write(data()));
}

void Index::readTree(Tree& tree)
{
	Exception::git2_assert(git_index_read_tree(data(), tree.data()));
}

OId Index::writeTree()
{
	git_oid oid;
	Exception::git2_assert(git_index_write_tree(&oid, data()));
	return OId(&oid);
}

size_t Index::entryCount() const
{
    return git_index_entrycount(data());
}


bool Index::find(const std::string& path)
{
    return git_index_find(NULL, data(), path.c_str()) >= 0;
}

void Index::remove(const std::string& path, int stage)
{
    Exception::git2_assert(git_index_remove(data(), path.c_str(), stage));
}

void Index::removeDirectory(const std::string& dir, int stage)
{
	Exception::git2_assert(git_index_remove_directory(data(), dir.c_str(), stage));
}

IndexEntry Index::get(size_t n) const
{
	return IndexEntry(git_index_get_byindex(data(), n));
}

IndexEntry Index::get(const std::string& path, int stage) const
{
	return IndexEntry(git_index_get_bypath(data(), path.c_str(), stage));
}

void Index::add(const IndexEntry& entry)
{
	Exception::git2_assert(git_index_add(data(), entry.constData()));
}

void Index::add(const std::string& path)
{
	Exception::git2_assert(git_index_add_bypath(data(),path.c_str()));
}

void Index::remove(const std::string& path)
{
	Exception::git2_assert(git_index_remove_bypath(data(),path.c_str()));
}

void Index::addConflict(const IndexEntry& ancestor, const IndexEntry& our, const IndexEntry& their)
{
	Exception::git2_assert(git_index_conflict_add(data(), ancestor.constData(), our.constData(), their.constData()));
}

void Index::getConflict(IndexEntry& ancestor, IndexEntry& our, IndexEntry& their, const std::string& path)
{
	// TODO rework it !
	const git_index_entry *out_ancestor, *out_our, *out_their;
	Exception::git2_assert(git_index_conflict_get(&out_ancestor, &out_our, &out_their, data(), path.c_str()));
	ancestor = IndexEntry(out_ancestor);
	our = IndexEntry(out_our);
	their = IndexEntry(out_their);
}

void Index::removeConflict(const std::string& path)
{
	Exception::git2_assert(git_index_conflict_remove(data(), path.c_str()));
}

void Index::cleanupConflict()
{
	git_index_conflict_cleanup(data());
}

bool Index::hasConflicts()const
{
	return git_index_has_conflicts(data()) != 0;
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
