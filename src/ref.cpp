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

#include "ref.hpp"

#include "exception.hpp"
#include "oid.hpp"
#include "repository.hpp"

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
// Reference
//

Reference::Reference(git_reference *ref):
_ref(ref, GitReferenceDeleter())
{
}

Reference::Reference(const Reference& ref):
_ref(ref._ref)
{
}

Reference::~Reference()
{
}

OId Reference::target() const
{
	OId oid;
	oid.fromString(git_reference_target(_ref.get()));
    return oid;
}

std::string Reference::symbolicTarget() const
{
    return std::string(git_reference_target(_ref.get()));
}

bool Reference::isDirect() const
{
    return git_reference_type(_ref.get()) == GIT_REF_OID;
}

bool Reference::isSymbolic() const
{
    return git_reference_type(_ref.get()) == GIT_REF_SYMBOLIC;
}

bool Reference::isPacked() const
{
	return git_reference_is_packed(data()) == 1;
}

std::string Reference::name() const
{
    return std::string(git_reference_name(_ref.get()));
}

Reference Reference::resolve() const
{
    git_reference *ref;
    Exception::assert(git_reference_resolve(&ref, _ref.get()));
    return Reference(ref);
}

Repository Reference::owner() const
{
    return Repository(git_reference_owner(_ref.get()));
}

void Reference::setSymbolicTarget(const std::string& target)
{
    Exception::assert(git_reference_set_target(data(), target.c_str()));
}

void Reference::setTarget(const OId& oid)
{
    Exception::assert(git_reference_set_target(data(), oid.format().c_str()));
}

void Reference::rename(const std::string name, bool force)
{
	Exception::assert(git_reference_rename(data(), name.c_str(), force?1:0));
}

void Reference::deleteReference()
{
	Exception::assert(git_reference_delete(data()));
	_ref.reset();
}

void Reference::reload()
{
	Exception::assert(git_reference_reload(data()));
}

bool Reference::isNull() const
{
    return data() == NULL;
}

git_reference* Reference::data() const
{
    return _ref.get();
}

const git_reference* Reference::constData() const
{
    return _ref.get();
}


bool operator == (const Reference& ref1, const Reference& ref2)
{
	return git_reference_cmp(ref1.data(), ref2.data()) == 0;
}

bool operator != (const Reference& ref1, const Reference& ref2)
{
	return git_reference_cmp(ref1.data(), ref2.data()) != 0;
}


} // namespace git2

