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

#include "repository.hpp"

#include "blob.hpp"
#include "branch.hpp"
#include "commit.hpp"
#include "config.hpp"
#include "database.hpp"
#include "exception.hpp"
#include "index.hpp"
#include "oid.hpp"
#include "ref.hpp"
#include "remote.hpp"
#include "revwalk.hpp"
#include "signature.hpp"
#include "status.hpp"
#include "tag.hpp"
#include "tree.hpp"


#ifdef GIT_WIN32
#define GIT2PP_PATH_DIRECTORY_SEPARATOR '\\'
#else
#define GIT2PP_PATH_DIRECTORY_SEPARATOR '/'
#endif


namespace git2
{

namespace
{

struct GitRepositoryDeleter{
	void operator()(git_repository *repo){
		git_repository_free(repo);
	}
};

struct GitRepositoryNoDeleter{
	void operator()(git_repository *repo){
		/* Do nothing */
	}
};

}

//
// Repository
//

Repository::Repository(git_repository *repository, bool own)
{
	if(own)
		_repo = ptr_type(repository,  GitRepositoryDeleter());
	else
		_repo = ptr_type(repository, GitRepositoryNoDeleter());
}

Repository::Repository( const Repository& repo ):
_repo(repo._repo)
{
}

Repository::~Repository()
{
}

std::string Repository::discover(const std::string& startPath, bool acrossFs, const std::list<std::string>& ceilingDirs)
{
	std::vector<char> repoPath((size_t)GIT_PATH_MAX, (char)0);
	std::string joinedCeilingDirs;
	if(!ceilingDirs.empty())
	{
		std::list<std::string>::const_iterator iter = ceilingDirs.begin();
		joinedCeilingDirs = *iter;
		while(++iter != ceilingDirs.end())
		{
			joinedCeilingDirs += GIT_PATH_LIST_SEPARATOR;
			joinedCeilingDirs += *iter;
		}
	}
	Exception::git2_assert(git_repository_discover(const_cast<char*>(repoPath.data()),
									  repoPath.size(), startPath.c_str(),
                                      acrossFs, joinedCeilingDirs.c_str()));
    return std::string(repoPath.data());
}

void Repository::init(const std::string& path, bool isBare)
{
	_repo.reset();
    git_repository *repo = NULL;
    Exception::git2_assert(git_repository_init(&repo, path.c_str(), isBare));
    _repo = ptr_type(repo, GitRepositoryDeleter());
}

void Repository::open(const std::string& path)
{
	_repo.reset();
    git_repository *repo = NULL;
    Exception::git2_assert(git_repository_open(&repo, path.c_str()));
    _repo = ptr_type(repo, GitRepositoryDeleter());
}

void Repository::discoverAndOpen(const std::string &startPath,
                                     bool acrossFs,
                                     const std::list<std::string> &ceilingDirs)
{
    open(discover(startPath, acrossFs, ceilingDirs));
}

Reference Repository::head() const
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_repository_head(&ref, _repo.get()));
    return Reference(ref);
}

bool Repository::isHeadDetached() const
{
    return Exception::git2_assert(git_repository_head_detached(_repo.get())) == 1;
}

bool Repository::isHeadOrphan() const
{
    return Exception::git2_assert(git_repository_head_orphan(_repo.get())) == 1;
}

bool Repository::isEmpty() const
{
    return Exception::git2_assert(git_repository_is_empty(_repo.get())) == 1;
}

bool Repository::isBare() const
{
    return Exception::git2_assert(git_repository_is_bare(_repo.get())) == 1;
}

std::string Repository::name() const
{
    std::string repoPath = isBare() ? path() : workdir();
	size_t pos = repoPath.rfind(GIT2PP_PATH_DIRECTORY_SEPARATOR);
	if(pos==std::string::npos)
		return repoPath;
	else if(pos+1<repoPath.size())
		return repoPath.substr(pos+1);
	else
		return "";
}

std::string Repository::path() const
{
    return std::string(git_repository_path(_repo.get()));
}

std::string Repository::workdir() const
{
    return std::string(git_repository_workdir(_repo.get()));
}

void Repository::setWorkdir(const std::string& path, bool updateGitLink)
{
	Exception::git2_assert( git_repository_set_workdir(data(), path.c_str(), updateGitLink ? 1 : 0) );
}

Config Repository::configuration() const
{
    git_config *cfg;
    Exception::git2_assert( git_repository_config(&cfg, _repo.get()) );
    return Config(cfg);
}

Reference Repository::lookupReference(const std::string& name) const
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_reference_lookup(&ref, _repo.get(), name.c_str()));
    return Reference(ref);
}

OId Repository::lookupReferenceOId(const std::string& name) const
{
    git_oid oid;
    Exception::git2_assert(git_reference_name_to_id(&oid, _repo.get(), name.c_str()));
    return OId(&oid);
}

Reference Repository::lookupShorthandReference(const std::string& shorthand) const
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_reference_dwim(&ref, _repo.get(), shorthand.c_str()));
    return Reference(ref);
}

Commit Repository::lookupCommit(const OId& oid) const
{
    git_commit *commit = NULL;
    Exception::git2_assert(git_commit_lookup_prefix(&commit, _repo.get(), oid.constData(), oid.length()));
    return Commit(commit);
}

Branch Repository::lookupBranch(const std::string& branchName, git_branch_t branchType)
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_branch_lookup(&ref, _repo.get(), branchName.c_str(), branchType));
    return Branch(ref);	
}

Tag Repository::lookupTag(const OId& oid) const
{
    git_tag *tag = NULL;
    Exception::git2_assert(git_tag_lookup_prefix(&tag, _repo.get(), oid.constData(), oid.length()));
    return Tag(tag);
}

Tree Repository::lookupTree(const OId& oid) const
{
    git_tree *tree = NULL;
    Exception::git2_assert(git_tree_lookup_prefix(&tree, _repo.get(), oid.constData(), oid.length()));
    return Tree(tree);
}

Blob Repository::lookupBlob(const OId& oid) const
{
    git_blob *blob = NULL;
    Exception::git2_assert(git_blob_lookup_prefix(&blob, _repo.get(), oid.constData(), oid.length()));
    return Blob(blob);
}

Object Repository::lookup(const OId &oid) const
{
    git_object *object = NULL;
    Exception::git2_assert(git_object_lookup_prefix(&object, _repo.get(), oid.constData(), oid.length(), GIT_OBJ_ANY));
    return Object(object);
}

Reference Repository::createReference(const std::string& name, const OId& id, bool force)
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_reference_create(&ref, data(), name.c_str(), id.constData(), force?1:0));
    return Reference(ref);
}

Reference Repository::createSymbolicReference(const std::string& name, const std::string& target, bool force)
{
	git_reference *ref;
	Exception::git2_assert(git_reference_symbolic_create(&ref, data(), name.c_str(), target.c_str(), force?1:0));
	return Reference(ref);
}

OId Repository::createCommit(const std::string& ref,
                                     const Signature& author,
                                     const Signature& committer,
                                     const std::string& message,
                                     const Tree& tree,
                                     const std::list<Commit>& parents)
{
    std::vector<const git_commit*> p;
	for(const Commit& parent : parents)
		p.push_back(parent.constData());

    OId oid;
    Exception::git2_assert(git_commit_create(oid.data(), _repo.get(), ref.c_str(), author.constData(), committer.constData(), NULL, message.c_str(), tree.data(), p.size(), p.data()));
    return oid;
}

OId Repository::createCommit(const std::string& ref,
					 const Signature& author,
					 const Signature& committer,
					 const std::string& messageEncoding,
					 const std::string& message,
					 const Tree& tree,
					 const std::list<Commit>& parents)
{
    std::vector<const git_commit*> p;
	for(const Commit& parent : parents)
		p.push_back(parent.constData());

    OId oid;
    Exception::git2_assert(git_commit_create(oid.data(), _repo.get(), ref.c_str(), author.constData(), committer.constData(), messageEncoding.c_str(), message.c_str(), tree.data(), p.size(), p.data()));
    return oid;
}

Branch Repository::createBranch(const std::string& branchName, const Commit& target, bool force)
{
	git_reference *out;
	Exception::git2_assert(git_branch_create(&out, data(), branchName.c_str(), target.constData(), force?1:0));
	return Branch(out);
}

OId Repository::createTag(const std::string& name,
                                  const Object& target,
                                  bool overwrite)
{
    OId oid;
    Exception::git2_assert(git_tag_create_lightweight(oid.data(), _repo.get(), name.c_str(),
                                         target.data(), overwrite));
    return oid;
}

OId Repository::createTag(const std::string& name,
                                  const Object& target,
                                  const Signature& tagger,
                                  const std::string& message,
                                  bool overwrite)
{
    OId oid;
    Exception::git2_assert(git_tag_create(oid.data(), _repo.get(), name.c_str(), target.data(),
                             tagger.constData(), message.c_str(), overwrite));
    return oid;
}

void Repository::deleteTag(const std::string& name)
{
    Exception::git2_assert(git_tag_delete(_repo.get(), name.c_str()));
}

OId Repository::createBlobFromDisk(const std::string& path)
{
    OId oid;
    Exception::git2_assert(git_blob_create_fromdisk(oid.data(), _repo.get(), path.c_str()));
    return oid;
}

OId Repository::createBlobFromDisk(const char* path)
{
    OId oid;
    Exception::git2_assert(git_blob_create_fromdisk(oid.data(), _repo.get(), path));
    return oid;
}


OId Repository::createBlobFromBuffer(const std::vector<unsigned char>& buffer)
{
    OId oid;
    Exception::git2_assert(git_blob_create_frombuffer(oid.data(), _repo.get(), buffer.data(), buffer.size()));
    return oid;
}

OId Repository::createBlobFromBuffer(const void* buffer, size_t len)
{
    OId oid;
    Exception::git2_assert(git_blob_create_frombuffer(oid.data(), _repo.get(), buffer, len));
    return oid;
}

OId Repository::createBlobFromWorkdir(const std::string& relativePath)
{
    OId oid;
    Exception::git2_assert(git_blob_create_fromworkdir(oid.data(), _repo.get(), relativePath.c_str()));
    return oid;
}

OId Repository::createBlobFromWorkdir(const char* relativePath)
{
    OId oid;
    Exception::git2_assert(git_blob_create_fromworkdir(oid.data(), _repo.get(), relativePath));
    return oid;
}

std::list<std::string> Repository::listTags(const std::string& pattern) const
{
    std::list<std::string> list;
    git_strarray tags;
	if(pattern.empty())
		Exception::git2_assert(git_tag_list(&tags, _repo.get()));
	else
	    Exception::git2_assert(git_tag_list_match(&tags, pattern.c_str(), _repo.get()));
    for(size_t i = 0; i < tags.count; ++i)
    {
        list.push_back(std::string(tags.strings[i]));
    }
    git_strarray_free(&tags);
    return list;
}

std::list<std::string> Repository::listReferences() const
{
    std::list<std::string> list;
    git_strarray refs;
    Exception::git2_assert(git_reference_list(&refs, _repo.get()));
    for(size_t i = 0; i < refs.count; ++i)
    {
        list.push_back(std::string(refs.strings[i]));
    }
    git_strarray_free(&refs);
    return list;
}

Database Repository::database() const
{
    git_odb *odb;
    Exception::git2_assert( git_repository_odb(&odb, _repo.get()) );
    return Database(odb);
}

#ifdef TODO // Index rework
Index Repository::index() const
{
    git_index *idx;
    Exception::git2_assert(git_repository_index(&idx, _repo.get()));
    return Index(idx);
}

void Repository::setIndex(Index& index)
{
	git_repository_set_index(data(), index.data());
}

OId Repository::writeIndexTree(Index& index)
{
	git_oid oid;
	Exception::git2_assert(git_index_write_tree_to(&oid, index.data(), data()));
	return OId(oid);
}

#endif // 0 Index rework

// TODO only available from v0.19.0
/*StatusList Repository::status(const StatusOptions *options) const
{
    const git_status_options opt = options->constData();
    git_status_list *statusList;
    Exception::git2_assert(git_status_list_new(&statusList, _repo.get(), &opt));
    return StatusList(statusList);
}*/


Remote* Repository::createRemote(const std::string& name, const std::string& url)
{
	git_remote *remote;
	Exception::git2_assert(git_remote_create(&remote, data(), name.c_str(), url.c_str()));
	return new Remote(remote);
}

Remote* Repository::createMemoryRemote(const std::string& fetch, const std::string& url)
{
	git_remote *remote;
	Exception::git2_assert(git_remote_create_inmemory(&remote, data(), fetch.c_str(), url.c_str()));
	return new Remote(remote);
}

Remote* Repository::getRemote(const std::string& name)
{
	git_remote *remote;
	Exception::git2_assert(git_remote_load(&remote, data(), name.c_str()));
	return new Remote(remote);
}

std::vector<std::string> Repository::listRemote()
{
    std::vector<std::string> list;
    git_strarray repos;
    Exception::git2_assert(git_remote_list(&repos, data()));
    for(size_t i = 0; i < repos.count; ++i)
    {
        list.push_back(std::string(repos.strings[i]));
    }
    git_strarray_free(&repos);
    return list;
}

std::string Repository::getBranchUpstreamName(const std::string& canonicalBranchName)
{
	char buffer[GIT_PATH_MAX];
	int res = git_branch_upstream_name(buffer, GIT_PATH_MAX, data(), canonicalBranchName.c_str());
	if(res==GIT_ENOTFOUND)
		return "";
	else if(res>0)
		return std::string(buffer, res-1);
	else
	{
		Exception::git2_assert(res);
		return "";
	}
}

std::string Repository::getBranchRemoteName(const std::string& canonicalBranchName)
{
	char buffer[GIT_PATH_MAX];
	int res = git_branch_remote_name(buffer, GIT_PATH_MAX, data(), canonicalBranchName.c_str());
	if(res==GIT_ENOTFOUND)
		return "";
	else if(res>0)
		return std::string(buffer, res-1);
	else
	{
		Exception::git2_assert(res);
		return "";
	}
}

RevWalk Repository::createRevWalk()
{
	git_revwalk *out;
	git_revwalk_new(&out, data());
	return RevWalk(out);
}

git_repository* Repository::data() const
{
    return _repo.get();
}

const git_repository* Repository::constData() const
{
    return _repo.get();
}

} // namespace git2
