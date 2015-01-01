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

#ifndef _GIT2PP_OBJECT_HPP_
#define _GIT2PP_OBJECT_HPP_

#include <git2.h>

#include <memory>
#include <string>
#include <vector>

#include "common.hpp"

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
class Object : public helper::Git2PtrWrapper<git_object, git_object_free>
{
public:

	enum Type {
		ANY    = GIT_OBJ_ANY,		/**< Object can be any of the following */
		BAD    = GIT_OBJ_BAD,		/**< Object is invalid. */
		COMMIT = GIT_OBJ_COMMIT,	/**< A commit object. */
		TREE   = GIT_OBJ_TREE,		/**< A tree (directory listing) object. */
		BLOB   = GIT_OBJ_BLOB,		/**< A file revision object. */
		TAG    = GIT_OBJ_TAG,		/**< An annotated tag object. */
		EXT1   = GIT_OBJ__EXT1,		/**< Reserved for future use. */
		EXT2   = GIT_OBJ__EXT2,		/**< Reserved for future use. */
		OFS_DELTA = GIT_OBJ_OFS_DELTA, /**< A delta, base is given by an offset. */
		REF_DELTA = GIT_OBJ_REF_DELTA, /**< A delta, base is given by object id. */
	};
	
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
	 * Check the type of this object.
	 * 
	 * @param type Expected type of object.
	 * @return true if the object is of expected type.
	 */
	bool checkType(Type type) const;

	/**
	 * Retrieve the type of this object.
	 * 
	 * @return Type of the object.
	 */
	Object::Type getType() const;

	/**
	 * Retrieve the type of object in string representation form.
	 * 
	 * @return Type of the object.
	 */
	std::string getTypeString() const;

	/**
	 * Convert type to its string representation.
	 * 
	 * @param type Type to convert.
	 * @returns String representation of type.
	 */
	static std::string type2String(Object::Type type);

	/**
	 * Convert a string representation of type to its type.
	 * 
	 * @param str String representation to convert.
	 * @returns Type.
	 */
	static Object::Type type2String(std::string str);

	/**
	 * Determine if the given git_otype is a valid loose object type.
	 * 
	 * @param type object type to test.
	 * @return true if the type represents a valid loose object type,
	 * false otherwise.
	 */
	static bool isLooseType(Object::Type type);


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
};

} // namespace git2
#endif // _GIT2PP_OBJECT_HPP_

