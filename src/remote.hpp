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

#ifndef _REMOTE_HPP_
#define _REMOTE_HPP_

#include <git2.h>

#include <string>

namespace git2
{

class Exception;

class Remote
{
public:

    /**
     * Constructor.
     */
    Remote(git_remote *remote = NULL);

	/**
	 * Destructor.
	 */
	~Remote();

	/**
	 * Save a remote to its repository's configuration
	 *
	 * @throws Exception
	 */
	void save();

	/**
	 * Get the remote's name
	 */
	std::string name();

	/**
	 * Get the remote's url
	 */
	std::string url();

	/**
	 * Set the remote's fetch refspec
	 *
	 * @param spec the new fetch refspec
	 * @throws Exception
	 */
	void setFetchSpec(const std::string& spec);

	/**
	 * Get the fetch refspec
	 *
	 * @return a pointer to the fetch refspec or NULL if it doesn't exist
	 */
	// TODO RefSpec* fetchSpec();

	/**
	 * Set the remote's push refspec
	 *
	 * @param spec the new push refspec
	 * @throws Exception
	 */
	void setPushSpec(const std::string& spec);

	/**
	 * Get the push refspec
	 *
	 * @return a pointer to the push refspec or NULL if it doesn't exist
	 */
	// TODO RefSpec* pushSpec();

	/**
	 * Open a connection to a remote
	 *
	 * The transport is selected based on the URL. The direction argument
	 * is due to a limitation of the git protocol (over TCP or SSH) which
	 * starts up a specific binary which can only do the one or the other.
	 *
	 * @param direction whether you want to receive or send data
	 * @throws Exception
	 */
	void connect(int direction);

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

	// TODO Implement git_remote_ls
	// TODO Implement git_remote_download
	// TODO Implement git_remote_update_tips
	
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
	
	git_remote* data() const;
    const git_remote* constData() const;
private:
	git_remote *_remote;
};

} // namespace git2
#endif // _BLOB_HPP_

