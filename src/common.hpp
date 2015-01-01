/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * libgit2pp
 * Copyright (C) 2013-2014 Émilien Kia <emilien.kia@gmail.com>
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

#ifndef _GIT2PP_COMMON_HPP_
#define _GIT2PP_COMMON_HPP_

#include <git2.h>

#include <memory>

namespace git2
{
namespace helper
{

/**
 * Represents a Git wrapped structure pointer.
 */
template<class _Type, void(*_Deleter)(_Type*) = nullptr>
class Git2PtrWrapper
{
public:
	typedef Git2PtrWrapper<_Type,_Deleter> _Class;

	Git2PtrWrapper(){}
	Git2PtrWrapper(_Type* ptr):_ptr(ptr, _Deleter){} // Todo protect deleter from null pointer
	Git2PtrWrapper(const _Class& other):_ptr(other._ptr){}

	bool ok()const{return _ptr.get()!=nullptr;}
	_Type* data() const {return _ptr.get();}

private:
    typedef std::shared_ptr<_Type> ptr_type;
    ptr_type _ptr;
};

} // namespace helper
} // namespace git2

#endif // _GIT2PP_COMMON_HPP_

