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

#include "blob.hpp"


namespace git2
{

Blob::Blob(git_blob *blob):
Object(reinterpret_cast<git_object*>(blob))
{
}

Blob::Blob( const Blob& other ):
Object(other)
{
}

Blob::~Blob()
{
}

const void* Blob::rawContent() const
{
    return git_blob_rawcontent(data());
}

std::vector<unsigned char> Blob::content() const
{
    return std::vector<unsigned char>( static_cast<const char *>(rawContent()), static_cast<const char *>(rawContent())+rawSize() );
}

int Blob::rawSize() const
{
    return git_blob_rawsize(data());
}

git_blob* Blob::data() const
{
    return reinterpret_cast<git_blob*>(Object::data());
}

const git_blob* Blob::constData() const
{
    return reinterpret_cast<git_blob*>(Object::data());
}

} // namespace git2

