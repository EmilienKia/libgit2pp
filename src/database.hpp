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
#include "object.hpp"

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
	DatabaseBackend(git_odb_backend *dbb = NULL);
	DatabaseBackend( const DatabaseBackend& dbb );

	~DatabaseBackend();

	/**
	 * Create a backend for loose objects.
	 * 
	 * @param objectsDir the Git repository's objects directory
	 * @param compressionLevel zlib compression level to use
	 * @param doFsync whether to do an fsync() after writing (currently ignored)
	 */
	static DatabaseBackend loose(const std::string& objectsDir, int compressionLevel, bool doFsync = false);

	/**
	 * Create a backend out of a single packfile.
	 * 
	 * This can be useful for inspecting the contents of a single packfile.
	 * 
	 * @param indexFile path to the packfile's .idx file
	 */
	static DatabaseBackend onePack(const std::string& indexFile);

	/**
	 * Create a backend for the packfiles.
	 * 
	 * @param objectDir the Git repository's objects directory
	 */
	static DatabaseBackend pack(const std::string& objectDir);


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
    Database();
    
    Database( git_odb *odb);

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
    */
    static Database open(const std::string& objectsDir);

    /**
     * Close an open object database.
     */
    void close();
    
    /**
     * Refresh the object database to load newly added files.
     */
    void refresh();

    /**
     * Add a custom backend to an existing Object DB
     *
     * Read <odb_backends.h> for more information.
     *
     * @param backend pointer to a databaseBackend instance
     */
    void addBackend(DatabaseBackend *backend, int priority);

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
    */
    void addAlternate(DatabaseBackend *backend, int priority);
    
    /**
     * Add an on-disk alternate to an existing Object DB.
     * 
     * Note that the added path must point to an objects, not to a
     * full repository, to use it as an alternate store.
     * 
     * Alternate backends are always checked for objects after all
     * the main backends have been exhausted.
     * 
     * Writing is disabled on alternate backends.
     * 
     * @param path path to the objects folder for the alternate
     */
    void addDiskAlternate(const std::string& path);

    /**
     * Determine if the given object can be found in the object database.
     *
     * @param id the object to search for.
     * @return
     * - true, if the object was found
     * - false, otherwise
     */
    int exists(const OId& id);

	// TODO implement foreach functions (git_odb_foreach)
	
	/**
	 * Get the number of ODB backend objects
	 *
	 * @return number of backends in the ODB
	 */
	size_t getNumBackends();
	
	/**
	 * Lookup an ODB backend object by index
	 *
	 * @ pos index into object database backend list
	 */
	DatabaseBackend getBackend(size_t pos);

	/**
	 * Determine the object-ID (sha1 hash) of a data buffer
	 * 
	 * The resulting SHA-1 OID will be the identifier for the data buffer as if the data buffer it were to written to the ODB.
	 * 
	 * @param data data to hash
	 * @param len size of the data
	 * @param type type of the data to hash
	 */
	static OId hash(const void* data, size_t len, Object::Type type);

	/**
	 * Read a file from disk and fill a git_oid with the object id that
	 * the file would have if it were written to the Object Database as
	 * an object of the given type (w/o applying filters).
	 * Similar functionality to git.git's git hash-object without
	 * the -w flag, however, with the --no-filters flag.
	 * If you need filters, see git_repository_hashfile.
	 * 
	 * @param path file to read and determine object id for
	 * @param type the type of the object that will be hashed
	 */
	static OId hashFile(const std::string& path, Object::Type type);
	
	

	// TODO Should give access to ODB objects, streams and related ?
	
	/**
	 * Write an object directly into the ODB
	 * 
	 * This method writes a full object straight into the ODB.
	 * For most cases, it is preferred to write objects through
	 * a write stream, which is both faster and less memory intensive,
	 * specially for big objects.
	 * 
	 * This method is provided for compatibility with custom backends
	 * which are not able to support streaming writes.
	 * 
	 * @param data buffer with the data to store
	 * @param len size of the buffer
	 * @param type type of the data to store
	 * @return OID of writen object
	 */
	OId write(const void* data, size_t len, Object::Type type);


    git_odb* data() const;
    const git_odb* constData() const;

private:
    git_odb *_db;
};


} // namespace git2
#endif // _DATABASE_HPP_

