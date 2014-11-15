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

#ifndef _BLOB_HPP_
#define _BLOB_HPP_

#include <git2.h>

#include "object.hpp"

namespace git2
{

/**
 * Representation of a Git Blob object.
 */
class Blob : public Object
{
public:
    /**
     * Create an Blob.
     *
     * Creates a Blob that points to the given git_blob.
     */
    explicit Blob(git_blob *blob = NULL);

    /**
     * Copy constructor.
     */
    Blob(const Blob& other);

    /**
     * Destroy the object.
     */
    ~Blob();

    /**
     * Determine if the blob content is most certainly binary or not.
     */
    bool isBinary() const;

    /**
     * Get the raw content of this blob.
     *
     * This method returns a read-only buffer with the raw content
     * of the blob. This pointer is automatically freed when there
     * are no more objects referencing it. The pointer may be
     * invalidated at a later time (e.g. when changing the contents
     * of the blob).
     *
     * @return the pointer; NULL if the blob has no contents
     */
    const void* rawContent() const;

    /**
      * @return The blob content as vector.
      */
    std::vector<unsigned char> content() const;

    /**
     * Get the size in bytes of the contents of a blob
     *
     * @return size on bytes
     */
    int64_t rawSize() const;

    git_blob* data() const;
    const git_blob* constData() const;
};

} // namespace git2
#endif // _BLOB_HPP_

