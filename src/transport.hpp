/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * libgit2pp
 * Copyright (C) 2015 Émilien Kia <emilien.kia@gmail.com>
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

#ifndef _GIT2PP_TRANSPORT_HPP_
#define _GIT2PP_TRANSPORT_HPP_

#include <git2.h>

#include "object.hpp"

#include <string>

namespace git2
{


class Credentials
{
public:

	/**
	 * Creates a new plain-text username and password credential object.
	 * The supplied credential parameter will be internally duplicated.
	 * 
	 * @param username The username of the credential.
	 * @param password The password of the credential.
	 */
	static git_cred* newUserPassPlainText(const std::string& username, const std::string& password);

	/**
	 * Creates a new ssh key file and passphrase credential object.
	 * The supplied credential parameter will be internally duplicated.
	 *
	 * @param publickey The path to the public key of the credential.
	 * @param privatekey The path to the private key of the credential.
	 * @param passphrase The passphrase of the credential.
	 */
	static git_cred* newSSHKeyfilePassphrase(const std::string& publickey, const std::string& privatekey, const std::string& passphrase);

	// TODO Wrap git_cred_ssh_publickey_new
};


typedef std::function<git_cred*(const std::string& url, const std::string& usernameFromUrl, unsigned int allowedTypes)> CredentialsAcquireCallbackFunction;


} // namespace git2
#endif // _GIT2PP_TRANSPORT_HPP_

