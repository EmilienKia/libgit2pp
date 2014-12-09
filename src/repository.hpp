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

#ifndef _GIT2PP_REPOSITORY_HPP_
#define _GIT2PP_REPOSITORY_HPP_

#include <git2.h>

#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace git2
{

class Blob;
class Branch;
class Commit;
class Config;
class Database;
class Index;
class Object;
class OId;
class Tag;
class Tree;
class Reference;
class RefLog;
class Remote;
class RevWalk;
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
	 * Create a new Git repository in the given folder with extended controls.
	 *
	 * This will initialize a new git repository (creating the path
	 * if requested by flags) and working directory as needed.  It will
	 * auto-detect the case sensitivity of the file system and if the
	 * file system supports file mode bits correctly.
	 * 
	 * @param path The path to the repository.
	 * @param flags - Combination of GIT_REPOSITORY_INIT flags above.
	 * @param mode  - Set to one of the standard GIT_REPOSITORY_INIT_SHARED_...
	 *        constants above, or to a custom value that you would like.
	 * @param workdirPath - The path to the working dir or empty for default (i.e.
	 *        repo_path parent on non-bare repos).  IF THIS IS RELATIVE PATH,
	 *        IT WILL BE EVALUATED RELATIVE TO THE REPO_PATH.  If this is not
	 *        the "natural" working directory, a .git gitlink file will be
	 *        created here linking to the repo_path.
	 * @param description - If set, this will be used to initialize the "description"
	 *        file in the repository, instead of using the template content.
	 * @param templatePath - When GIT_REPOSITORY_INIT_EXTERNAL_TEMPLATE is set,
	 *        this contains the path to use for the template directory.  If
	 *        this is NULL, the config or default directory options will be
	 *        used instead.
	 * @param initialHead - The name of the head to point HEAD at.  If empty, then
	 *        this will be treated as "master" and the HEAD ref will be set
	 *        to "refs/heads/master".  If this begins with "refs/" it will be
	 *        used verbatim; otherwise "refs/heads/" will be prefixed.
	 * @param originUrl - If this is non-empty, then after the rest of the
	 *        repository initialization is completed, an "origin" remote
	 *        will be added pointing to this URL.
	 */
	void init(const std::string& path,
		uint32_t    flags,
		uint32_t    mode,
		const std::string& workdirPath,
		const std::string& description,
		const std::string& templatePath,
		const std::string& initialHead,
		const std::string& originUrl);


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

	// TODO Implement git_repository_wrap_odb

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

	// TODO Implement git_repository_open_ext
	
	/**
	 * Open a bare repository on the serverside.
	 *
	 * This is a fast open for bare repositories that will come in handy
	 * if you're e.g. hosting git repositories and need to access them
	 * efficiently
	 * 
	 * @param path Direct path to the bare repository
	 */
	void openBare(const std::string& path);

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
    Reference lookupReference(const std::string& name) const;

    /**
     * Lookup a reference by its name in a repository and returns the oid of its target.
     *
     * @throws Exception
     * @return The OId of the target
     */
    OId lookupReferenceOId(const std::string& name) const;

    /**
	 * Lookup a reference by DWIMing its short name
	 *
	 * Apply the git precendence rules to the given shorthand to determine
	 * which reference the user is refering to.
     *
     * @param shorthand the short name for the reference
     * @throws Exception
     * @return The reference with the given name
     */
    Reference lookupShorthandReference(const std::string& shorthand) const;

    /**
     * Lookup a commit object from a repository.
     *
	 * @param id Identity of the commit to locate. If the object is
	 *		an annotated tag it will be peeled back to the commit (complete or short id).
     * @throws Exception
     */
    Commit lookupCommit(const OId& oid) const;

	/**
	 * Lookup a branch by its name in a repository.
	 * 
	 * @param branchName Name of the branch to be looked-up;
	 * this name is validated for consistency.
	 * @param branchType Type of the considered branch.
	 * This should be valued with either GIT_BRANCH_LOCAL or GIT_BRANCH_REMOTE.
	 */
	Branch lookupBranch(const std::string& branchName, git_branch_t branchType);

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
	 * Create a new symbolic reference.
	 *
	 * A direct reference (also called an object id reference) refers
	 * directly to a specific object id (a.k.a. OID or SHA) in the repository.
	 * The id permanently refers to the object (although the reference itself can be moved).
	 * For example, in libgit2 the direct ref "refs/tags/v0.17.0" refers
	 * to OID 5b9fac39d8a76b9139667c26a63e6b3f204b3977.
	 * 
	 * The direct reference will be created in the repository and
	 * written to the disk. The generated reference object must be freed by the user.
	 * 
	 * Valid reference names must follow one of two patterns:
	 *  - Top-level names must contain only capital letters and underscores,
	 * and must begin and end with a letter. (e.g. "HEAD", "ORIG_HEAD").
	 *  - Names prefixed with "refs/" can be almost anything.
	 * You must avoid the characters '~', '^', ':', ' \ ', '?', '[',
	 * and '*', and the sequences ".." and " @ {" which have special
	 * meaning to revparse.
	 * 
	 * This function will throw an exception if a reference already
	 * exists with the given name unless force is true,
	 * in which case it will be overwritten.
	 * 
	 * The signature and message for the reflog will be ignored if
	 * the reference does not belong in the standard set
	 * (HEAD, branches and remote-tracking branches) and and it does not have a reflog.
     *
     * @param name The name of the reference
     * @param id The object id pointed to by the reference.
     * @param force Overwrite existing references
     * @throws Exception
     */
    Reference createReference(const std::string& name, const OId& id, bool force);

	/**
	 * Create a new symbolic reference.
	 *
	 * A symbolic reference is a reference name that refers to another
	 * reference name.  If the other name moves, the symbolic name will move,
	 * too.  As a simple example, the "HEAD" reference might refer to
	 * "refs/heads/master" while on the "master" branch of a repository.
	 *
	 * The symbolic reference will be created in the repository and written to
	 * the disk.  The generated reference object must be freed by the user.
	 *
	 * Valid reference names must follow one of two patterns:
	 *
	 * 1. Top-level names must contain only capital letters and underscores,
	 *    and must begin and end with a letter. (e.g. "HEAD", "ORIG_HEAD").
	 * 2. Names prefixed with "refs/" can be almost anything.  You must avoid
	 *    the characters '~', '^', ':', '\\', '?', '[', and '*', and the
	 *    sequences ".." and "@{" which have special meaning to revparse.
	 *
	 * This function will return an error if a reference already exists with the
	 * given name unless `force` is true, in which case it will be overwritten.
	 *
	 * @param name Reference name
	 * @param target Reference target
	 * @param force True to overwrite existing reference with same name, if any.
	 * @return Created reference.
     * @throws Exception
	 */
	Reference createSymbolicReference(const std::string& name, const std::string& target, bool force);

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
	 * Create a new branch pointing at a target commit
	 * 
	 * A new direct reference will be created pointing to this target commit.
	 * If force is true and a reference already exists with the given name, it'll be replaced.
	 * The branch name will be checked for validity.
	 * 
	 * @param branchName Name for the branch; this name is validated for consistency.
	 * It should also not conflict with an already existing branch name.
	 * @param target Commit to which this branch should point.
	 * This object must belong to this repository.
	 * @param force Overwrite existing branch.
	 */
	Branch createBranch(const std::string& branchName, const Commit& target, bool force);

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

	// TODO implement git_reference_foreach and git_reference_foreach_name
	
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
    
    // TODO Implement git_repository_refdb

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
//	void setIndex(Index& index);
	
	/**
	 * Write the index as a tree to the given repository
	 * 
	 * The index must not contain any file in conflict.
	 * 
	 * @param index Index to write
	 * @return OID of the the written tree
	 */
//	OId writeIndexTree(Index& index);

/** 
 * @name Message
 * @{
 */

	/** 
	 * Retrieve git's prepared message
	 *
	 * Operations such as git revert/cherry-pick/merge with the -n option
	 * stop just short of creating a commit with the changes and save
	 * their prepared message in .git/MERGE_MSG so the next git-commit
	 * execution can present it to the user for them to amend if they
	 * wish.
	 *
	 * Use this function to get the contents of this file. Don't forget to
	 * remove the file after you create the commit.
	 */
	std::string message()const;
	
	/**
	 * Remove git's prepared message.
	 */
	void removeMessage();

/** @} */
	
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
 * @name Remotes
 * @{
 */

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
	 * Create a remote in memory
	 *
	 * Create a remote with the given refspec in memory. You can use
	 * this when you have a URL instead of a remote's name.  Note that in-memory
	 * remotes cannot be converted to persisted remotes.
	 *
	 * @param fetch the fetch refspec to use for this remote. May be empty for defaults.
	 * @param url the remote repository's URL
	 */
	Remote* createMemoryRemote(const std::string& fetch, const std::string& url);

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
	std::vector<std::string> listRemote();
	
	/**
	 * Return the name of the reference supporting the remote tracking branch,
	 * given the name of a local branch reference.
	 * 
	 * @param canonicalBranchName name of the local branch.
	 * @return Remote tracking branch name. Empty if not found.
	 */
	std::string getBranchUpstreamName(const std::string& canonicalBranchName);
	
	/**
	 * Return the name of remote that the remote tracking branch belongs to.
	 * 
	 * @param canonicalBranchName name of the local branch.
	 * @return Remote tracking remote name. Empty if not found.
	 */
	 std::string getBranchRemoteName(const std::string& canonicalBranchName);
	 
/** @} */
	
	/**
	 * Create a revision walker for this repository.
	 */
	RevWalk createRevWalk();
	
	/**
	 * Count the number of unique commits between two commit objects
	 *
	 * There is no need for branches containing the commits to have any
	 * upstream relationship, but it helps to think of one as a branch and
	 * the other as its upstream, the `ahead` and `behind` values will be
	 * what git would report for the branches.
	 * 
	 * @param local the commit for local
	 * @param upstream the commit for upstream
	 */
	std::pair<size_t, size_t> aheadBehind(const OId& local, const OId& upstream)const;
	
/**
 * @name Ignore
 * @{
 */
	
	/**
	 * Add ignore rules for a repository.
	 *
	 * Excludesfile rules (i.e. .gitignore rules) are generally read from
	 * .gitignore files in the repository tree or from a shared system file
	 * only if a "core.excludesfile" config value is set.  The library also
	 * keeps a set of per-repository internal ignores that can be configured
	 * in-memory and will not persist.  This function allows you to add to
	 * that internal rules list.
	 *
	 * Example usage:
	 *
	 *     repository.addIgnoreRule("*.c\ndir/\nFile with space\n");
	 *
	 * This would add three rules to the ignores.
	 *
	 * @param rules Text of rules, a la the contents of a .gitignore file.
	 *              It is okay to have multiple rules in the text; if so,
	 *              each rule should be terminated with a newline.
	 */
	 void addIgnoreRule(const std::string& rules);

	/**
	 * Clear ignore rules that were explicitly added.
	 *
	 * Resets to the default internal ignore rules.  This will not turn off
	 * rules in .gitignore files that actually exist in the filesystem.
	 *
	 * The default internal ignores ignore ".", ".." and ".git" entries.
	 */
	void clearIgnoreInternalRules();
	
	/**
	 * Test if the ignore rules apply to a given path.
	 *
	 * This function checks the ignore rules to see if they would apply to the
	 * given file.  This indicates if the file would be ignored regardless of
	 * whether the file is already in the index or committed to the repository.
	 *
	 * One way to think of this is if you were to do "git add ." on the
	 * directory containing the file, would it be added or not?
	 *
	 * @param path the file to check ignores for, relative to the repo's workdir.
	 * @return true if ignored
	 */
	bool isIgnored(const std::string& path);

/** @} */

	/**
	 * Remove all the metadata associated with an ongoing git merge, including
	 * MERGE_HEAD, MERGE_MSG, etc.
	 */
	void cleanupMerge();

	// TODO implement git_repository_fetchhead_foreach
	// TODO implement git_repository_mergehead_foreach
	
	/**
	 * Calculate hash of file using repository filtering rules.
	 *
	 * If you simply want to calculate the hash of a file on disk with no filters,
	 * you can just use the `Database::hash()` API.  However, if you want to
	 * hash a file in the repository and you want to apply filtering rules (e.g.
	 * crlf filters) before generating the SHA, then use this function.
	 * 
	 * @param path Path to file on disk whose contents should be hashed.
	 * 			Can be a relative path.
	 * @param type The object type to hash as (e.g. GIT_OBJ_BLOB)
	 * @param asPath The path to use to look up filtering rules. If this is
	 *             empty, then the `path` parameter will be used instead. If
	 *             this is passed as the empty string, then no filters will be
	 *             applied when calculating the hash.
	 */
	OId hashFile(const std::string& path, git_otype type, const std::string& asPath);

	/**
	 * Make the repository HEAD point to the specified reference.
	 *
	 * If the provided reference points to a Tree or a Blob, the HEAD is
	 * unaltered and an exception is raised.
	 *
	 * If the provided reference points to a branch, the HEAD will point
	 * to that branch, staying attached, or become attached if it isn't yet.
	 * If the branch doesn't exist yet, no error will be return. The HEAD
	 * will then be attached to an unborn branch.
	 *
	 * Otherwise, the HEAD will be detached and will directly point to
	 * the Commit.
	 * 
	 * @param refname Canonical name of the reference the HEAD should point at
	 */
	void setHead(const std::string& refname);
	
	/**
	 * Make the repository HEAD directly point to the Commit.
	 *
	 * If the provided committish cannot be found in the repository, the HEAD
	 * is unaltered and an exception is raised.
	 *
	 * If the provided commitish cannot be peeled into a commit, the HEAD
	 * is unaltered and an exception is raised.
	 *
	 * Otherwise, the HEAD will eventually be detached and will directly point to
	 * the peeled Commit.
	 * 
	 * @param commitish Object id of the Commit the HEAD should point to
	 */
	// TODO TODO Need reworked OId
	//void setDetachedHead(const OId& commitish);

	/**
	 * Detach the HEAD.
	 *
	 * If the HEAD is already detached and points to a non commitish, the HEAD is 
	 * unaltered, and an exception is raised.
	 */
	void detachHead();
	
	/**
	 * Determines the status of a git repository - ie, whether an operation
	 * (merge, cherry-pick, etc) is in progress.
	 * 
	 * @see git_repository_state_t
	 */
	int state()const;
	
/**
 * @name Namespaces
 * @{
 */

	/**
	 * Sets the active namespace for this Git Repository
	 *
	 * This namespace affects all reference operations for the repo.
	 * See `man gitnamespaces`
	 * 
	 * @param nmspace The namespace. This should not include the refs
	 *	folder, e.g. to namespace all references under `refs/namespaces/foo/`,
	 *	use `foo` as the namespace.
	 */
	void setNamespace(const std::string& nmspace);
	
	/**
	 * Get the currently active namespace for this repository
	 */
	std::string getNamespace();

/** @} */

	/**
	 * Determine if the repository was a shallow clone
	 */
	bool shallow()const;
	
/**
 * @name Reset
 * @{
 */
 
	/**
	 * Sets the current head to the specified commit oid and optionally
	 * resets the index and working tree to match.
	 *
	 * SOFT reset means the Head will be moved to the commit.
	 *
	 * MIXED reset will trigger a SOFT reset, plus the index will be replaced
	 * with the content of the commit tree.
	 *
	 * HARD reset will trigger a MIXED reset and the working directory will be
	 * replaced with the content of the index.  (Untracked and ignored files
	 * will be left alone, however.)
	 *
	 * TODO: Implement remaining kinds of resets.
	 *
	 * @param target Committish to which the Head should be moved to. This object
	 * must belong to the given `repo` and can either be a commit or a
	 * tag. When a git_tag is being passed, it should be dereferencable
	 * to a commit which oid will be used as the target of the branch.
	 *
	 * @param resetType Kind of reset operation to perform.
	 */
	void reset(Object& target, git_reset_t resetType);
	
	/**
	 * Updates some entries in the index from the target commit tree.
	 *
	 * The scope of the updated entries is determined by the paths
	 * being passed in the `pathspecs` parameters.
	 *
	 * Passing a NULL `target` will result in removing
	 * entries in the index matching the provided pathspecs.
	 *
	 * @param target The committish which content will be used to reset the content
	 * of the index.
	 *
	 * @param pathspecs List of pathspecs to operate on.
	 */ 
	void resetDefault(Object* target, const std::vector<std::string> pathspecs);
	
/** @} */
	
    git_repository* data() const;
    const git_repository* constData() const;

private:
    typedef std::shared_ptr<git_repository> ptr_type;
    ptr_type _repo;
};

} // namespace git2
#endif // _GIT2PP_REPOSITORY_HPP_

