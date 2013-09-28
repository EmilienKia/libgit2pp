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

#ifndef _TAG_HPP_
#define _TAG_HPP_

#include <git2.h>

#include "object.hpp"

namespace git2
{

class OId;
class Signature;

/**
 * Represents a Git tag object.
 */
class Tag : public Object
{
public:
    /**
     * Creates a Tag that points to tag. The pointer object becomes managed by
     * this Tag, and must not be passed to another Tag or closed outside this
     * object.
     */
    explicit Tag(git_tag *tag = 0);

    /**
     * Copy constructor; creates a copy of the object, sharing the same underlaying data
     * structure.
     */
    Tag(const Tag& other);

    /**
     * Destroys the object.
     */
    ~Tag();

    /**
     * Get the id of a tag.
     * @return object identity for the tag.
     */
    OId oid() const;

    /**
     * Get the tagged object of a tag
     *
     * @throws LibQGit2::Exception
     */
    Object target() const;

    /**
     * Get the name of a tag
     * @return name of the tag
     */
    std::string name() const;

    /**
     * Get the tagger (author) of a tag
     * @return reference to the tag's author
     */
    Signature tagger() const;

    /**
     * Get the message of a tag
     * @return message of the tag
     */
    std::string message();

    git_tag* data() const;
    const git_tag* constData() const;
};

} // namespace git2
#endif // _TAG_HPP_

