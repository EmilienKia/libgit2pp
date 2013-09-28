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

// TODO Only available from v0.18.0
/*std::string Reference::symbolicTarget() const
{
    return std::string(git_reference_symbolic_target(_ref.get()));
}*/

bool Reference::isDirect() const
{
    return git_reference_type(_ref.get()) == GIT_REF_OID;
}

bool Reference::isSymbolic() const
{
    return git_reference_type(_ref.get()) == GIT_REF_SYMBOLIC;
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

// TODO Only available from v0.18.0
/*void Reference::setSymbolicTarget(const std::string& target)
{
    git_reference* rp;
    Exception::assert(git_reference_symbolic_set_target(&rp, data(), target.c_str()));
    _ref = ptr_type(rp);
}*/

void Reference::setTarget(const OId& oid)
{
    Exception::assert(git_reference_set_target(data(), oid.format().c_str()));
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


} // namespace git2

