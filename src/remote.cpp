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

#include "remote.hpp"

#include "exception.hpp"

namespace git2
{

//
// RefSpec
//

	RefSpec::RefSpec(const git_refspec *refspec):
_refspec(refspec)
{
}

RefSpec::~RefSpec()
{
}

std::string RefSpec::src()const
{
	return std::string(git_refspec_src(constData()));
}

std::string RefSpec::dst()const
{
	return std::string(git_refspec_dst(constData()));
}

bool RefSpec::matches(const std::string& refname)const
{
	return git_refspec_src_matches(constData(), refname.c_str());
}

std::string RefSpec::transform(const std::string& name)const
{
	char buffer[GIT_PATH_MAX];
	Exception::assert(git_refspec_transform(buffer, GIT_PATH_MAX, constData(), name.c_str()));
	return std::string(buffer);
}

const git_refspec *RefSpec::constData()const
{
	return _refspec;
}

//
// Remote
//

Remote::Remote(git_remote *remote):
_remote(remote)		
{
}

Remote::~Remote()
{
	if(_remote!=NULL)
	   git_remote_free(_remote);
}

void Remote::save()
{
	Exception::assert(git_remote_save(data()));
}

std::string Remote::name()
{
	return std::string(git_remote_name(data()));
}

std::string Remote::url()
{
	return std::string(git_remote_url(data()));
}

void Remote::connect(Direction direction)
{
	Exception::assert(git_remote_connect(data(), (git_direction)direction));
}

bool Remote::isConnected()
{
	return git_remote_connected(data()) != 0;
}

void Remote::disconnect()
{
	git_remote_disconnect(data());
}

bool Remote::isValidUrl(const std::string& url)
{
	return git_remote_valid_url(url.c_str()) != 0;
}

bool Remote::isSupportedUrl(const std::string& url)
{
	return git_remote_supported_url(url.c_str()) != 0;
}

git_remote* Remote::data() const
{
    return _remote;
}

const git_remote* Remote::constData() const
{
    return _remote;
}


} // namespace git2

