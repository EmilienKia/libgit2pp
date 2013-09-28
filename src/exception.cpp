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

#include "exception.hpp"

namespace git2
{

Exception::Exception()
{
	const git_error *err = giterr_last();
	if (err != NULL) {
		_msg = err->message;
		giterr_clear();
	}
}

Exception::~Exception() throw()
{
}

const char * Exception::what() const throw()
{
	return _msg.c_str();
}

std::string Exception::message() const throw()
{
	return _msg;
}

int Exception::assert(int ret)
{
	if(ret<0)
	{
		throw Exception();
	}
	return ret;
}

} // namespace git2

