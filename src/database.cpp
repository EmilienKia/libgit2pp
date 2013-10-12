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

namespace git2
{


//
// DatabaseBackend
//

DatabaseBackend::DatabaseBackend()
{
}

DatabaseBackend::DatabaseBackend(const DatabaseBackend&)
{
}

DatabaseBackend::~DatabaseBackend()
{
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
// Database
//


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

int Database::open(const std::string& objectsDir)
{
    return git_odb_open(&_db, objectsDir.c_str());
}

void Database::close()
{
    return git_odb_free(_db);
}

int Database::addBackend(DatabaseBackend *backend, int priority)
{
    return git_odb_add_backend(_db, (git_odb_backend *)backend, priority);
}

int Database::addAlternate(DatabaseBackend *backend, int priority)
{
    return git_odb_add_alternate(_db, (git_odb_backend *)backend, priority);
}

int Database::exists(const OId& id)
{
    return git_odb_exists(_db, id.constData());
}

git_odb* Database::data() const
{
    return _db;
}

const git_odb* Database::constData() const
{
    return _db;
}


} // namespace git2

