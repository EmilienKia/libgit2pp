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

Config Config::openDefaultConfig()
{
    git_config * def;
    git_config_open_default(&def); 
    return Config(def);
}

Config Config::openGlobalConfig()
{
    git_config * def;
    git_config * cfg;
    git_config_open_default(&def); 
    if ( git_config_open_global(&cfg, def) == GIT_OK )
        return Config(cfg);

    return Config();
}

Config Config::openLevel(const Config& parent, Level level)
{
    git_config * conf;
    git_config_open_level(&conf, parent.constData(), (git_config_level_t)level);
    return Config(conf);
}

Config Config::openOnDisk(const std::string& path)
{
    git_config * conf;
    git_config_open_ondisk(&conf, path.c_str());
    return Config(conf);
}

bool Config::parseBool(const std::string& value)
{
    int res;
    Exception::assert( git_config_parse_bool(&res, value.c_str()) );
    return res != 0;
}

int32_t Config::parseInt32(const std::string& value)
{
    int32_t res;
    Exception::assert( git_config_parse_int32(&res, value.c_str()) );
    return res;
}

int64_t Config::parseInt64(const std::string& value)
{
    int64_t res;
    Exception::assert( git_config_parse_int64(&res, value.c_str()) );
    return res;
}

void Config::refresh()
{
    Exception::assert( git_config_refresh(data()) );
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

std::string Config::findXdg()
{
    char buffer[GIT_PATH_MAX];
    Exception::assert( git_config_find_xdg(buffer, GIT_PATH_MAX) );
    return std::string(buffer);
}


bool Config::addFile(const std::string &path, Level level, bool force)
{
    return git_config_add_file_ondisk(_conf, path.c_str(), (git_config_level_t)level, force ? 1 : 0) == GIT_OK;
}

bool Config::get(const std::string &key, std::string& value) const
{
    const char * result = NULL;
    if (git_config_get_string(&result, _conf, key.c_str()) == GIT_OK)
	{
		value.assign(result);
        return true;
	}
	else
	{
		return false;
	}
}

std::string Config::get(const std::string &key, const std::string &defaultValue) const
{
    const char * result = NULL;
    if (git_config_get_string(&result, _conf, key.c_str()) == GIT_OK)
        return std::string(result);

    return defaultValue;
}

void Config::set(const std::string &key, const std::string &value)
{
    Exception::assert( git_config_set_string(_conf, key.c_str(), value.c_str()) );
}



bool Config::get(const std::string &key, int32_t* value) const
{
    return git_config_get_int32(value, _conf, key.c_str()) == GIT_OK;
}

int32_t Config::get(const std::string &key, int32_t defaultValue) const
{
    int32_t result = 0;
    if (git_config_get_int32(&result, _conf, key.c_str()) == GIT_OK)
        return result;
    return defaultValue;
}

void Config::set(const std::string &key, int32_t value)
{
	Exception::assert( git_config_set_int32(_conf, key.c_str(), value) );	
}

bool Config::get(const std::string &key, int64_t* value) const
{
    return git_config_get_int64(value, _conf, key.c_str()) == GIT_OK;
}

int64_t Config::get(const std::string &key, int64_t defaultValue) const
{
    int64_t result = 0;
    if (git_config_get_int64(&result, _conf, key.c_str()) == GIT_OK)
        return result;
    return defaultValue;
}

void Config::set(const std::string &key, int64_t value)
{
	Exception::assert( git_config_set_int64(_conf, key.c_str(), value) );	
}



bool Config::get(const std::string &key, bool* value) const
{
	int result = 0;
    if(git_config_get_bool(&result, _conf, key.c_str()) == GIT_OK)
	{
		*value = result;
		return true;
	}
	else
		return false;
}

int32_t Config::get(const std::string &key, bool defaultValue) const
{
    int result = 0;
    if (git_config_get_bool(&result, _conf, key.c_str()) == GIT_OK)
	{
        return result;
	}
    return defaultValue;
}

void Config::set(const std::string &key, bool value)
{
	Exception::assert( git_config_set_bool(_conf, key.c_str(), value?1:0) );
}


void Config::deleteEntry(const std::string &name)
{
	Exception::assert( git_config_delete_entry(_conf, name.c_str()) );	
}

git_config * Config::data()
{
	return _conf;
}

const git_config * Config::constData()const
{
	return _conf;
}

} // namespace git2

