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

//
// Repository
//

Repository::Repository()
{
}

Repository::Repository(git_repository *repository):
_Class(repository)
{
}

Repository::Repository( const Repository& other):
_Class(other)
{
}

std::string Repository::discover(const std::string& startPath, bool acrossFs, const std::list<std::string>& ceilingDirs)
{
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
        helper::GitBuffer repoPath;
	Exception::git2_assert(git_repository_discover(repoPath, startPath.c_str(), acrossFs, joinedCeilingDirs.c_str()));
    return repoPath;
}

Repository Repository::init(const std::string& path, bool isBare)
{
    git_repository *repo = NULL;
    Exception::git2_assert(git_repository_init(&repo, path.c_str(), isBare));
    return Repository(repo);
}

Repository Repository::init(const std::string& path,
	uint32_t    flags,
	uint32_t    mode,
	const std::string& workdirPath,
	const std::string& description,
	const std::string& templatePath,
	const std::string& initialHead,
	const std::string& originUrl)
{
    git_repository *repo = NULL;
    
    git_repository_init_options opts;
    opts.version = GIT_REPOSITORY_INIT_OPTIONS_VERSION;
    opts.flags   = flags;
    opts.mode    = mode;
    opts.workdir_path  = workdirPath.empty() ? NULL : workdirPath.c_str();
    opts.description   = description.empty() ? NULL : description.c_str();
    opts.template_path = templatePath.empty() ? NULL : templatePath.c_str();
    opts.initial_head  = initialHead.empty() ? NULL : initialHead.c_str();
    opts.origin_url    = originUrl.empty() ? NULL : originUrl.c_str();
    
    Exception::git2_assert(git_repository_init_ext(&repo, path.c_str(), &opts));
    return Repository(repo);
}


Repository Repository::open(const std::string& path)
{
    git_repository *repo = NULL;
    Exception::git2_assert(git_repository_open(&repo, path.c_str()));
    return Repository(repo);
}

Repository Repository::discoverAndOpen(const std::string &startPath,
                                     bool acrossFs,
                                     const std::list<std::string> &ceilingDirs)
{
    return open(discover(startPath, acrossFs, ceilingDirs));
}

Repository Repository::openBare(const std::string& path)
{
    git_repository *repo = NULL;
    Exception::git2_assert(git_repository_open_bare(&repo, path.c_str()));
    return Repository(repo);
}

Repository Repository::clone(const std::string& url, const std::string& localPath, const CloneOptions& options)
{
	auto notify_cb = [](git_checkout_notify_t why, const char *path,
		const git_diff_file *baseline, const git_diff_file *target,
		const git_diff_file *workdir, void *payload)->int
	{
		CheckoutNotifyCallbackFunction* cb = (CheckoutNotifyCallbackFunction*)payload;
		if(cb!=nullptr)
			return (*cb)(why, path, DiffFile(baseline), DiffFile(target), DiffFile(workdir)) ? 0 : GIT_EUSER;
		else
			return 0;
	};
	
	auto checkout_progress_cb = [](const char *path, size_t completed_steps, size_t total_steps, void *payload)
	{
		CheckoutProgressCallbackFunction* cb = (CheckoutProgressCallbackFunction*)payload;
		if(cb!=nullptr)
			(*cb)(path, completed_steps, total_steps);
	};
	
	auto transfert_progress_cb = [](const git_transfer_progress *stats, void *payload)->int
	{
		TransfertProgressCallbackFunction* callback = (TransfertProgressCallbackFunction*)payload;
		if(payload!=nullptr)
			(*callback)(stats->total_objects, stats->indexed_objects, stats->received_objects, stats->received_bytes);
		return 0;
	};
	
	auto credentail_acquire_cb = [](git_cred **cred, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)->int
	{
		CredentialsAcquireCallbackFunction* callback = (CredentialsAcquireCallbackFunction*)payload;
		if(payload!=nullptr)
		{
			try {
				git_cred* res = (*callback)(url, username_from_url, allowed_types);
				*cred = res;
				return GIT_OK;
			}catch(Exception& ex){
				return ex.err();
			}
		}
		return GIT_OK;
	};
	
	git_clone_options opts = {
		/* version = */GIT_CLONE_OPTIONS_VERSION,
		/* checkout_opts = */ {
			GIT_CHECKOUT_OPTIONS_VERSION,
			options.checkoutOptions.strategy,
			options.checkoutOptions.disableFilters ? 1 :  0,
			options.checkoutOptions.dirMode,
			options.checkoutOptions.fileMode,
			options.checkoutOptions.fileOpenFlags,
			options.checkoutOptions.notifyFlags,
			notify_cb,
			(void*)&options.checkoutOptions.notifyCb,
			checkout_progress_cb,
			(void*)&options.checkoutOptions.progressCb,
			{},
			options.checkoutOptions.baseline.ok() ? options.checkoutOptions.baseline.data() : nullptr
//			options.checkoutOptions.targetDirectory.c_str()
		},
		/* fetch_opts = */ {},
		/* bare = */ options.bare,
		/* local =  */ GIT_CLONE_LOCAL_AUTO,
		/* checkout_branch = */ options.checkoutBranch.empty() ? NULL : options.checkoutBranch.c_str(),
		/* repository_cb = */ NULL,
		/* repository_cb_payload = */ NULL,
		/* remote_cb = */ NULL,
		/* remote_cb_payload = */ NULL
/*		transfert_progress_cb,
		(void*)&options.fetchProgressCb,
		options.remoteName.c_str(),
		options.pushUrl.empty() ? NULL : options.pushUrl.c_str(),
		options.fetchSpec.empty() ? NULL : options.fetchSpec.c_str(),
		options.pushSpec.empty() ? NULL : options.pushSpec.c_str(),
		credentail_acquire_cb,
		(void*)&options.credentialAcquireCb,
		options.transportFlags,
		NULL,//transport
		NULL,//remote_callbacks
		options.remoteAutotag,
*/
	};
	
    git_repository *repo = NULL;
    Exception::git2_assert(git_clone(&repo, url.c_str(), localPath.c_str(), &opts));
    return Repository(repo);
}


Reference Repository::head() const
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_repository_head(&ref, data()));
    return Reference(ref);
}

bool Repository::isHeadDetached() const
{
    return Exception::git2_assert(git_repository_head_detached(data())) == 1;
}

bool Repository::isEmpty() const
{
    return Exception::git2_assert(git_repository_is_empty(data())) == 1;
}

bool Repository::isBare() const
{
    return Exception::git2_assert(git_repository_is_bare(data())) == 1;
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
    return std::string(git_repository_path(data()));
}

std::string Repository::workdir() const
{
    return std::string(git_repository_workdir(data()));
}

void Repository::setWorkdir(const std::string& path, bool updateGitLink)
{
	Exception::git2_assert( git_repository_set_workdir(data(), path.c_str(), updateGitLink ? 1 : 0) );
}

Config Repository::configuration() const
{
    git_config *cfg;
    Exception::git2_assert( git_repository_config(&cfg, data()) );
    return Config(cfg);
}

Reference Repository::lookupReference(const std::string& name) const
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_reference_lookup(&ref, data(), name.c_str()));
    return Reference(ref);
}

OId Repository::lookupReferenceOId(const std::string& name) const
{
    git_oid oid;
    Exception::git2_assert(git_reference_name_to_id(&oid, data(), name.c_str()));
    return OId(&oid);
}

Reference Repository::lookupShorthandReference(const std::string& shorthand) const
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_reference_dwim(&ref, data(), shorthand.c_str()));
    return Reference(ref);
}

Commit Repository::lookupCommit(const OId& oid) const
{
    git_commit *commit = NULL;
    Exception::git2_assert(git_commit_lookup_prefix(&commit, data(), oid.constData(), oid.length()));
    return Commit(commit);
}

Branch Repository::lookupBranch(const std::string& branchName, git_branch_t branchType)
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_branch_lookup(&ref, data(), branchName.c_str(), branchType));
    return Branch(ref);	
}

Tag Repository::lookupTag(const OId& oid) const
{
    git_tag *tag = NULL;
    Exception::git2_assert(git_tag_lookup_prefix(&tag, data(), oid.constData(), oid.length()));
    return Tag(tag);
}

Tree Repository::lookupTree(const OId& oid) const
{
    git_tree *tree = NULL;
    Exception::git2_assert(git_tree_lookup_prefix(&tree, data(), oid.constData(), oid.length()));
    return Tree(tree);
}

Blob Repository::lookupBlob(const OId& oid) const
{
    git_blob *blob = NULL;
    Exception::git2_assert(git_blob_lookup_prefix(&blob, data(), oid.constData(), oid.length()));
    return Blob(blob);
}

Object Repository::lookup(const OId &oid) const
{
    git_object *object = NULL;
    Exception::git2_assert(git_object_lookup_prefix(&object, data(), oid.constData(), oid.length(), GIT_OBJ_ANY));
    return Object(object);
}

Reference Repository::createReference(const std::string& name, const OId& id, bool force, const std::string& logMessage)
{
    git_reference *ref = NULL;
    Exception::git2_assert(git_reference_create(&ref, data(), name.c_str(), id.constData(), force?1:0, logMessage.c_str()));
    return Reference(ref);
}

Reference Repository::createSymbolicReference(const std::string& name, const std::string& target, bool force, const std::string& logMessage)
{
	git_reference *ref;
	Exception::git2_assert(git_reference_symbolic_create(&ref, data(), name.c_str(), target.c_str(), force?1:0, logMessage.c_str()));
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
		p.push_back(parent.data());

    OId oid;
    Exception::git2_assert(git_commit_create(oid.data(), data(), ref.c_str(), author.data(), committer.data(), NULL, message.c_str(), tree.data(), p.size(), p.data()));
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
		p.push_back(parent.data());

    OId oid;
    Exception::git2_assert(git_commit_create(oid.data(), data(), ref.c_str(), author.data(), committer.data(), messageEncoding.c_str(), message.c_str(), tree.data(), p.size(), p.data()));
    return oid;
}

Branch Repository::createBranch(const std::string& branchName, const Commit& target, bool force)
{
	git_reference *out;
	Exception::git2_assert(git_branch_create(&out, data(), branchName.c_str(), target.data(), force?1:0));
	return Branch(out);
}

OId Repository::createTag(const std::string& name,
                                  const Object& target,
                                  bool overwrite)
{
    OId oid;
    Exception::git2_assert(git_tag_create_lightweight(oid.data(), data(), name.c_str(),
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
    Exception::git2_assert(git_tag_create(oid.data(), data(), name.c_str(), target.data(),
                             tagger.data(), message.c_str(), overwrite));
    return oid;
}

void Repository::deleteTag(const std::string& name)
{
    Exception::git2_assert(git_tag_delete(data(), name.c_str()));
}

OId Repository::createBlobFromDisk(const std::string& path)
{
    OId oid;
    Exception::git2_assert(git_blob_create_fromdisk(oid.data(), data(), path.c_str()));
    return oid;
}

OId Repository::createBlobFromDisk(const char* path)
{
    OId oid;
    Exception::git2_assert(git_blob_create_fromdisk(oid.data(), data(), path));
    return oid;
}


OId Repository::createBlobFromBuffer(const std::vector<unsigned char>& buffer)
{
    OId oid;
    Exception::git2_assert(git_blob_create_frombuffer(oid.data(), data(), buffer.data(), buffer.size()));
    return oid;
}

OId Repository::createBlobFromBuffer(const void* buffer, size_t len)
{
    OId oid;
    Exception::git2_assert(git_blob_create_frombuffer(oid.data(), data(), buffer, len));
    return oid;
}

OId Repository::createBlobFromWorkdir(const std::string& relativePath)
{
    OId oid;
    Exception::git2_assert(git_blob_create_fromworkdir(oid.data(), data(), relativePath.c_str()));
    return oid;
}

OId Repository::createBlobFromWorkdir(const char* relativePath)
{
    OId oid;
    Exception::git2_assert(git_blob_create_fromworkdir(oid.data(), data(), relativePath));
    return oid;
}

std::list<std::string> Repository::listTags(const std::string& pattern) const
{
    std::list<std::string> list;
    git_strarray tags;
	if(pattern.empty())
		Exception::git2_assert(git_tag_list(&tags, data()));
	else
	    Exception::git2_assert(git_tag_list_match(&tags, pattern.c_str(), data()));
	helper::push_back(list, &tags);
    git_strarray_free(&tags);
    return list;
}

std::list<std::string> Repository::listReferences() const
{
    std::list<std::string> list;
    git_strarray refs;
    Exception::git2_assert(git_reference_list(&refs, data()));
    helper::push_back(list, &refs);
    git_strarray_free(&refs);
    return list;
}

static int git_reference_foreach_proxy(git_reference* ref, void* payload)
{
	std::function<bool(Reference)>& callback = *(std::function<bool(Reference)>*)payload;
	return callback(Reference::undeletable(ref)) ? 1 : 0;
}

bool Repository::foreachReference(std::function<bool(Reference)> callback)
{
	int res = git_reference_foreach(data(), git_reference_foreach_proxy, (void*)&callback);
	if (res==GIT_OK)
		return false;
	else if (res==GIT_EUSER)
		return true;
	else
		Exception::git2_assert(res);
}


static int git_reference_foreach_name_proxy(const char *name, void *payload)
{
	std::function<bool(const std::string&)>& callback = *(std::function<bool(const std::string&)>*)payload;
	return callback(std::string(name)) ? 1 : 0;
}


bool Repository::foreachReferenceName(std::function<bool(const std::string&)> callback)
{
	int res = git_reference_foreach_name(data(), git_reference_foreach_name_proxy, (void*)&callback);
	if (res==GIT_OK)
		return false;
	else if (res==GIT_EUSER)
		return true;
	else
		Exception::git2_assert(res);
}


Database Repository::database() const
{
    git_odb *odb;
    Exception::git2_assert( git_repository_odb(&odb, data()) );
    return Database(odb);
}

Index Repository::index() const
{
    git_index *idx;
    Exception::git2_assert(git_repository_index(&idx, data()));
    return Index(idx);
}

/*
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
*/


std::string Repository::message()const
{
	helper::GitBuffer buffer;
	Exception::git2_assert(git_repository_message(buffer, data()));
	return buffer;
}

void Repository::removeMessage()
{
	Exception::git2_assert(git_repository_message_remove(data()));
}

bool Repository::statusForeach(StatusCallbackFunction callback)
{
	auto status_cb = [](const char *path, unsigned int status_flags, void *payload)->int
	{
		StatusCallbackFunction* callback = (StatusCallbackFunction*)payload;
		if(callback!=nullptr)
			return (*callback)(path, Status(status_flags)) ? 0 : GIT_EUSER;
		else
			return 0;
	};
	
	int res = git_status_foreach(data(), status_cb, (void*)&callback);
	if(res==GIT_OK)
		return true;
	else if (res==GIT_EUSER)
		return false;
	else
		Exception::git2_assert(res);
}

bool Repository::statusForeach(StatusCallbackFunction callback, git_status_show_t show, unsigned int flags, const std::vector<std::string>& pathspec)
{
	auto status_cb = [](const char *path, unsigned int status_flags, void *payload)->int
	{
		StatusCallbackFunction* callback = (StatusCallbackFunction*)payload;
		if(callback!=nullptr)
			return (*callback)(path, Status(status_flags)) ? 0 : GIT_EUSER;
		else
			return 0;
	};
	
	git_status_options opts = 
	{
		GIT_STATUS_OPTIONS_VERSION,
		show, flags,
		{}
	};
	helper::StrArrayFiller<std::vector<std::string>> filler(&opts.pathspec, pathspec);
	
	int res = git_status_foreach_ext(data(), &opts, status_cb, (void*)&callback);
	if(res==GIT_OK)
		return true;
	else if (res==GIT_EUSER)
		return false;
	else
		Exception::git2_assert(res);
}

Status Repository::status(const std::string& path)
{
	unsigned int status_flags;
	Exception::git2_assert(git_status_file(&status_flags, data(), path.c_str()));
	return Status(status_flags);
}

StatusList Repository::listStatus(git_status_show_t show, unsigned int flags, const std::vector<std::string>& pathspec)
{
	git_status_options opts = 
	{
		GIT_STATUS_OPTIONS_VERSION,
		show, flags,
		{}
	};
	helper::StrArrayFiller<std::vector<std::string>> filler(&opts.pathspec, pathspec);
	git_status_list *out;
	Exception::git2_assert(git_status_list_new(&out, data(), &opts));
	return StatusList(out);
}

Remote* Repository::createRemote(const std::string& name, const std::string& url)
{
	git_remote *remote;
	Exception::git2_assert(git_remote_create(&remote, data(), name.c_str(), url.c_str()));
	return new Remote(remote);
}

Remote* Repository::getRemote(const std::string& name)
{
	git_remote *remote;
	Exception::git2_assert(git_remote_lookup(&remote, data(), name.c_str()));
	return new Remote(remote);
}

void Repository::deleteRemote(const std::string& name)
{
    Exception::git2_assert(git_remote_delete(data(), name.c_str()));
}

std::vector<std::string> Repository::listRemote()
{
    std::vector<std::string> list;
    git_strarray repos;
    Exception::git2_assert(git_remote_list(&repos, data()));
    helper::push_back(list, &repos);
    git_strarray_free(&repos);
    return list;
}

std::string Repository::getBranchUpstreamName(const std::string& canonicalBranchName)
{
	helper::GitBuffer buffer;
	int res = git_branch_upstream_name(buffer, data(), canonicalBranchName.c_str());
	if(res==GIT_ENOTFOUND)
		return "";
	else if(res>0)
		return buffer;
	else
	{
		Exception::git2_assert(res);
		return "";
	}
}

std::string Repository::getBranchRemoteName(const std::string& canonicalBranchName)
{
	helper::GitBuffer buffer;
	int res = git_branch_remote_name(buffer, data(), canonicalBranchName.c_str());
	if(res==GIT_ENOTFOUND)
		return "";
	else if(res>0)
		return buffer;
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

std::pair<size_t, size_t> Repository::aheadBehind(const OId& local, const OId& upstream)const
{
	std::pair<size_t, size_t> res;
	Exception::git2_assert(git_graph_ahead_behind(&res.first, &res.second, data(), local.constData(), upstream.constData()));
	return res;
}

void Repository::addIgnoreRule(const std::string& rules)
{
	Exception::git2_assert(git_ignore_add_rule(data(), rules.c_str()));
}

void Repository::clearIgnoreInternalRules()
{
	Exception::git2_assert(git_ignore_clear_internal_rules(data()));
}

bool Repository::isIgnored(const std::string& path)
{
	int res;
	Exception::git2_assert(git_ignore_path_is_ignored(&res, data(), path.c_str()));
	return res!=0;
}

void Repository::stateCleanup()
{
	Exception::git2_assert(git_repository_state_cleanup(data()));
}

OId Repository::hashFile(const std::string& path, git_otype type, const std::string& asPath)
{
	git_oid out;
	Exception::git2_assert(git_repository_hashfile(&out, data(),
		path.empty()?NULL:path.c_str(), type, asPath.empty()?NULL:asPath.c_str()));
		
	return OId(&out);
}

void Repository::setHead(const std::string& refname)
{
	Exception::git2_assert(git_repository_set_head(data(), refname.c_str()));
}

/* TODO Need reworked OId
void Repository::setDetachedHead(const OId& commitish)
{
	Exception::git2_assert(git_repository_set_head_detached(data(), commitish.constData()));
}
*/

void Repository::detachHead()
{
	Exception::git2_assert(git_repository_detach_head(data()));
}

int Repository::state()const
{
	return git_repository_state(data());
}

void Repository::setNamespace(const std::string& nmspace)
{
	Exception::git2_assert(git_repository_set_namespace(data(), nmspace.c_str()));
}

std::string Repository::getNamespace()
{
	return git_repository_get_namespace(data());
}

void Repository::reset(Object& target, git_reset_t resetType)
{
	Exception::git2_assert(git_reset(data(), target.data(), resetType, nullptr));
}

void Repository::resetDefault(Object* target, const std::vector<std::string> pathspecs)
{
	git_strarray arr;
	helper::StrArrayFiller<std::vector<std::string> > filler(&arr, pathspecs);
	try {
		Exception::git2_assert(git_reset_default(data(), target!=NULL?target->data():NULL, &arr));
	} catch( Exception ex ) {
		throw ex;
	}
}

bool Repository::shallow()const
{
	return git_repository_is_shallow(data())!=0;
}

#if 0 // Removed for upgrading to 0.24.0
DiffList Repository::diffTreeToTree(Tree oldTree, Tree newTree)
{
	git_diff_list *diff;
	Exception::git2_assert(git_diff_tree_to_tree(&diff, data(), oldTree.data(), newTree.data(), NULL));
	return DiffList(diff);
}

DiffList Repository::diffTreeToTree(Tree oldTree, Tree newTree, uint32_t flags, uint16_t contextLines, uint16_t interhunkLines,
		const std::string& oldPrefix , const std::string& newPrefix, const std::vector<std::string>& pathspec, git_off_t maxSize,
		DiffNotifyCallbackFunction notify)
{
	git_diff_options options = {
		GIT_DIFF_OPTIONS_VERSION,
		flags,
		contextLines,
		interhunkLines,
		oldPrefix.c_str(),
		newPrefix.c_str(),
		{
			pathspec.size()>0 ? new char*[pathspec.size()] : NULL,
			pathspec.size()
		},
		maxSize,
		[](const git_diff_list *diff_so_far, const git_diff_delta *delta_to_add, const char *matched_pathspec, void *payload)->int
		{
			DiffNotifyCallbackFunction* callback = (DiffNotifyCallbackFunction*)payload;
			if(callback!=nullptr)
				return (*callback)(DiffList(const_cast<git_diff_list*>(diff_so_far)), DiffDelta(const_cast<git_diff_delta*>(delta_to_add)), std::string(matched_pathspec));
			else
				return 0;
		},
		(void*)&notify
	};
	if(pathspec.size()>0)
	{
		for(size_t n=0; n<pathspec.size(); ++n)
		{
			options.pathspec.strings[n] = const_cast<char*>(pathspec[n].data());
		}
	}
	
	git_diff_list *diff;
	int res = git_diff_tree_to_tree(&diff, data(), oldTree.data(), newTree.data(), &options);
	
	if(pathspec.size()>0)
	{
		delete options.pathspec.strings;
	}
	
	Exception::git2_assert(res);
	return DiffList(diff);
}

DiffList Repository::diffTreeToIndex(Tree oldTree, Index index)
{
	git_diff_list *diff;
	Exception::git2_assert(git_diff_tree_to_index(&diff, data(), oldTree.data(), index.data(), NULL));
	return DiffList(diff);
}

DiffList Repository::diffTreeToIndex(Tree oldTree, Index index, uint32_t flags, uint16_t contextLines, uint16_t interhunkLines,
		const std::string& oldPrefix , const std::string& newPrefix, const std::vector<std::string>& pathspec, git_off_t maxSize,
		DiffNotifyCallbackFunction notify)
{
	git_diff_options options = {
		GIT_DIFF_OPTIONS_VERSION,
		flags,
		contextLines,
		interhunkLines,
		oldPrefix.c_str(),
		newPrefix.c_str(),
		{
			pathspec.size()>0 ? new char*[pathspec.size()] : NULL,
			pathspec.size()
		},
		maxSize,
		[](const git_diff_list *diff_so_far, const git_diff_delta *delta_to_add, const char *matched_pathspec, void *payload)->int
		{
			DiffNotifyCallbackFunction* callback = (DiffNotifyCallbackFunction*)payload;
			if(callback!=nullptr)
				return (*callback)(DiffList(const_cast<git_diff_list*>(diff_so_far)), DiffDelta(const_cast<git_diff_delta*>(delta_to_add)), std::string(matched_pathspec));
			else
				return 0;
		},
		(void*)&notify
	};
	if(pathspec.size()>0)
	{
		for(size_t n=0; n<pathspec.size(); ++n)
		{
			options.pathspec.strings[n] = const_cast<char*>(pathspec[n].data());
		}
	}
	
	git_diff_list *diff;
	int res = git_diff_tree_to_index(&diff, data(), oldTree.data(), index.data(), &options);
	
	if(pathspec.size()>0)
	{
		delete options.pathspec.strings;
	}
	
	Exception::git2_assert(res);
	return DiffList(diff);
}

DiffList Repository::diffIndexToWorkdir(Index index)
{
	git_diff_list *diff;
	Exception::git2_assert(git_diff_index_to_workdir(&diff, data(), index.data(), NULL));
	return DiffList(diff);
}


DiffList Repository::diffIndexToWorkdir(Index index, uint32_t flags, uint16_t contextLines, uint16_t interhunkLines,
		const std::string& oldPrefix , const std::string& newPrefix, const std::vector<std::string>& pathspec, git_off_t maxSize,
		DiffNotifyCallbackFunction notify)
{
	git_diff_options options = {
		GIT_DIFF_OPTIONS_VERSION,
		flags,
		contextLines,
		interhunkLines,
		oldPrefix.c_str(),
		newPrefix.c_str(),
		{
			pathspec.size()>0 ? new char*[pathspec.size()] : NULL,
			pathspec.size()
		},
		maxSize,
		[](const git_diff_list *diff_so_far, const git_diff_delta *delta_to_add, const char *matched_pathspec, void *payload)->int
		{
			DiffNotifyCallbackFunction* callback = (DiffNotifyCallbackFunction*)payload;
			if(callback!=nullptr)
				return (*callback)(DiffList(const_cast<git_diff_list*>(diff_so_far)), DiffDelta(const_cast<git_diff_delta*>(delta_to_add)), std::string(matched_pathspec));
			else
				return 0;
		},
		(void*)&notify
	};
	if(pathspec.size()>0)
	{
		for(size_t n=0; n<pathspec.size(); ++n)
		{
			options.pathspec.strings[n] = const_cast<char*>(pathspec[n].data());
		}
	}
	
	git_diff_list *diff;
	int res = git_diff_index_to_workdir(&diff, data(), index.data(), &options);
	
	if(pathspec.size()>0)
	{
		delete options.pathspec.strings;
	}
	
	Exception::git2_assert(res);
	return DiffList(diff);
}

DiffList Repository::diffTreeToWorkdir(Tree oldTree)
{
	git_diff_list *diff;
	Exception::git2_assert(git_diff_tree_to_workdir(&diff, data(), oldTree.data(), NULL));
	return DiffList(diff);
}

DiffList Repository::diffTreeToWorkdir(Tree oldTree, uint32_t flags, uint16_t contextLines, uint16_t interhunkLines,
		const std::string& oldPrefix , const std::string& newPrefix, const std::vector<std::string>& pathspec, git_off_t maxSize,
		DiffNotifyCallbackFunction notify)
{
	git_diff_options options = {
		GIT_DIFF_OPTIONS_VERSION,
		flags,
		contextLines,
		interhunkLines,
		oldPrefix.c_str(),
		newPrefix.c_str(),
		{
			pathspec.size()>0 ? new char*[pathspec.size()] : NULL,
			pathspec.size()
		},
		maxSize,
		[](const git_diff_list *diff_so_far, const git_diff_delta *delta_to_add, const char *matched_pathspec, void *payload)->int
		{
			DiffNotifyCallbackFunction* callback = (DiffNotifyCallbackFunction*)payload;
			if(callback!=nullptr)
				return (*callback)(DiffList(const_cast<git_diff_list*>(diff_so_far)), DiffDelta(const_cast<git_diff_delta*>(delta_to_add)), std::string(matched_pathspec));
			else
				return 0;
		},
		(void*)&notify
	};
	if(pathspec.size()>0)
	{
		for(size_t n=0; n<pathspec.size(); ++n)
		{
			options.pathspec.strings[n] = const_cast<char*>(pathspec[n].data());
		}
	}
	
	git_diff_list *diff;
	int res = git_diff_tree_to_workdir(&diff, data(), oldTree.data(), &options);
	
	if(pathspec.size()>0)
	{
		delete options.pathspec.strings;
	}
	
	Exception::git2_assert(res);
	return DiffList(diff);
}

void Repository::checkoutHead(const CheckoutOptions& options)
{
	auto notify_cb = [](git_checkout_notify_t why, const char *path,
		const git_diff_file *baseline, const git_diff_file *target,
		const git_diff_file *workdir, void *payload)->int
	{
		CheckoutNotifyCallbackFunction* cb = (CheckoutNotifyCallbackFunction*)payload;
		if(cb!=nullptr)
			return (*cb)(why, path, DiffFile(baseline), DiffFile(target), DiffFile(workdir)) ? 0 : GIT_EUSER;
		else
			return 0;
	};
	
	auto progress_cb = [](const char *path, size_t completed_steps, size_t total_steps, void *payload)
	{
		CheckoutProgressCallbackFunction* cb = (CheckoutProgressCallbackFunction*)payload;
		if(cb!=nullptr)
			(*cb)(path, completed_steps, total_steps);
	};
	
	git_checkout_opts opts = {
		GIT_CHECKOUT_OPTS_VERSION,
		options.strategy,
		options.disableFilters ? 1 :  0,
		options.dirMode,
		options.fileMode,
		options.fileOpenFlags,
		options.notifyFlags,
		notify_cb,
		(void*)&options.notifyCb,
		progress_cb,
		(void*)&options.progressCb,
		{},
		options.baseline.ok() ? options.baseline.data() : nullptr,
		options.targetDirectory.c_str()
	};
	helper::StrArrayFiller<std::vector<std::string>> filler(&opts.paths, options.paths);
	
	Exception::git2_assert(git_checkout_head(data(), &opts));	
}

void Repository::checkoutIndex(Index index, const CheckoutOptions& options)
{
	auto notify_cb = [](git_checkout_notify_t why, const char *path,
		const git_diff_file *baseline, const git_diff_file *target,
		const git_diff_file *workdir, void *payload)->int
	{
		CheckoutNotifyCallbackFunction* cb = (CheckoutNotifyCallbackFunction*)payload;
		if(cb!=nullptr)
			return (*cb)(why, path, DiffFile(baseline), DiffFile(target), DiffFile(workdir)) ? 0 : GIT_EUSER;
		else
			return 0;
	};
	
	auto progress_cb = [](const char *path, size_t completed_steps, size_t total_steps, void *payload)
	{
		CheckoutProgressCallbackFunction* cb = (CheckoutProgressCallbackFunction*)payload;
		if(cb!=nullptr)
			(*cb)(path, completed_steps, total_steps);
	};
	
	git_checkout_opts opts = {
		GIT_CHECKOUT_OPTS_VERSION,
		options.strategy,
		options.disableFilters ? 1 :  0,
		options.dirMode,
		options.fileMode,
		options.fileOpenFlags,
		options.notifyFlags,
		notify_cb,
		(void*)&options.notifyCb,
		progress_cb,
		(void*)&options.progressCb,
		{},
		options.baseline.ok() ? options.baseline.data() : nullptr,
		options.targetDirectory.c_str()
	};
	helper::StrArrayFiller<std::vector<std::string>> filler(&opts.paths, options.paths);
	
	Exception::git2_assert(git_checkout_index(data(), index.data(), &opts));
}

void Repository::checkoutTree(Object treeish,  const CheckoutOptions& options)
{
	auto notify_cb = [](git_checkout_notify_t why, const char *path,
		const git_diff_file *baseline, const git_diff_file *target,
		const git_diff_file *workdir, void *payload)->int
	{
		CheckoutNotifyCallbackFunction* cb = (CheckoutNotifyCallbackFunction*)payload;
		if(cb!=nullptr)
			return (*cb)(why, path, DiffFile(baseline), DiffFile(target), DiffFile(workdir)) ? 0 : GIT_EUSER;
		else
			return 0;
	};
	
	auto progress_cb = [](const char *path, size_t completed_steps, size_t total_steps, void *payload)
	{
		CheckoutProgressCallbackFunction* cb = (CheckoutProgressCallbackFunction*)payload;
		if(cb!=nullptr)
			(*cb)(path, completed_steps, total_steps);
	};
	
	git_checkout_opts opts = {
		GIT_CHECKOUT_OPTS_VERSION,
		options.strategy,
		options.disableFilters ? 1 :  0,
		options.dirMode,
		options.fileMode,
		options.fileOpenFlags,
		options.notifyFlags,
		notify_cb,
		(void*)&options.notifyCb,
		progress_cb,
		(void*)&options.progressCb,
		{},
		options.baseline.ok() ? options.baseline.data() : nullptr,
		options.targetDirectory.c_str()
	};
	helper::StrArrayFiller<std::vector<std::string>> filler(&opts.paths, options.paths);
	
	Exception::git2_assert(git_checkout_tree(data(), (git_object*)treeish.data(), &opts));
}

OId Repository::stashSave(Signature stasher, const std::string& message, unsigned int flags)
{
	git_oid out;
	
	int res = git_stash_save(&out, data(), const_cast<git_signature*>(stasher.data()), message.empty()?NULL:message.data(), flags);
	if(res==GIT_OK)
	{
		return OId(&out);
	}
	else if(res==GIT_ENOTFOUND)
	{
		return OId();
	}
	else
	{
		Exception::git2_assert(res);
	}
}

bool Repository::stashForeach(StashCallbackFunction callback)
{
	auto foreach_cb = [](size_t index, const char* message, const git_oid *stash_id, void *payload)->int
	{
		StashCallbackFunction* cb = (StashCallbackFunction*)payload;
		if(cb!=nullptr)
			return (*cb)(index, message, OId(stash_id)) ? GIT_OK : GIT_EUSER;
		else
			return GIT_OK;
	};
	
	int res = git_stash_foreach(data(), foreach_cb, &callback);
	if(res==GIT_OK)
	{
		return true;
	}
	else if(res==GIT_EUSER)
	{
		return false;
	}
	else
	{
		Exception::git2_assert(res);
	}
}

void Repository::stashDrop(size_t index)
{
	Exception::git2_assert(git_stash_drop(data(), index));
}
#endif // Removed for upgrading to 0.24.0

} // namespace git2
