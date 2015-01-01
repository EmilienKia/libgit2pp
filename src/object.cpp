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


//
// Object
//

Object::Object(git_object *object):
_Class(object)
{
}

Object::Object(const Object& other):
_Class(other.data())
{
}

Commit Object::toCommit() const
{
	if(isCommit())
		return Commit(*this);
	else
		return Commit();
}

Tag Object::toTag() const
{
	if(isTag())
		return Tag(*this);
	else
		return Tag();
}

Tree Object::toTree() const
{
	if(isTree())
		return Tree(*this);
	else
		return Tree();
}

Blob Object::toBlob() const
{
	if(isBlob())
		return Blob(*this);
	else
		return Blob();
}

bool Object::isNull() const
{
    return data()==nullptr;
}

OId Object::oid() const
{
    return OId(git_object_id(data()));
}

bool Object::checkType(Type type) const
{
	return git_object_type(data()) == (git_otype)type;
}

Object::Type Object::getType() const
{
	return (Object::Type) git_object_type(data());
}

std::string Object::getTypeString() const
{
	return std::string(git_object_type2string(git_object_type(data())));
}

std::string Object::type2String(Object::Type type)
{
	return std::string(git_object_type2string((git_otype)type));
}

Object::Type Object::type2String(std::string str)
{
	return (Object::Type)git_object_string2type(str.c_str());
}

bool Object::isLooseType(Object::Type type)
{
	return git_object_typeisloose((git_otype)type) != 0;
}


bool Object::isCommit() const
{
    return git_object_type(data()) == GIT_OBJ_COMMIT;
}

bool Object::isTag() const
{
    return git_object_type(data()) == GIT_OBJ_TAG;
}

bool Object::isTree() const
{
    return git_object_type(data()) == GIT_OBJ_TREE;
}

bool Object::isBlob() const
{
    return git_object_type(data()) == GIT_OBJ_BLOB;
}

std::string Object::typeString() const
{
    return std::string(git_object_type2string(git_object_type(data())));
}

} // namespace git2
