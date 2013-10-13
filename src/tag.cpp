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

#include "tag.hpp"

#include "exception.hpp"
#include "object.hpp"
#include "oid.hpp"
#include "signature.hpp"

namespace git2
{


Tag::Tag(git_tag *tag):
Object(reinterpret_cast<git_object*>(tag))
{
}

Tag::Tag(const Tag& other):
Object(other)
{
}

Tag::~Tag()
{
}

OId Tag::oid() const
{
    return OId(git_tag_id(data()));
}

Object Tag::target() const
{
    git_object *obj;
	Exception::assert(git_tag_target(&obj, data()));
    return Object(obj);
}

OId Tag::targetOid() const
{
	 return OId(git_tag_target_oid(data()));
}

Object::Type Tag::targetType() const
{
	return (Object::Type) git_tag_type(data());
}

std::string Tag::name() const
{
    return git_tag_name(data());
}

Signature Tag::tagger() const
{
    return Signature(git_tag_tagger(data()));
}

std::string Tag::message()
{
    return git_tag_message(data());
}

Object Tag::peel()
{
	git_object *obj;
	Exception::assert(git_tag_peel(&obj, data()));
	return Object(obj);
}

git_tag* Tag::data() const
{
    return reinterpret_cast<git_tag*>(Object::data());
}

const git_tag* Tag::constData() const
{
    return reinterpret_cast<git_tag*>(Object::data());
}



} // namespace git2

