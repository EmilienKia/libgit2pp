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
#include <git2/trace.h>

#include <memory>

namespace git2
{
	

/**
 * @name Traces
 * @{
 */

typedef std::function<void(git_trace_level_t level, const std::string& msg)> TraceCallback;
 
/**
 * Sets the system tracing configuration to the specified level with the
 * specified callback.  When system events occur at a level equal to, or
 * lower than, the given level they will be reported to the given callback.
 * 
 * @param level Level to set tracing to
 * @param cb Function to call with trace data
 */
void setTraceCallback(git_trace_level_t level, TraceCallback cb);
 
/** @} */
	
	
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
	typedef std::shared_ptr<_Type> _PtrType;

	Git2PtrWrapper(){}
	Git2PtrWrapper(_Type* ptr):_ptr(ptr, _Deleter){} // Todo protect deleter from null pointer
	Git2PtrWrapper(const _Class& other):_ptr(other._ptr){}
	~Git2PtrWrapper(){}

	bool ok()const{return _ptr.get()!=nullptr;}
	_Type* data() const {return _ptr.get();}
	_PtrType& ptr() const {return _ptr;}

private:
    _PtrType _ptr;
};

/**
 * Helper to easily fill and free git_strarray from string container.
 * The git_strarray should be filled as long as the filler is OK.
 * @tparam StringContainer Should be a container of std::string like a std::vector<std::string>
 */
template<class StringContainer>
class StrArrayFiller
{
public:
	StrArrayFiller(git_strarray* array, const StringContainer& container):
	_array(array),
	_container(container)
	{
		fill();
	}
	
	~StrArrayFiller()
	{
		free();
	}

private:
	StrArrayFiller(){}
	
	void fill()
	{
		_array->count   = _container.size();
		_array->strings = new char*[_container.size()];
		size_t n=0;
		for(const std::string& val : _container)
		{
			_array->strings[n++] = const_cast<char*>(val.c_str());
		}
	}
	
	void free()
	{
		if(_array->strings!=nullptr)
		{
			delete _array->strings;
			_array->strings = nullptr;
		}
		_array->count = 0;
	}

	git_strarray* _array;
	const StringContainer& _container;
};


/**
 * Helper function to push-back strings from git_strarray to string container.
 * The string container must be of type container<std::string> with push_back method, like std::vector<std::string>>
 */
template<class StringContainer>
void push_back(StringContainer& container, const git_strarray* array)
{
	if(array!=nullptr && array->count>0 && array->strings!=nullptr)
	{
		for(size_t n=0; n<array->count; ++n)
		{
			container.push_back(std::string(array->strings[n]));
		}
	}
}


} // namespace helper
} // namespace git2

#endif // _GIT2PP_COMMON_HPP_

