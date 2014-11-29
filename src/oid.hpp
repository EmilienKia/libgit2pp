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

#ifndef _GIT2PP_OID_HPP_
#define _GIT2PP_OID_HPP_

#include <git2.h>

#include <string>
#include <vector>

namespace git2
{


/**
 * This class represent a Git SHA1 id, i.e. 40 hexadecimal digits.
 */
class OId
{
public:
	/** Constructor */
	OId(const git_oid *oid = NULL);

	/** Copy constructor */
	OId(const OId& oid);

	/** Destructor */
	~OId();

    /**
     * Checks if this is a valid Git OId.
	 * An OId is invalid if it is empty or 0x0000... (20 byte).
     * @return True, if the OId is valid. False if not.
     */
    bool isValid() const;
	

    git_oid* data();
    const git_oid* constData() const;

	/**
	 * Returns the length of the OId as a number of hexadecimal characters.
	 *
	 * The full length of a OId is 40, but OId represented by this class may be shorter.
     */
	int length() const;

    /**
     * Set the value of the object parsing a hex array.
     *
     * This method parses an array of hexadecimal values trying to
     * convert it into an OId.
     * If the array contains more than 40 values, only the first 40
     * will be converted.
     * If the array contains less than 40 values the resulting OId
     * will be a shortened OId, or a prefix.
     *
     * @param hex
     * Input hex string; must be at least 4 bytes.
     * @throws Exception
     */
	void fromHex(const std::vector<char>& hex);

    /**
     * Set the value of the object parsing a string.
     *
     * This method behaves just like fromHex() but parses a string
     * that contains hexadecimal values. The same rules of fromHex()
     * apply here.
     *
     * @param str Input string; must be at least 4 characters long.
     *
     * @throws Exception
     */
    void fromString(const std::string& str);

    /**
     * Set the value of the object from a raw oid.
     *
     * This method uses the input raw hexadecimal array without parsing
     * it and without performing prefix lookup. The raw array must be
     * 40 characters long, otherwise trucate.
     *
     * @param raw the raw input bytes to be copied.
     */
    void fromRawData(const std::vector<unsigned char>& raw);

    /**
     * Format a OId into a hex string.
     */
    std::string format() const;

    /**
     * Format a git_oid into a loose-object path string.
     *
     * The resulting string is "aa/...", where "aa" is the first two
     * hex digitis of the oid and "..." is the remaining 38 digits.
     */
    std::string pathFormat() const;

	/**
	 * Check is an oid is all zeros.
	 * 
	 * @return True if OId is all zeros.
	 */
	bool isZero() const;
	
    /**
     * Parse a hex formatted object id into a OId.
     *
     * @param hex
     * Input hex array; if less than 40 bytes, prefix lookup will be performed. Must be
     * at least 4 bytes.
     *
     * @return OId; null OId on failure.
     * @throws Exception
     */
    static OId hexToOid(const std::vector<char>& hex);

    /**
     * Parse a hex formatted object id into a OId.
     *
     * @param str
     * Input hex string; if less than 40 bytes, prefix lookup will be performed. Must be
     * at least 4 bytes.
     *
     * @return OId; null OId on failure.
     * @throws Exception
     */
    static OId stringToOid(const std::string& str);

    /**
     * Copy an already raw oid into a git_oid structure.
     * @param raw the raw input bytes to be copied.
     */
    static OId rawDataToOid(const std::vector<unsigned char>& raw);

	// TODO Should implement oid shorten related functions ?
	
private:
	std::vector<unsigned char> _oid;
};

/**
 * Compare two OIds.
 */
bool operator ==(const OId &oid1, const OId &oid2);

/**
 * Compare two OIds.
 */
bool operator !=(const OId &oid1, const OId &oid2);

/**
 * Compare two OIds.
 */
bool operator >(const OId &oid1, const OId &oid2);

/**
 * Compare two OIds.
 */
bool operator <(const OId &oid1, const OId &oid2);

/**
 * Compare two OIds.
 */
bool operator >=(const OId &oid1, const OId &oid2);

/**
 * Compare two OIds.
 */
bool operator <=(const OId &oid1, const OId &oid2);

/**
 * Compare an OId with a string.
 */
bool operator == (const OId &oid, const std::string &str);

/**
 * Compare an OId with a string.
 */
bool operator == (const OId &oid, const char* str);


} // namespace git2
#endif // _GIT2PP_OID_HPP_

