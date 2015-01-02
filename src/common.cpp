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

#include "common.hpp"

#include "exception.hpp"

namespace git2
{

static TraceCallback trace_callback(nullptr);

static void trace_cb(git_trace_level_t level, const char* msg)
{
	trace_callback(level, std::string(msg));
}

void setTraceCallback(git_trace_level_t level, TraceCallback cb)
{
	trace_callback = cb;
	Exception::git2_assert(git_trace_set(level, trace_cb));
}

} // namespace git2

