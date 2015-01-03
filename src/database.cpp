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

#include "database.hpp"

#include <git2/odb_backend.h>

#include "exception.hpp"

namespace git2
{


//
// DatabaseBackend
//

DatabaseBackend::DatabaseBackend(git_odb_backend *dbb):
_dbb(dbb)
{
}

DatabaseBackend::DatabaseBackend(const DatabaseBackend&)
{
}

DatabaseBackend::~DatabaseBackend()
{
}

DatabaseBackend DatabaseBackend::loose(const std::string& objectsDir, int compressionLevel, bool doFsync)
{
	git_odb_backend* dbb;
	Exception::git2_assert( git_odb_backend_loose(&dbb, objectsDir.c_str(), compressionLevel, doFsync?1:0) );
	return DatabaseBackend(dbb);
}

DatabaseBackend DatabaseBackend::onePack(const std::string& indexFile)
{
	git_odb_backend* dbb;
	Exception::git2_assert( git_odb_backend_one_pack(&dbb, indexFile.c_str()) );
	return DatabaseBackend(dbb);
}

DatabaseBackend DatabaseBackend::pack(const std::string& objectDir)
{
	git_odb_backend* dbb;
	Exception::git2_assert( git_odb_backend_pack(&dbb, objectDir.c_str()) );
	return DatabaseBackend(dbb);
}


git_odb_backend* DatabaseBackend::data() const
{
    return _dbb;
}

const git_odb_backend* DatabaseBackend::constData() const
{
    return _dbb;
}

//
// DatabaseObject
//

DatabaseObject::DatabaseObject(git_odb_object* obj):
_Class(obj)
{
}

DatabaseObject::DatabaseObject(const DatabaseObject& other):
_Class(other.data())
{
}

size_t DatabaseObject::size()
{
	return git_odb_object_size(data());
}

const void* DatabaseObject::raw()
{
	return git_odb_object_data(data());
}

OId DatabaseObject::oid()
{
	return OId(git_odb_object_id(data()));
}

git_otype DatabaseObject::type()
{
	return git_odb_object_type(data());
}

//
// Database
//

Database::Database()
{
	git_odb *odb;
	Exception::git2_assert( git_odb_new(&odb) );
	_db = odb;
}

Database::Database(git_odb *odb):
_db(odb)
{
}

Database::Database( const Database& db )
{
    _db = db._db;
}

Database::~Database()
{
}

Database Database::open(const std::string& objectsDir)
{
	git_odb *db;
    Exception::git2_assert( git_odb_open(&db, objectsDir.c_str()) );
    return Database(db);
}

void Database::close()
{
    git_odb_free(_db);
}

void Database::refresh()
{
	Exception::git2_assert( git_odb_refresh(data()) );
}

void Database::addBackend(DatabaseBackend *backend, int priority)
{
    Exception::git2_assert( git_odb_add_backend(_db, (git_odb_backend *)backend, priority) );
}

void Database::addAlternate(DatabaseBackend *backend, int priority)
{
    Exception::git2_assert( git_odb_add_alternate(_db, (git_odb_backend *)backend, priority) );
}

void Database::addDiskAlternate(const std::string& path)
{
    Exception::git2_assert( git_odb_add_disk_alternate(_db, path.c_str()) );
}

int Database::exists(const OId& id)
{
    return git_odb_exists(_db, id.constData());
}

size_t Database::getNumBackends()
{
	return git_odb_num_backends(data());
}

DatabaseBackend Database::getBackend(size_t pos)
{
	git_odb_backend * dbb;
	Exception::git2_assert( git_odb_get_backend(&dbb, data(), pos) );
	return DatabaseBackend(dbb);
}

OId Database::hash(const void* data, size_t len, git_otype type)
{
	git_oid oid;
	Exception::git2_assert( git_odb_hash(&oid, data, len, type) );
	return OId(&oid);
}

OId Database::hashFile(const std::string& path, git_otype type)
{
	git_oid oid;
	Exception::git2_assert( git_odb_hashfile(&oid, path.c_str(), type) );
	return OId(&oid);
}

DatabaseObject Database::read(OId oid)
{
	git_odb_object *obj;
	Exception::git2_assert( git_odb_read(&obj, data(), oid.constData()) );
	return DatabaseObject(obj);
}

OId Database::write(const void* data, size_t len, git_otype type)
{
	git_oid oid;
	Exception::git2_assert( git_odb_write(&oid, this->data(), data, len, type) );
	return OId(&oid);
}


git_odb* Database::data() const
{
    return _db;
}


} // namespace git2

