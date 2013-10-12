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

#include "oid.hpp"

#include "exception.hpp"

namespace git2
{


OId::OId(const git_oid *oid):
_oid((size_t)GIT_OID_RAWSZ, (unsigned char)0)
{
	if(oid!=NULL)
	{
		_oid.assign((unsigned char*)oid, (unsigned char*)oid+GIT_OID_RAWSZ);
	}
}


OId::OId(const OId& oid):
_oid(oid._oid)
{
}

OId::~OId()
{
}

bool OId::isValid() const
{
	return ( !_oid.empty() &&
	         (_oid.size() <= GIT_OID_RAWSZ) &&
	         (_oid != std::vector<unsigned char>((size_t)GIT_OID_RAWSZ, (unsigned char)0))
	         );
}

void OId::fromHex(const std::vector<char>& hex)
{
	size_t len = std::min(hex.size(), (size_t)GIT_OID_HEXSZ);
	 Exception::assert(git_oid_fromstrn(data(), &hex.front(), len));
	_oid.resize(len / 2);
}

void OId::fromString(const std::string& str)
{
	size_t len = std::min(str.size(), (size_t)GIT_OID_HEXSZ);
	 Exception::assert(git_oid_fromstrn(data(), str.c_str(), len));
	_oid.resize(len / 2);
}

void OId::fromRawData(const std::vector<unsigned char>& raw)
{
	if(raw.size()<GIT_OID_RAWSZ)
		_oid = raw;
	else
		_oid.assign(raw.begin(), raw.begin()+GIT_OID_RAWSZ);
}

bool OId::isZero() const
{
	return git_oid_iszero(constData());
}

OId OId::hexToOid(const std::vector<char>& hex)
{
	OId oid;
	oid.fromHex(hex);
	return oid;
}

OId OId::stringToOid(const std::string& str)
{
	OId oid;
	oid.fromString(str);
	return oid;
}

OId OId::rawDataToOid(const std::vector<unsigned char>& raw)
{
	OId oid;
	oid.fromRawData(raw);
	return oid;		
}

std::string OId::format() const
{
	std::vector<char> buffer((size_t)GIT_OID_HEXSZ+1, (char)0);
	git_oid_fmt(buffer.data(), constData());
	return std::string(buffer.data());
}

std::string OId::pathFormat() const
{
	std::vector<char> buffer((size_t)GIT_OID_HEXSZ+1, (char)0);
	git_oid_pathfmt(buffer.data(), constData());
	return std::string(buffer.data());
}

git_oid* OId::data()
{
	return reinterpret_cast<git_oid*>(&_oid.front());
}
	
const git_oid* OId::constData() const
{
	return reinterpret_cast<const git_oid*>(&_oid.front());
}

int OId::length() const
{
	return _oid.size() * 2;
}

bool operator ==(const OId &oid1, const OId &oid2)
{
	return git_oid_cmp(oid1.constData(), oid2.constData()) == 0;
}

bool operator !=(const OId &oid1, const OId &oid2)
{
	return git_oid_cmp(oid1.constData(), oid2.constData()) != 0;
}

bool operator >(const OId &oid1, const OId &oid2)
{
	return git_oid_cmp(oid1.constData(), oid2.constData()) > 0;
}

bool operator <(const OId &oid1, const OId &oid2)
{
	return git_oid_cmp(oid1.constData(), oid2.constData()) < 0;
}

bool operator >=(const OId &oid1, const OId &oid2)
{
	return git_oid_cmp(oid1.constData(), oid2.constData()) >= 0;
}

bool operator <=(const OId &oid1, const OId &oid2)
{
	return git_oid_cmp(oid1.constData(), oid2.constData()) <= 0;
}

bool operator == (const OId &oid, const std::string &str)
{
	return git_oid_streq(oid.constData(), str.c_str()) != 0;
}

bool operator == (const OId &oid, const char* str)
{
	return git_oid_streq(oid.constData(), str) != 0;
}



} // namespace git2
