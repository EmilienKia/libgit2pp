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

#ifndef _REPOSITORY_HPP_
#define _REPOSITORY_HPP_

#include <git2.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace git2
{

class Blob;
class Commit;
class Config;
class Database;
class Index;
class Object;
class OId;
class Tag;
class Tree;
class Reference;
class Remote;
class Signature;
class StatusList;
class StatusOptions;

/**
 * Represents a Git repository.
 */
class Repository
{
public:

    /**
     * Construct a wrapper around a libgit2 repository pointer.
     *
     * If `own` is true, this Repository takes ownership of the pointer, and makes
     * sure it is freed when the owner is deleted, unless there are other instances
     * sharing the ownership. If `own` is true, the pointer must not be freed manually,
     * and must not be passed to another Repository instance also with `own` true.
     */
    Repository(git_repository *repository = NULL, bool own = false);

    /**
     * Copy constructor; creates a copy of the object, sharing the same underlaying data
     * structure.
     */
    Repository(const Repository& repo);

    /**
     * Destruct a previously allocated repository
     */
    ~Repository();

    /**
     * Look for a git repository and return its path. The lookup start from startPath and
     * walk across parent directories if nothing has been found. The lookup ends when the
     * first repository is found, or when reaching a directory that is referenced in
     * ceilingDirs, or when the filesystem changes (unless acrossFs is true).
     *
     * The method will automatically detect if the repository is bare (if there is a
     * repository).
     *
     * The function will only return successfully if a repository was found, otherwise an
     * exception is thrown, providing an error message.
     *
     * @param startPath
     * The base path where the lookup starts.
     *
     * @param acrossFs
     * If true, then the lookup will not stop when a filesystem device change is detected
     * while exploring parent directories.
     *
     * @param ceilingDirs
     * A list of absolute symbolic link free paths. The lookup will stop if any of these
     * paths are reached. Note that the lookup always performs on startPath no matter if
     * startPath appears in ceilingDirs.
     *
     * @return The path of the found repository
     * @throws Exception
     */
    static std::string discover(const std::string& startPath,
                            bool acrossFs = false,
                            const std::list<std::string>& ceilingDirs = std::list<std::string>());

    /**
     * Constructs a new Git repository in the given folder.
     *
     * @param path the path to the repository
     * @param isBare if true, a Git repository without a working directory is created
     * at the pointed path. If false, provided path will be considered as the working
     * directory into which the .git directory will be created.
     *
     * @throws Exception
     */
    void init(const std::string& path, bool isBare);

    /**
     * Open a git repository.
     *
     * The 'path' argument must point to an existing git repository
     * folder, e.g.
     *
     * /path/to/my_repo/.git/ (normal repository)
     * objects/
     * index
     * HEAD
     *
     * /path/to/bare_repo/ (bare repository)
     * objects/
     * index
     * HEAD
     *
     * The method will automatically detect if 'path' is a normal
     * or bare repository or fail is 'path' is neither.
     *
     * @param path the path to the repository
     * @throws Exception
     */
    void open(const std::string& path);

    /**
     * Convenience function for finding and opening a git repository.
     *
     * Calls discover() with the given arguments, and passes the result to open().
     *
     * @throws Exception
     */
    void discoverAndOpen(const std::string &startPath,
                         bool acrossFs = false,
                         const std::list<std::string> &ceilingDirs = std::list<std::string>());

    /**
     * Retrieve and resolve the reference pointed at by HEAD.
     *
     * @throws Exception
     */
    Reference head() const;

    /**
     * Check if a repository's HEAD is detached
     *
     * A repository's HEAD is detached when it points directly to a commit
     * instead of a branch.
     *
     * @throws Exception
     */
    bool isHeadDetached() const;

    /**
     * Check if the current branch is an orphan
     *
     * An orphan branch is one named from HEAD but which doesn't exist in
     * the refs namespace, because it doesn't have any commit to point to.
     *
     * @throws Exception
     */
    bool isHeadOrphan() const;

    /**
     * Check if a repository is empty
     *
     * An empty repository has just been initialized and contains
     * no commits.
     *
     * @throws Exception
     */
    bool isEmpty() const;

    /**
     * Check if a repository is bare
     *
     * @throws Exception
     */
    bool isBare() const;

    /**
     * The name equals the repositories working directory name.
     * In case of a bare repository, the name equals the repositorie's directory.
     */
    std::string name() const;

    /**
	 * Get the path of this repository
	 *
	 * This is the path of the `.git` folder for normal repositories,
	 * or of the repository itself for bare repositories.
	 *
	 * @return the path to the repository
     */
    std::string path() const;
	
    /**
	 * Get the path of the working directory for this repository
	 *
	 * If the repository is bare, this function will always return empty.
	 *
	 * @return the path to the working dir, if it exists
     */
    std::string workdir() const;

	/**
	 * Set the path to the working directory for this repository
	 *
	 * The working directory doesn't need to be the same one
	 * that contains the `.git` folder for this repository.
	 *
	 * If this repository is bare, setting its working directory
	 * will turn it into a normal repository, capable of performing
	 * all the common workdir operations (checkout, status, index
	 * manipulation, etc).
	 *
	 * @param path The path to a working directory
	 * @param updateGitLink Create/update gitlink in workdir and set config
	 *        "core.worktree" (if workdir is not the parent of the .git directory)
	 * @throws Exception
	 */
	void setWorkdir(const std::string& path, bool updateGitLink);
		
    /**
	 * Get the configuration file for this repository.
	 *
	 * If a configuration file has not been set, the default
	 * config set for the repository will be returned, including
	 * global and system configurations (if they are available).
	 * 
	 * @return The configuration object.
	 * @throws Exception
     */
    Config configuration() const;

    /**
     * Lookup a reference by its name in a repository.
     *
     * @throws Exception
     * @return The reference with the given name
     */
    Reference* lookupRef(const std::string& name) const;

    /**
     * Lookup a reference by its name in a repository and returns the oid of its target.
     *
     * @throws Exception
     * @return The OId of the target
     */
    OId* lookupRefOId(const std::string& name) const;

    /**
     * Lookup a reference by its shorthand name in a repository.
     *
     * @throws Exception
     * @return The reference with the given name
     */
// TODO only available from v0.19.0
//    Reference* lookupShorthandRef(const std::string& shorthand) const;

    /**
     * Lookup a commit object from a repository.
     *
	 * @param id Identity of the commit to locate. If the object is
	 *		an annotated tag it will be peeled back to the commit (complete or short id).
     * @throws Exception
     */
    Commit lookupCommit(const OId& oid) const;

    /**
     * Lookup a tag object from the repository.
     *
     * @throws Exception
     */
    Tag lookupTag(const OId& oid) const;

    /**
     * Lookup a tree object from the repository.
     *
     * @throws Exception
     */
    Tree lookupTree(const OId& oid) const;

    /**
     * Lookup a blob object from a repository.
	 * @param id Identity of the blob to locate (complete or short id).
     * @throws Exception
     */
    Blob lookupBlob(const OId& oid) const;

    /**
     * Lookup a reference to one of the objects in a repostory.
	 * @param id Identifier for the object (complete or short id).
     * @throws Exception
     */
    Object lookup(const OId& oid) const;

    /**
     * Create a new object id reference.
     *
     * The reference will be created in the repository and written
     * to the disk.
     *
     * If `overwrite` is true and there already exists a reference
     * with the same name, it will be overwritten.
     *
     * @throws Exception
     */
    Reference* createRef(const std::string& name, const OId& id, bool overwrite = true);

	/**
	 * Create a new symbolic reference.
	 *
	 * The reference will be created in the repository and written
	 * to the disk.
	 *
	 * The generated reference must be freed by the user.
	 *
	 * If `force` is true and there already exists a reference
	 * with the same name, it will be overwritten.
	 *
	 * @param name Reference name
	 * @param target Reference target
	 * @param force True to overwrite existing reference with same name, if any.
	 * @return Created reference.
     * @throws Exception
	 */
	Reference* createSymbolicReference(const std::string& name, const std::string& target, bool force=false);

    /**
     * Create a new commit in the repository
     *
     * @throws Exception
     */
    OId createCommit(const std::string& ref,
                         const Signature& author,
                         const Signature& committer,
                         const std::string& message,
                         const Tree& tree,
                         const std::list<Commit>& parents);
    OId createCommit(const std::string& ref,
                         const Signature& author,
                         const Signature& committer,
                         const std::string& messageEncoding,
                         const std::string& message,
                         const Tree& tree,
                         const std::list<Commit>& parents);

    /**
     * Create a new lightweight tag pointing at a target object
     *
     * A new direct reference will be created pointing to
     * this target object. If `force` is true and a reference
     * already exists with the given name, it'll be replaced.
     *
     * @throws Exception
     */
    OId createTag(const std::string& name,
                      const Object& target,
                      bool overwrite = true);

    /**
     * Create a new tag in the repository from an object
     *
     * A new reference will also be created pointing to
     * this tag object. If `overwrite` is true and a reference
     * already exists with the given name, it'll be replaced.
     *
     * @throws Exception
     */
    OId createTag(const std::string& name,
                      const Object& target,
                      const Signature& tagger,
                      const std::string& message,
                      bool overwrite = true);

    /**
     * Delete an existing tag reference.
     *
     * @throws Exception
     */
    void deleteTag(const std::string& name);

    /**
     * Write an in-memory buffer to the ODB as a blob
     *
     * @param buffer Data to be written into the blob
     * @return Created loose blob OId.
     * @throws Exception
     */
    OId createBlobFromBuffer(const std::vector<unsigned char>& buffer);
    OId createBlobFromBuffer(const void* buffer, size_t len);
    
    // TODO Wrap git_blob_create_fromchunks

    /**
     * Read a file from the filesystem and write its content
     * to the Object Database as a loose blob
     *
     * @param path file from which the blob will be created
     * @return Created loose blob OId.
     * @throws Exception
     */
    OId createBlobFromDisk(const std::string& path);
    OId createBlobFromDisk(const char* path);

    /**
     * Read a file from the working folder of a repository and
     * write it to the Object Database as a loose blob
     *
     * @param path file from which the blob will be created,
     * relative to the repository's working dir
     * @return Created loose blob OId.
     * @throws Exception
     */
    OId createBlobFromWorkdir(const std::string& relativePath);
    OId createBlobFromWorkdir(const char* relativePath);


    /**
     * Create a list with all the tags in the Repository
     * which name match a defined pattern
     *
     * If an empty pattern is provided, all the tags
     * will be returned.
     *
     * @param pattern Standard fnmatch pattern
     * @throws Exception
     */
    std::list<std::string> listTags(const std::string& pattern = "") const;

    /**
     * Create a list with all references in the Repository.
     *
     * @throws Exception
     */
    std::list<std::string> listReferences() const;

	// TODO implement git_reference_foreach
	
    /**
	 * Get the Object Database for this repository.
	 *
	 * If a custom ODB has not been set, the default
	 * database for the repository will be returned (the one
	 * located in `.git/objects`).
	 *
	 * @return Object database.
     * @throws Exception
     */
    Database database() const;

    /**
	 * Get the Index file for this repository.
	 *
	 * If a custom index has not been set, the default
	 * index for the repository will be returned (the one
	 * located in `.git/index`).
	 *
	 * @return The Index file for this repository.
	 * @throws Exception 
     */
    Index index() const;

	/**
	 * Set the index file for this repository
	 *
	 * This index will be used for all index-related operations
	 * involving this repository.
	 *
	 * The repository will keep a reference to the index file;
	 * the user must still free the index after setting it
	 * to the repository, or it will leak.
	 *
	 * @param index An index object
	 */
	void setIndex(Index& index);
	
	/**
	 * Write the index as a tree to the given repository
	 * 
	 * The index must not contain any file in conflict.
	 * 
	 * @param index Index to write
	 * @return OID of the the written tree
	 */
	OId writeIndexTree(Index& index);
	
    /**
     * @brief Get the status information of the Git repository
     *
     * This function returns the status of the repository entries, according to
     * the given options.
     *
     * @throws Exception
     * @return The list of status entries
     */
// TODO only available from v0.19.0
//    StatusList status(const StatusOptions *options) const;


	/**
	 * Create a remote in memory
	 *
	 * Create a remote with the default refspecs in memory. You can use
	 * this when you have a URL instead of a remote's name.
	 *
	 * @param name The remote's name
	 * @param url The remote repository's URL
	 * @return Pointer to the new remote object.
	 * @throws Exception
	 */
	Remote* createRemote(const std::string& name, const std::string& url);

	/**
	 * Get the information for a particular remote
	 *
	 * @param name the remote's name
	 * @return Pointer to the new remote object
	 * @throws Exception
	 */
	Remote* getRemote(const std::string& name);

	/**
	 * Get a list of the configured remotes for a repo
	 * 
	 * @throws Exception
	 */
	std::list<std::string> listRemote();
	
    git_repository* data() const;
    const git_repository* constData() const;

private:
    typedef std::shared_ptr<git_repository> ptr_type;
    ptr_type _repo;
};

} // namespace git2
#endif // _REPOSITORY_HPP_

