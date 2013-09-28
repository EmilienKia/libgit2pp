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

#include "tree.hpp"

#include "exception.hpp"
#include "oid.hpp"
#include "repository.hpp"

namespace git2
{

//
// TreeEntry
//
TreeEntry::TreeEntry(const git_tree_entry* treeEntry):
_entry(treeEntry)
{
}

TreeEntry::TreeEntry(const TreeEntry& treeEntry):
_entry(treeEntry._entry)
{
}

TreeEntry::~TreeEntry()
{
}

bool TreeEntry::isNull() const
{
    return _entry == 0;
}

// TODO available only from 0.19.0
/*unsigned int TreeEntry::attributes() const
{
    return git_tree_entry_filemode(_entry);
}*/

std::string TreeEntry::name() const
{
    return git_tree_entry_name(_entry);
}

OId TreeEntry::oid() const
{
    return OId(git_tree_entry_id(_entry));
}


Object TreeEntry::toObject(const Repository& repo)
{
    git_object *obj;
    Exception::assert(git_tree_entry_to_object(&obj, repo.data(), _entry));
    return Object(obj);
}

const git_tree_entry* TreeEntry::data() const
{
    return _entry;
}


//
// Tree
//
Tree::Tree(git_tree *tree):
Object(reinterpret_cast<git_object*>(tree))
{
}

Tree::Tree( const Tree& tree ):
Object(tree)
{
}

Tree::~Tree()
{
}

OId Tree::oid()
{
    return OId(git_tree_id(data()));
}

size_t Tree::entryCount()
{
    return git_tree_entrycount(data());
}

TreeEntry Tree::entryByName(const std::string& fileName) const
{
    return TreeEntry(git_tree_entry_byname(const_cast<git_tree*>(constData()), fileName.c_str()));
}

TreeEntry Tree::entryByIndex(int idx) const
{
    return TreeEntry(git_tree_entry_byindex(data(), idx));
}

git_tree* Tree::data() const
{
    return reinterpret_cast<git_tree*>(Object::data());
}

const git_tree* Tree::constData() const
{
    return reinterpret_cast<git_tree*>(Object::data());
}

} // namespace git2

