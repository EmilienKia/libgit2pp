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

#ifndef _GIT2PP_REMOTE_HPP_
#define _GIT2PP_REMOTE_HPP_

#include <git2.h>

#include <functional>
#include <string>
#include <vector>

#include "common.hpp"

#include "oid.hpp"

namespace git2
{

class Exception;

typedef std::function<bool(bool local, OId oid, OId loid, const std::string& name)> HeadListCallbackFunction;

typedef std::function<void(unsigned int total_objects, unsigned int indexed_objects, unsigned int received_objects, size_t received_bytes)> TransfertProgressCallbackFunction;

typedef std::function<void(const std::string& problematicRefspec)> RenameProblemCallbackFunction;

/**
 * Represents a Git remote refspec.
 */ 
class RefSpec
{
public:
	/**
	 * Constructor.
	 */
	RefSpec(const git_refspec *refspec);

	/**
	 * Destructor.
	 */
	~RefSpec();

	/**
	 * Get the source specifier
	 */
	std::string src()const;

	/**
	 * Get the destination specifier
	 */
	std::string dst()const;

	/**
	 * Get the refspec's string
	 */
	std::string str()const;
	
	/**
	 * Get the force update setting
	 */
	bool force()const;

	/**
	 * Get the refspec's direction.
	 * 
	 * @return GIT_DIRECTION_FETCH or GIT_DIRECTION_PUSH
	 */
	git_direction direction()const;

	/**
	 * Check if a refspec's source descriptor matches a reference
	 */
	bool sourceMatches(const std::string& refname)const;

	/**
	 * Check if a refspec's destination descriptor matches a reference
	 */
	bool destinationMatches(const std::string& refname)const;

	/**
	 * Transform a reference to its target following the refspec's rules
	 *
	 * @param name the name of the reference to transform
	 * @return The target name.
	 * @throws Exception
	 */
	std::string transform(const std::string& name)const;
	 
	 /**
	  * Transform a target reference to its source reference following the refspec's rules
	  * 
	  * @param name the name of the reference to transform
	  * @return The source reference name.
	  * @throws Exception
	  */
	std::string rtransform(const std::string& name)const;
	  
	 
	const git_refspec *constData()const;
	
private:
	const git_refspec *_refspec;
};


/**
 * Represents a Git remote.
 */ 
class Remote : public helper::Git2PtrWrapper<git_remote, git_remote_free>
{
public:
    /**
     * Constructor.
     */
    Remote(git_remote *remote = NULL);
    
    /**
     * Copy constructor; creates a copy of the object, sharing the same underlaying data
     * structure.
     */
    Remote(const Remote& other);

    /**
     * Destructor.
     */
    ~Remote();

    /**
     * Get the remote's name
     */
    std::string name()const;

    /**
     * Get the remote's url
     */
    std::string url()const;

    /**
     * Get the remote's url for pushing
     */
    std::string pushUrl()const;

#if 0 // Removed for upgrading to 0.24.0        
	/**
	 * Set the remote's url
	 *
	 * Existing connections will not be updated.
	 */
	void setUrl(const std::string& url);

	/**
	 * Set the remote's url for pushing
	 *
	 * Existing connections will not be updated.
	 */
	void setPushUrl(const std::string& url);
#endif // Removed for upgrading to 0.24.0


	/**
	 * Get the remote's list of fetch refspecs
	 */
	std::vector<std::string> getFetchRefspec();

	/**
	 * Get the remote's list of push refspecs
	 */
	std::vector<std::string> getPushRefspec();
	
	/**
	 * Get the number of refspecs for a remote
	 */
	size_t refspecCount()const;
	
	/**
	 * Get a refspec from the remote
	 */
	RefSpec getRefspec(size_t n);
	
#if 0 // Removed for upgrading to 0.24.0        
	/**
	 * Remove a refspec from the remote
	 */
	void removeRefspec(size_t n);

	/**
	 * Add a fetch refspec to the remote.
	 * 
	 * Convenience function for adding a single fetch refspec to the current list in the remote.
	 * 
	 * @param refspec the new fetch refspec
	 */
	void addFetch(const std::string& refspec);

	/**
	 * Add a push refspec to the remote.
	 * 
	 * Convenience function for adding a single push refspec to the current list in the remote.
	 * 
	 * @param refspec the new push refspec
	 */
	void addPush(const std::string& refspec);
#endif // Removed for upgrading to 0.24.0

	/**
	 * Open a connection to a remote
	 *
	 * The transport is selected based on the URL. The direction argument
	 * is due to a limitation of the git protocol (over TCP or SSH) which
	 * starts up a specific binary which can only do the one or the other.
	 *
	 * @param direction whether you want to receive or send data
	 * @throws Exception
         * TODO add callback and custom HTTP headers
	 */
	void connect(git_direction direction);

	/**
	 * Check whether the remote is connected
	 *
	 * Check whether the remote's underlying transport is connected to the
	 * remote host.
	 */
	bool isConnected();

	/**
	 * Disconnect from the remote
	 *
	 * Close the connection to the remote and free the underlying
	 * transport.
	 */
	void disconnect();
	
	/**
	 * Cancel the operation
	 *
	 * At certain points in its operation, the network code checks whether
	 * the operation has been cancelled and if so stops the operation.
	 */
	void stop();

#if 0 // Removed for upgrading to 0.24.0
	/**
	 * Get a list of refs at the remote
	 *
	 * The remote (or more exactly its transport) must be connected. The
	 * memory belongs to the remote.
	 *
	 * If you a return false from the callback, this will stop
	 * looping over the refs.
	 */
	bool list(HeadListCallbackFunction callback);

	/**
	 * Download the packfile
	 *
	 * Negotiate what objects should be downloaded and download the
	 * packfile with those objects. The packfile is downloaded with a
	 * temporary filename, as it's final name is not known yet. If there
	 * was no packfile needed (all the objects were available locally),
	 * filename will be NULL and the function will return success.
	 */
	void download(TransfertProgressCallbackFunction callback);

	/**
	 * Update the tips to the new state
	 */
	void UpdateTips();
	
	/**
	 * Return whether a string is a valid remote URL
	 *
	 * @param tranport The url to check
	 */
	static bool isValidUrl(const std::string& url);

	/**
	 * Return whether the passed URL is supported by this version of the library.
	 *
	 * @param tranport The url to check
	 */
	static bool isSupportedUrl(const std::string& url);
	
	/**
	 * Choose whether to check the server's certificate (applies to HTTPS only)
	 */
	void checkCert(bool check);
#endif // Removed for upgrading to 0.24.0
	
	// TODO implement git_remote_set_cred_acquire_cb
	// TODO implement git_remote_set_transport
	// TODO implement git_remote_set_callbacks

	/**
	 * Get the statistics structure that is filled in by the fetch operation.
	 */
	const git_transfer_progress * stats();
	
	/**
	 * Retrieve the tag auto-follow setting
	 */
	git_remote_autotag_option_t autotag()const;
	
#if 0 // Removed for upgrading to 0.24.0
	/**
	 * Set the tag auto-follow setting
	 */
	void setAutotags(git_remote_autotag_option_t value);
	
	/**
	 * Give the remote a new name
	 *
	 * All remote-tracking branches and configuration settings
	 * for the remote are updated.
	 *
	 * The new name will be checked for validity.
	 *
	 * A temporary in-memory remote cannot be given a name with this method.
	 */
	void rename(const std::string& name, RenameProblemCallbackFunction callback);
	
	/**
	 * Retrieve the update FETCH_HEAD setting.
	 */
	int updateFetchhead()const;
	
	/**
	 * Sets the update FETCH_HEAD setting.  By default, FETCH_HEAD will be
	 * updated on every fetch.  Set to 0 to disable.
	 */
	void setUpdateFetchhead(int value);
#endif // Removed for upgrading to 0.24.0
	
	/**
	 * Ensure the remote name is well-formed.
	 */
	static bool isValidName(const std::string& name);
};

} // namespace git2
#endif // _GIT2PP_BLOB_HPP_

