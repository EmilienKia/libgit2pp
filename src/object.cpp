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

#include "object.hpp"

#include "blob.hpp"
#include "commit.hpp"
#include "oid.hpp"
#include "repository.hpp"
#include "tag.hpp"
#include "tree.hpp"

namespace git2
{

namespace
{

struct GitObjectDeleter{
	void operator()(git_object *object){
		git_object_free(object);
	}
};

}


//
// Object
//

// git_object_free() is automatically invoked on the pointer when
// it is no more referenced.
Object::Object(git_object *object):
_obj(object, GitObjectDeleter())
{
}

Object::Object(const Object& other):
_obj(other._obj)
{
}

Object::~Object()
{
}

Commit Object::toCommit() const
{
    Commit commit;
    if (isCommit()) {
        commit._obj = _obj;
    }
    return commit;
}

Tag Object::toTag() const
{
    Tag tag;
    if (isTag()) {
        tag._obj = _obj;
    }
    return tag;
}

Tree Object::toTree() const
{
    Tree tree;
    if (isTree()) {
        tree._obj = _obj;
    }
    return tree;
}

Blob Object::toBlob() const
{
    Blob blob;
    if (isBlob()) {
        blob._obj = _obj;
    }
    return blob;
}

bool Object::isNull() const
{
    return (bool)_obj;
}

OId Object::oid() const
{
    return OId(git_object_id(_obj.get()));
}

bool Object::isCommit() const
{
    return git_object_type(_obj.get()) == GIT_OBJ_COMMIT;
}

bool Object::isTag() const
{
    return git_object_type(_obj.get()) == GIT_OBJ_TAG;
}

bool Object::isTree() const
{
    return git_object_type(_obj.get()) == GIT_OBJ_TREE;
}

bool Object::isBlob() const
{
    return git_object_type(_obj.get()) == GIT_OBJ_BLOB;
}

std::string Object::typeString() const
{
    return std::string(git_object_type2string(git_object_type(_obj.get())));
}

Repository Object::owner() const
{
    return Repository(git_object_owner(_obj.get()));
}

git_object* Object::data() const
{
    return _obj.get();
}

const git_object* Object::constData() const
{
    return _obj.get();
}

} // namespace git2
