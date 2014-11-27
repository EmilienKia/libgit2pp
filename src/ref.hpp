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

#ifndef _REF_HPP_
#define _REF_HPP_

#include <git2.h>

#include <memory>

namespace git2
{

class OId;
class Repository;
class RefLog;
class RefLogEntry;
class Signature;

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
     */
    OId target() const;
    
    /**
     * Return the peeled OID target of this reference.
	 *
	 * This peeled OID only applies to direct references that point to
	 * a hard Tag object: it is the result of peeling such Tag.
	 */
	OId peeledTarget() const;

    /**
     * Get the full name of a reference
     *
     * @return the full name for the ref
     */
    std::string name() const;

	/**
	 * Get full name to the reference pointed to by a symbolic reference.
	 *
	 * Only available if the reference is symbolic.
	 */
	std::string symbolicTarget()const;

	/**
	 * Get the type of a reference.
	 * 
	 * Either direct (GIT_REF_OID) or symbolic (GIT_REF_SYMBOLIC)
	 */
	git_ref_t type()const;

    /**
     * Return true if the reference is direct (i.e. a reference to an OID)
     */
    bool isDirect() const;

    /**
     * Return true if the reference is symbolig (i.e. a reference to another ref)
     */
    bool isSymbolic() const;

	/**
	 * Check if a reference is a local branch.
	 * 
     * @return true when the reference lives in the refs/heads namespace.
     */
	bool isBranch() const;
	
	/**
	 * Check if a reference is a remote tracking branch.
	 * 
     * @return true when the reference lives in the refs/remotes namespace.
     */
	bool isRemote() const;
	
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
	 * Create a new reference with the same name as the given reference but a
	 * different symbolic target. The reference must be a symbolic reference,
	 * otherwise this will fail.
	 *
	 * The new reference will be written to disk, overwriting the given reference.
	 *
	 * The target name will be checked for validity.
	 * 
	 * @param target The new target for the reference
	 * @return the new reference.
	 */
	Reference setSymbolicTarget(const std::string& target);

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

	/**
	 * Rename an existing reference
	 *
	 * This method works for both direct and symbolic references.
	 * The new name will be checked for validity and may be
	 * modified into a normalized form.
	 *
	 * The reference will be immediately renamed in-memory
	 * and on disk.
	 *
	 * If the `force` flag is not enabled, and there's already
	 * a reference with the given name, the renaming will fail.
	 *
	 * IMPORTANT:
	 * The user needs to write a proper reflog entry if the
	 * reflog is enabled for the repository. We only rename
	 * the reflog if it exists.
	 *
	 * @param name The new name for the reference
	 * @param force Overwrite an existing reference
	 * @throws Exception
	 */
	void rename(const std::string name, bool force = false);

	/**
	 * Delete an existing reference
	 *
	 * This method works for both direct and symbolic references.
	 *
	 * The reference will be immediately removed on disk and from
	 * memory. The given reference pointer will no longer be valid.
	 * 
	 * @throws Exception
	 */
	void deleteReference();
	
    bool isNull() const;

	/**
	 * Compare two references.
	 */
	int compare(const Reference& ref)const;


	/**
	 * Read the reflog for the given reference
	 *
	 * @return The reflog.
	 * @throws Exception
	 */
	RefLog* readRefLog();

	/**
	 * Rename the reflog for the given reference
	 * 
	 * @param name New reflog name.
	 * @throws Exception
	 */
	void renameRefLog(const std::string name);
 
	/**
	 * Delete the reflog for the given reference
	 *
	 * @throws Exception
	 */
	void deleteRefLog();
	
	/**
	 * Ensure the reference name is well-formed.
	 * 
	 * Valid reference names must follow one of two patterns:
	 *   - Top-level names must contain only capital letters and underscores,
	 * and must begin and end with a letter. (e.g. "HEAD", "ORIG_HEAD").
	 *   - Names prefixed with "refs/" can be almost anything.
	 * You must avoid the characters '~', '^', ':', ' \ ', '?', '[', and '*',
	 * and the sequences ".." and " @ {" which have special meaning to revparse.
	 * 
     * @param name name to be checked.
     * @return true if the reference name is acceptable
     */
	static bool isValidName(const std::string& name);
	
	/**
	 * Normalize reference name and check validity.
	 * 
	 * This will normalize the reference name by removing any leading slash '/' 
	 * characters and collapsing runs of adjacent slashes between name
	 * components into a single slash.
	 * 
	 * Once normalized, if the reference name is valid, it will be returned.
	 * 
     * @param name Reference name to be checked.
	 * @param flags Flags to constrain name validation rules
     * @return Normalized name if valid.
     */
	static std::string normalizeName(const std::string& name, unsigned int flags=GIT_REF_FORMAT_NORMAL);

    git_reference* data() const;
    const git_reference* constData() const;

private:
    typedef std::shared_ptr<git_reference> ptr_type;
    ptr_type _ref;
};

/**
 * Compare two references.
 */
bool operator == (const Reference& ref1, const Reference& ref2);
bool operator != (const Reference& ref1, const Reference& ref2);
bool operator <= (const Reference& ref1, const Reference& ref2);
bool operator >= (const Reference& ref1, const Reference& ref2);
bool operator < (const Reference& ref1, const Reference& ref2);
bool operator > (const Reference& ref1, const Reference& ref2);


// Todo implement Reference iterator functions


/**
 * Represents a Git reflog.
 */
class RefLog
{
public:
    /**
     * Create an new reflog object
     */	
	RefLog(git_reflog* reflog = NULL);

    /**
     * Copy constructor
     */	
	RefLog(const RefLog& reflog);

	/**
	 * Destructor.
	 */
	~RefLog();

	/**
	 * Add a new entry to the in-memory reflog.
	 *
	 * @param id the OID the reference is now pointing to
	 * @param committer the signature of the committer
	 * @param msg the reflog message
	 */
	void append(const OId& id, const Signature& commiter, const std::string& msg);

	/**
	 * Remove an entry from the reflog by its index.
	 * 
	 * To ensure there's no gap in the log history, set rewrite param value to true.
	 * When deleting entry n, member old_oid of entry n-1 (if any) will
	 * be updated with the value of member new_oid of entry n+1.
	 * 
	 * @param idx the position of the entry to remove.
	 * Should be greater than or equal to 0 (zero) and less than getEntryCount.
	 * @param rewrite true to rewrite the history.
	 */
	void drop(size_t idx, bool rewrite);

	/**
	 * Write an existing in-memory reflog object back to disk using an atomic file lock.
	 */
	void write();

	/**
	 * Get the number of log entries in a reflog
	 * 
     * @return the number of log entries
	 */
	unsigned int getEntryCount();

	/**
	 * Lookup an entry by its index
	 *
	 * @param idx The position to lookup
	 * @return The entry; NULL if not found
	 */
	RefLogEntry* getEntry(size_t idx);
	
	
	
	git_reflog* data() const;
    const git_reflog* constData() const;

private:
    typedef std::shared_ptr<git_reflog> ptr_type;
    ptr_type _reflog;
};

/**
 * Represent a Git reflog entry.
 */
class RefLogEntry
{
public:
    /**
     * Create an new reflog entry object
     */	
	RefLogEntry(const git_reflog_entry* entry);

    /**
     * Copy constructor
     */	
	RefLogEntry(const RefLogEntry& entry);

	/**
	 * Destructor.
	 */
	~RefLogEntry();

	/**
	 * Get the old oid
	 *
	 * @return the old oid
	 */
	OId getOldOId() const;

	/**
	 * Get the new oid
	 *
	 * @return the new oid at this time
	 */
	OId getNewOId() const;

	/**
	 * Get the committer of this entry
	 *
	 * @return the committer
	 */
	Signature* getCommitter() const;

	/**
	 * Get the log msg
	 *
	 * @return the log msg
	 */
	std::string getEntryMessage() const;
	
	const git_reflog_entry * data()const;
private:
	const git_reflog_entry *_entry; 
};

} // namespace git2
#endif // _REF_HPP_

