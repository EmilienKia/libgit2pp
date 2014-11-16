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

#include "signature.hpp"

#include "exception.hpp"

namespace git2
{

//
// SignatureBuilder
//

SignatureBuilder::SignatureBuilder(const std::string& name, const std::string& email, time_t dateTime, int offset)
{
    Exception::assert(git_signature_new(&_sign, name.c_str(), email.c_str(), dateTime, offset));
}

SignatureBuilder::SignatureBuilder(const std::string& name, const std::string& email)
{
    Exception::assert(git_signature_now(&_sign, name.c_str(), email.c_str()));
}

SignatureBuilder::SignatureBuilder(const SignatureBuilder& sign)
{
    _sign = git_signature_dup(sign._sign);
}

SignatureBuilder::~SignatureBuilder()
{
    git_signature_free(_sign);
}

std::string SignatureBuilder::name() const
{
    return std::string(_sign->name);
}

std::string SignatureBuilder::email() const
{
    return std::string(_sign->email);
}

time_t SignatureBuilder::when() const
{
	return _sign->when.time;
}

int SignatureBuilder::when_offset() const
{
	return _sign->when.offset;
}

const git_signature* SignatureBuilder::data() const
{
    return _sign;
}

//
// Signature
//

Signature::Signature(const git_signature *signature):
_sign(signature)
{
}

Signature::Signature(const Signature& other):
_sign(other.constData())
{
}

Signature::Signature(const SignatureBuilder& other):
_sign(other.data())
{
}

Signature::~Signature()
{
}

std::string Signature::name() const
{
    return std::string(_sign->name);
}

std::string Signature::email() const
{
    return std::string(_sign->email);
}

time_t Signature::when() const
{
	return _sign->when.time;
}

int Signature::when_offset() const
{
	return _sign->when.offset;
}

const git_signature *Signature::constData() const
{
    return _sign;
}


} // namespace git2

