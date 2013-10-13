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
    static Config openGlobalConfig();

	/**
	 * Add an on-disk config file instance to an existing config
	 *
	 * The on-disk file pointed at by `path` will be opened and
	 * parsed; it's expected to be a native Git config file following
	 * the default Git config syntax (see man git-config).
      *
      * @param path the absolute path to the config file
      * @param priority the access priority; values with higher priority are accessed first
      *
      * @return true on success
      */
    bool addFile(const std::string& path, int priority);


    /**
     * Reads a single string value from the configuration.
     *
	 * @param key Variable name
	 * @param value Reference where store the value.
     * @return true if the value exists, false otherwise.
     */
    bool get(const std::string &key, std::string& value) const;
	
    /**
     * Reads a single string value from the configuration.
     *
	 * @param key Variable name
	 * @param defaultValue Default value to return if not available. 
     * @return The value if available else the default value.
     */
    std::string get(const std::string &key, const std::string &defaultValue = "") const;

    /**
     * Writes a value in the configuration with the highest priority.
     *
     * @param key the name of the value to write
     * @param value the value
     * @throws Exception
     */
    void set(const std::string &key, const std::string &value);

    /**
     * Reads a single integer value from the configuration.
     *
	 * @param key Variable name
	 * @param value Pointer where store the value.
     * @return true if the value exists, false otherwise.
     */
    bool get(const std::string &key, int32_t* value) const;
	
    /**
     * Reads a single integer value from the configuration.
     *
	 * @param key Variable name
	 * @param defaultValue Default value to return if not available. 
     * @return The value if available else the default value.
     */
    int32_t get(const std::string &key, int32_t defaultValue = 0) const;

    /**
     * Writes a value in the configuration with the highest priority.
     *
     * @param key the name of the value to write
     * @param value the value
     * @throws Exception
     */
    void set(const std::string &key, int32_t value);

    /**
     * Reads a single integer value from the configuration.
     *
	 * @param key Variable name
	 * @param value Pointer where store the value.
     * @return true if the value exists, false otherwise.
     */
    bool get(const std::string &key, int64_t* value) const;
	
    /**
     * Reads a single integer value from the configuration.
     *
	 * @param key Variable name
	 * @param defaultValue Default value to return if not available. 
     * @return The value if available else the default value.
     */
    int32_t get(const std::string &key, int64_t defaultValue = 0) const;

    /**
     * Writes a value in the configuration with the highest priority.
     *
     * @param key the name of the value to write
     * @param value the value
     * @throws Exception
     */
    void set(const std::string &key, int64_t value);

    /**
     * Reads a single boolean value from the configuration.
     *
	 * @param key Variable name
	 * @param value Pointer where store the value.
     * @return true if the value exists, false otherwise.
     */
    bool get(const std::string &key, bool* value) const;
	
    /**
     * Reads a single boolean value from the configuration.
     *
	 * @param key Variable name
	 * @param defaultValue Default value to return if not available. 
     * @return The value if available else the default value.
     */
    int32_t get(const std::string &key, bool defaultValue = false) const;

    /**
     * Writes a value in the configuration with the highest priority.
     *
     * @param key the name of the value to write
     * @param value the value
     * @throws Exception
     */
    void set(const std::string &key, bool value);	

    /**
     * Remove a value from the configuration.
     *
     * @param key the name for the value to remove
     * @throws Exception
     */
    void remove(const std::string &key);

	// TODO Add multivar get and set (git_config_get_multivar and git_config_set_multivar)
	// TODO Add operation on each variable (git_config_foreach())
	// TODO Add get on variable map (git_config_get_mapped())

	
	/**
	 * Locate the path to the global configuration file
	 *
	 * The user or global configuration file is usually
	 * located in `$HOME/.gitconfig`.
	 *
	 * This method will try to guess the full path to that
	 * file, if the file exists. The returned path
	 * may be used on any `git_config` call to load the
	 * global configuration file.
	 *
	 * @return Global configuration path.
	 */
    static std::string findGlobal();

	/**
	 * Locate the path to the system configuration file
	 *
	 * If /etc/gitconfig doesn't exist, it will look for
	 * %PROGRAMFILES%\Git\etc\gitconfig.
	 * 
	 * @return System configuration path.
	 */
    static std::string findSystem();


	git_config * data();
	const git_config * constData();
	
private:
    git_config * _conf; //!< internal pointer to the libgit2 config instance
};

} // namespace git2
#endif // _CONFIG_HPP_

