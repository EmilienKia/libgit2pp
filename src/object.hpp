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

#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

#include <git2.h>

#include <memory>
#include <string>
#include <vector>

namespace git2
{
	
class Blob;
class Commit;
class OId;
class Repository;
class Tag;
class Tree;


/**
 * This is the base class for every repository object, i.e. blob, commit,
 * tag and tree. Every object is identified with it's git2::OId.
 */
class Object
{
public:
    /**
     * Create an Object.
     *
     * Creates an Object that points to the given object.
     * The pointer to the underlaying git_object is managed by this
     * Object, and is automatically freed when no more referenced.
     */
    explicit Object(git_object *object = NULL);
		
    /**
     * Copy constructor.
     */
    Object(const Object& other);

    /**
     * Destroy the object.
     */
    ~Object();

    /**
     * Convert into a commit object.
     *
     * If the underlaying git_object is a commit this returns a valid
     * Commit object, otherwise it returns an empty one.
     */
    Commit toCommit() const;

    /**
     * Convert into a tag object.
     *
     * If the underlaying git_object is a tag this returns a valid
     * Tag object, otherwise it returns an empty one.
     */
    Tag toTag() const;

    /**
     * Convert into a tree object.
     *
     * If the underlaying git_object is a tree this returns a valid
     * Tag object, otherwise it returns an empty one.
     */
    Tree toTree() const;

    /**
     * Convert into a blob object.
     *
     * If the underlaying git_object is a blob this returns a valid
     * Tag object, otherwise it returns an empty one.
     */
    Blob toBlob() const;

    /**
     * Check if the pointer is null.
     *
     * Returns true if the git_object pointer owned by this
     * instance is null.
     */
    bool isNull() const;

    /**
     * Get the OId (SHA1) of a repository object.
     *
     * This returns the OId of the object.
     * Remember that in-memory objects created by git_object_new()
     * do not have a SHA1 id until they are written on a repository.
     *
     * @return the OId of the object
     */
    OId oid() const;

    /**
     * Check if this is a commit.
     *
     * Returns true if the object represents a commit; false otherwise.
     */
    bool isCommit() const;

    /**
     * Check if this is a tag.
     *
     * Returns true if the object represents a tag; false otherwise.
     */
    bool isTag() const;

    /**
     * Check if this is a tree.
     *
     * Returns true if the object represents a tree; false otherwise.
     */
    bool isTree() const;

    /**
     * Check if this is a blob.
     *
     * Returns true if the object represents a blob; false otherwise.
     */
    bool isBlob() const;

    /**
     * Get the object type as a string.
     */
    std::string typeString() const;

    /**
     * Get the repository that owns this object.
     */
    Repository owner() const;

    git_object* data() const;
    const git_object* constData() const;

private:
	std::shared_ptr<git_object> _obj;
		
};

} // namespace git2
#endif // _OBJECT_HPP_

