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

#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <git2.h>

#include "oid.hpp"

#include <string>

namespace git2
{

class DatabaseBackend;
class Database;

/**
 * Represents a Git object database backend.
 */
class DatabaseBackend
{
    public:
        DatabaseBackend();

        DatabaseBackend( const DatabaseBackend& dbb );

        ~DatabaseBackend();

		// TODO Should give easy access to backend functions ?
		
        git_odb_backend* data() const;
        const git_odb_backend* constData() const;
    private:
        git_odb_backend *_dbb;
};

/**
 * Represents a Git object database containing unique sha1 object ids.
 */
class Database
{
public:
    /**
     * Create a new object database with no backends.
     *
     * Before the ODB can be used for read/writing, a custom database
     * backend must be manually added using `addBackend()`
     *
     */
    Database( git_odb *odb = NULL);

    Database( const Database& db );

    ~Database();

    /**
    * Create a new object database and automatically add
    * the two default backends:
    *
    * - backendLoose: read and write loose object files
    * from disk, assuming `objects_dir` as the Objects folder
    *
    * - backendPack: read objects from packfiles,
    * assuming `objectsDir` as the Objects folder which
    * contains a 'pack/' folder with the corresponding data
    *
    * @param objectsDir path of the backends' "objects" directory.
    * @return GIT_SUCCESS if the database opened; otherwise an error
    * code describing why the open was not possible.
    */
    int open(const std::string& objectsDir);

    /**
     * Close an open object database.
     */
    void close();

    /**
     * Add a custom backend to an existing Object DB
     *
     * Read <odb_backends.h> for more information.
     *
     * @param backend pointer to a databaseBackend instance
     * @return 0 on sucess; error code otherwise
     */
    int addBackend(DatabaseBackend *backend, int priority);

    /**
    * Add a custom backend to an existing Object DB; this
    * backend will work as an alternate.
    *
    * Alternate backends are always checked for objects *after*
    * all the main backends have been exhausted.
    *
    * Writing is disabled on alternate backends.
    *
    * Read <odb_backends.h> for more information.
    *
    * @param backend pointer to a databaseBackend instance
    * @return 0 on sucess; error code otherwise
    */
    int addAlternate(DatabaseBackend *backend, int priority);

    /**
     * Determine if the given object can be found in the object database.
     *
     * @param id the object to search for.
     * @return
     * - true, if the object was found
     * - false, otherwise
     */
    int exists(const OId& id);

	// TODO Should give access to ODB objects, streams and related ?

    git_odb* data() const;
    const git_odb* constData() const;

private:
    git_odb *_db;
};


} // namespace git2
#endif // _DATABASE_HPP_

