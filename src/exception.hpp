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

#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <git2.h>

#include <exception>
#include <string>

namespace git2
{

	class Exception: public std::exception 
	{
	public:
		Exception();
		~Exception() throw();
		const char *what() const throw();
		std::string message() const throw();

		static int assert(int ret);
	private:
		std::string _msg;
	};

} // namespace git2
#endif // _EXCEPTION_HPP_

