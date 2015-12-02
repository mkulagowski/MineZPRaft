/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  UTF-related functions implementation.
 */

#ifndef __COMMON_UTFFUNCS_HPP__
#define __COMMON_UTFFUNCS_HPP__

#include <vector>

/**
 * Convert from UTF16 to UTF8
 *
 * @param s String in UTF16
 * @return String in UTF8
 */
std::string UTF16ToUTF8(const std::wstring &s);

/**
 * Convert from UTF8 to UTF16
 *
 * @param s String in UTF8
 * @return String in UTF16
 */
std::wstring UTF8ToUTF16(const std::string &s);

#endif // __COMMON_UTFFUNCS_HPP__