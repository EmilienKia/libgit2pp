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

#ifndef _GIT2PP_CONFIG_HPP_
#define _GIT2PP_CONFIG_HPP_

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
	enum Level {
		LEVEL_SYSTEM = GIT_CONFIG_LEVEL_SYSTEM,
		LEVEL_XDG = GIT_CONFIG_LEVEL_XDG,
		LEVEL_GLOBAL = GIT_CONFIG_LEVEL_GLOBAL,
		LEVEL_LOCAL = GIT_CONFIG_LEVEL_LOCAL,
		LEVEL_APP = GIT_CONFIG_LEVEL_APP,
		LEVEL_HIGHEST = GIT_CONFIG_HIGHEST_LEVEL,
	};

    /**
      * Default constructor to create a new configuration object.
      *
      * @param cfg when given, the instance is used instead of creating a new one
      */
    Config(git_config *cfg = 0);
    Config(const Config &other);
    virtual ~Config();

	/**
	 * Open the global, XDG and system configuration files.
	 */
	 static Config openDefaultConfig();

    /**
      * Creates a new configuration object and adds the global Git configuration when found.
      * Otherwise an empty configuration object is created.
      *
      * @return the new instance
      */
    static Config openGlobalConfig();
    
    /**
     * Build a single-level focused config object from a multi-level one.
     * 
     * The returned config object can be used to perform get/set/delete
     * operations on a single specific level.
     * 
     * Getting several times the same level from the same parent
     * multi-level config will return different config instances,
     * but containing the same config_file instance.
     */
    static Config openLevel(const Config& parent, Level level);

    /**
     * Create a new config instance containing a single on-disk file.
     */
    static Config openOnDisk(const std::string& path);
    
    
    /**
     * Parse a string value as a bool.
     * 
     * Valid values for true are: 'true', 'yes', 'on', 1 or any number
     * different from 0 Valid values for false are: 'false', 'no', 'off', 0
     */
    static bool parseBool(const std::string& value);

    /**
     * Parse a string value as an int32.
     * 
     * An optional value suffix of 'k', 'm', or 'g' will cause the value
     * to be multiplied by 1024, 1048576, or 1073741824 prior to output.
     */
    static int32_t parseInt32(const std::string& value);

    /**
     * Parse a string value as an int64.
     * 
     * An optional value suffix of 'k', 'm', or 'g' will cause the value
     * to be multiplied by 1024, 1048576, or 1073741824 prior to output.
     */
    static int64_t parseInt64(const std::string& value);

	/**
	 * Reload changed config files
	 *
	 * A config file may be changed on disk out from under the in-memory
	 * config object.  This function causes us to look for files that have
	 * been modified since we last loaded them and refresh the config with
	 * the latest information.
	 */
	void refresh();

	/**
	 * Add an on-disk config file instance to an existing config
	 *
	 * The on-disk file pointed at by `path` will be opened and
	 * parsed; it's expected to be a native Git config file following
	 * the default Git config syntax (see man git-config).
      *
      * @param path the absolute path to the config file
      * @param level the priority level of the backend
      * @param force replace config file at the given priority level
      *
      * @return true on success
      */
    bool addFile(const std::string& path, Level level, bool force);


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
    int64_t get(const std::string &key, int64_t defaultValue) const;

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
     * Delete a config variable from the config file with the highest level (usually the local one).
     *
     * @param name the variable to delete.
     * @throws Exception
     */
    void deleteEntry(const std::string &name);

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
    
	/**
	 * Locate the path to the global xdg compatible configuration file
	 *
	 * The xdg compatible configuration file is usually
	 * located in $HOME/.config/git/config.
	 *
	 * This method will try to guess the full path to that
	 * file, if the file exists. The returned path
	 * may be used on any git_config call to load the xdg
	 * compatible configuration file.
	 *
	 * @return XDG configuration path.
	 */
    static std::string findXdg();


	git_config * data();
	const git_config * constData()const;
	
private:
    git_config * _conf; //!< internal pointer to the libgit2 config instance
};

} // namespace git2
#endif // _GIT2PP_CONFIG_HPP_

