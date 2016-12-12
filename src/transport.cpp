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

#include "transport.hpp"

#include "exception.hpp"

namespace git2
{

//
// Credentials
//

git_cred* Credentials::newUserPassPlainText(const std::string& username, const std::string& password)
{
	git_cred *out;
	Exception::git2_assert(git_cred_userpass_plaintext_new(&out, username.c_str(), password.c_str()));
	return out;
}

git_cred* Credentials::newSSHKeyfilePassphrase(const std::string& publickey, const std::string& privatekey, const std::string& passphrase)
{
	git_cred *out = NULL;
#ifdef GIT_SSH
	Exception::git2_assert(git_cred_ssh_keyfile_passphrase_new(&out, publickey.c_str(), privatekey.c_str(), passphrase.c_str()));
#endif // GIT_SSH
	return out;
}


} // namespace git2

