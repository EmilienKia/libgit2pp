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

#include "ref.hpp"

#include "exception.hpp"
#include "oid.hpp"
#include "repository.hpp"
#include "signature.hpp"

namespace git2
{

//
// Reference
//

Reference::Reference(git_reference *ref):
_Class(ref)
{
}

Reference Reference::undeletable(git_reference *ref)
{
    return Reference(ref, [](git_reference*){});
}

Reference::Reference(git_reference *ref, void(*deleter)(git_reference*)):
_Class(ref, deleter)
{
}

Reference::Reference(const Reference& other):
_Class(other.data())
{
}

OId Reference::target() const
{
	return OId(git_reference_target(data()));
}

OId Reference::peeledTarget() const
{
	return OId(git_reference_target_peel(data()));
}

git_ref_t Reference::type()const
{
	return git_reference_type(data());
}

bool Reference::isDirect() const
{
    return git_reference_type(data()) == GIT_REF_OID;
}

bool Reference::isSymbolic() const
{
    return git_reference_type(data()) == GIT_REF_SYMBOLIC;
}

bool Reference::isBranch() const
{
    return git_reference_is_branch(data()) != 0;
}

bool Reference::isRemote() const
{
    return git_reference_is_remote(data());
}

bool Reference::isNote() const
{
    return git_reference_is_note(data());
}

bool Reference::isTag() const
{
    return git_reference_is_tag(data());
}

std::string Reference::name() const
{
    return std::string(git_reference_name(data()));
}

std::string Reference::shorthand() const
{
    return std::string(git_reference_shorthand(data()));
}

std::string Reference::symbolicTarget()const
{
	return std::string(git_reference_symbolic_target(data()));
}

Reference Reference::resolve() const
{
    git_reference *ref;
    Exception::git2_assert(git_reference_resolve(&ref, data()));
    return Reference(ref);
}

Reference Reference::setSymbolicTarget(const std::string& target, const std::string& logMessage)
{
	git_reference *out;
    Exception::git2_assert(git_reference_symbolic_set_target(&out, data(), target.c_str(), logMessage.c_str()));
    return Reference(out);
}

void Reference::setTarget(const OId& oid, const std::string& logMessage)
{
	git_reference *ref;
    Exception::git2_assert(git_reference_set_target(&ref, data(), oid.constData(), logMessage.c_str()));
    *this = Reference(ref);
}

void Reference::rename(const std::string name, bool force, const std::string& logMessage)
{
	git_reference *ref;
	Exception::git2_assert(git_reference_rename(&ref, data(), name.c_str(), force?1:0, logMessage.c_str()));
    *this = Reference(ref);
}

void Reference::deleteReference()
{
	Exception::git2_assert(git_reference_delete(data()));
    *this = Reference();
}

#if 0 // Removed for upgrading to 0.24.0
RefLog Reference::readRefLog()
{
    git_reflog *reflog;
    Exception::git2_assert(git_reflog_read(&reflog, data()));
    return RefLog(reflog);
}

void Reference::renameRefLog(const std::string name)
{
	Exception::git2_assert(git_reflog_rename(data(), name.c_str()));
}

void Reference::deleteRefLog()
{
	Exception::git2_assert(git_reflog_delete(data()));
}
#endif // Removed for upgrading to 0.24.0

bool Reference::isNull() const
{
    return data() == NULL;
}

int Reference::compare(const Reference& ref)const
{
	return git_reference_cmp(data(), ref.data());
}

bool Reference::isValidName(const std::string& name)
{
	return git_reference_is_valid_name(name.c_str());
}

std::string Reference::normalizeName(const std::string& name, unsigned int flags)
{
	char buffer[GIT_PATH_MAX];
	Exception::git2_assert(git_reference_normalize_name(buffer, GIT_PATH_MAX-1, name.c_str(), flags));
	return std::string(buffer);
}

bool operator == (const Reference& ref1, const Reference& ref2)
{
	return ref1.compare(ref2) == 0;
}

bool operator != (const Reference& ref1, const Reference& ref2)
{
	return ref1.compare(ref2) != 0;
}

bool operator >= (const Reference& ref1, const Reference& ref2)
{
	return ref1.compare(ref2) >= 0;
}

bool operator <= (const Reference& ref1, const Reference& ref2)
{
	return ref1.compare(ref2) <= 0;
}

bool operator > (const Reference& ref1, const Reference& ref2)
{
	return ref1.compare(ref2) > 0;
}

bool operator < (const Reference& ref1, const Reference& ref2)
{
	return ref1.compare(ref2) < 0;
}


//
// RefLog
//

RefLog::RefLog(git_reflog *reflog):
_Class(reflog)
{
}

RefLog::RefLog(const RefLog& other):
_Class(other.data())
{
}

RefLog::~RefLog()
{
}

void RefLog::append(const OId& id, const Signature& commiter, const std::string& msg)
{
	Exception::git2_assert(git_reflog_append(data(), id.constData(), commiter.data(), msg.empty()?NULL:msg.c_str()));
}

void RefLog::drop(size_t idx, bool rewrite)
{
	Exception::git2_assert(git_reflog_drop(data(), idx, rewrite?1:0));
}

void RefLog::write()
{
	Exception::git2_assert(git_reflog_write(data()));
}

unsigned int RefLog::getEntryCount()
{
	return git_reflog_entrycount(data());
}

RefLogEntry* RefLog::getEntry(size_t idx)
{
	const git_reflog_entry * entry = git_reflog_entry_byindex(data(), idx);
	if(entry!=NULL)
		return new RefLogEntry(entry);
	else
		return NULL;
}


//
// RefLogEntry
//

RefLogEntry::RefLogEntry(const git_reflog_entry* entry):
_entry(entry)
{
}

/**
 * Copy constructor
 */	
RefLogEntry::RefLogEntry(const RefLogEntry& entry):
_entry(entry._entry)
{
}

/**
 * Destructor.
 */
RefLogEntry::~RefLogEntry()
{
}

OId RefLogEntry::getOldOId() const
{
	return OId(git_reflog_entry_id_old(data()));
}

OId RefLogEntry::getNewOId() const
{
	return OId(git_reflog_entry_id_new(data()));
}

Signature* RefLogEntry::getCommitter() const
{
	const git_signature * sign = git_reflog_entry_committer(data());
	if(sign!=NULL)
		return new Signature(sign);
	else
		return NULL;
}

std::string RefLogEntry::getEntryMessage() const
{
	return std::string(git_reflog_entry_message(data()));
}

const git_reflog_entry * RefLogEntry::data()const
{
	return _entry;
}

} // namespace git2

