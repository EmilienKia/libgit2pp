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

#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <git2.h>

#include <string>

namespace git2
{

class Repository;

/**
  * Represents the git configuration file.
  */
class Config
{
public:
    /**
      * Default constructor to create a new configuration object.
      *
      * @param cfg when given, the instance is used instead of creating a new one
      */
    Config(git_config *cfg = 0);
    Config(const Config &other);
    virtual ~Config();

    /**
      * Creates a new configuration object and adds the global Git configuration when found.
      * Otherwise an empty configuration object is created.
      *
      * @return the new instance
      */
    static Config fromGlobalConfig();

    /**
      * Appends a config file with the given access priority.
      *
      * @param path the absolute path to the config file
      * @param priority the access priority; values with higher priority are accessed first
      *
      * @return true on success
      */
//TODO    bool append(const std::string& path, git_config_level_t level, int force);

    /**
      * Reads a single value from the configuration.
      *
      * @return the value as QVariant or an empty QVariant instance
      */
    std::string value(const std::string &key, const std::string &defaultValue = "") const;

    /**
      * Writes a value in the configuration with the highest priority.
      *
      * @param key the name of the value to write
      * @param value the value
      *
      * @todo handle the QVariant type correctly
      */
    void setValue(const std::string &key, const std::string &value);

    /**
      * Remove a value from the configuration.
      *
      * @param key the name for the value to remove
      */
    void remove(const std::string &key);

public:
    /**
      * Searches for the global configuration file located in $HOME.
      * @see git_config_find_global
      */
    static std::string findGlobal();

    /**
      * Searches for the system configuration file.
      * @see git_config_find_system
      */
    static std::string findSystem();

private:
    git_config * _conf; //!< internal pointer to the libgit2 config instance
};

} // namespace git2
#endif // _CONFIG_HPP_

