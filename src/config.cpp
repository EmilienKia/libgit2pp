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

#include "config.hpp"

#include "exception.hpp"

namespace git2
{

Config::Config(git_config *cfg):
_conf(cfg)
{
    if (_conf == NULL)
        git_config_new(&_conf);
}

Config::Config(const Config &other):
_conf(other._conf)
{
}

Config::~Config()
{
    git_config_free(_conf);
}

Config Config::fromGlobalConfig()
{
//    git_config * def;
    git_config * cfg;
//TODO    git_config_open_default(&def); // Not available before v0.18
    if ( git_config_open_global(&cfg/*, def TODO Not available before v0.17*/ ) == GIT_OK )
        return Config(cfg);

    return Config();
}

std::string Config::findGlobal()
{
    char buffer[GIT_PATH_MAX];
    Exception::assert(git_config_find_global(buffer, GIT_PATH_MAX) );
    return std::string(buffer);
}

std::string Config::findSystem()
{
    char buffer[GIT_PATH_MAX];
    Exception::assert( git_config_find_system(buffer, GIT_PATH_MAX) );
    return std::string(buffer);
}

/*TODO bool Config::append(const std::string &path, git_config_level_t level, int force)
{
    return GIT_OK == git_config_add_file_ondisk(_conf, path.c_str(), level, force);
}*/

std::string Config::value(const std::string &key, const std::string &defaultValue) const
{
    const char * result = 0;
    if (git_config_get_string(&result, _conf, key.c_str()) == GIT_OK)
        return std::string(result);

    return defaultValue;
}

void Config::setValue(const std::string &key, const std::string &value)
{
    Exception::assert( git_config_set_string(_conf, key.c_str(), value.c_str()) );
}


} // namespace git2

