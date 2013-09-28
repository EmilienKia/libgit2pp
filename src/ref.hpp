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

#ifndef _REF_HPP_
#define _REF_HPP_

#include <git2.h>

#include <memory>

namespace git2
{

class OId;
class Repository;

/**
* Represents a Git reference.
* Reference objects are branches, tags, etc.
*/
class Reference
{
public:

    /**
     * Create an new ref object
     */
    Reference(git_reference *ref = NULL);

    /**
     * Copy constructor
     */
    Reference(const Reference& other);

    /**
     * Free an existing reference object.
     */
    ~Reference();

    /**
     * Get the OID pointed to by a reference.
     *
     * Only available if the reference is direct (i.e. not symbolic)
     *
     * @return a pointer to the oid if available, NULL otherwise
     */
    OId target() const;

    /**
     * Get full name to the reference pointed by this reference
     *
     * Only available if the reference is symbolic
     *
     * @return a pointer to the name if available, NULL otherwise
     */
// TODO Only available from v0.18.0
//    std::string symbolicTarget() const;

    /**
     * Return true if the reference is direct (i.e. a reference to an OID)
     */
    bool isDirect() const;

    /**
     * Return true if the reference is symbolig (i.e. a reference to another ref)
     */
    bool isSymbolic() const;

    /**
     * Get the full name of a reference
     *
     * @return the full name for the ref
     */
    std::string name() const;

    /**
     * Resolve a symbolic reference
     *
     * Thie method iteratively peels a symbolic reference
     * until it resolves to a direct reference to an OID.
     *
     * If a direct reference is passed as an argument,
     * that reference is returned immediately
     *
     * @param resolvedRef Pointer to the peeled reference
     * @return 0 on success; error code otherwise
     * @throws Exception
     */
    Reference resolve() const;

    /**
     * Write a reference back to disk.
     *
     * The reference must have a valid name and a valid target
     * (either direct or symbolic).
     *
     * If the reference has been loaded from disk and no changes
     * have been made, no action will take place.
     *
     * The writing to disk is atomic.
     *
     * @return 0 on success; error code otherwise
     */
    int write();

    /**
     * Get the repository where a reference resides
     *
     * @return a pointer to the repository
     */
    Repository owner() const;

    /**
     * Set the name of a reference.
     *
     * This marks the reference as modified; changes
     * won't take effect until it is manually written back
     * to disk.
     *
     * @param name The new name for the reference
     */
    void setName(const std::string& name);

    /**
     * Set the target reference of a reference.
     *
     * This converts the reference into a symbolic
     * reference.
     *
     * This marks the reference as modified; changes
     * won't take effect until it is manually written back
     * to disk.
     *
     * @param target The new target for the reference
     * @throws Exception
     */
// TODO Only available from v0.18.0
//    void setSymbolicTarget(const std::string& target);

    /**
     * Set the OID target of a reference.
     *
     * This converts the reference into a direct
     * reference.
     *
     * This marks the reference as modified; changes
     * won't take effect until it is manually written back
     * to disk.
     *
     * @param target The new target OID for the reference
     * @throws Exception
     */
    void setTarget(const OId& oid);

    bool isNull() const;

    git_reference* data() const;
    const git_reference* constData() const;

private:
    typedef std::shared_ptr<git_reference> ptr_type;
    ptr_type _ref;
};

} // namespace git2
#endif // _REF_HPP_

