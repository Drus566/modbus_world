
#ifndef MB_STRING_H
#define MB_STRING_H

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

namespace mb {
namespace helpers {

/** @brief Space */
extern const std::string SPACE_STRING;

/** 
 *  @brief Helper function to trim a string
 *  @param str String to trim
 *  @param chars Characters to remove from the start and from the end of the string
 *  @return Reference to the trimmed string
*/
std::string& trim(std::string& str, const std::string& chars = SPACE_STRING);

/** 
 *  @brief Helper function to trim a string
 *  @param str String to trim
 *  @param chars Characters to remove from the start of the string
 *  @return Reference to the trimmed string
*/
std::string& ltrim(std::string& str, const std::string& chars = SPACE_STRING);

/** 
 *  @brief Helper function to trim a string
 *  @param str String to trim
 *  @param chars Characters to remove from the end of the string
 *  @return Reference to the trimmed string
*/
std::string& rtrim(std::string& str, const std::string& chars = SPACE_STRING);

/** 
 *  @brief Helper function to split a string
 *  @param str String to split
 *  @param separator Separator character
 *  @return Array of splitted substrings
*/
std::vector<std::string> split(const std::string& str, char separator);

/** 
 *  @brief Helper function to replace a substring with another into a string
 *  @param str String where replace
 *  @param what Substring to search
 *  @param with Substring to use as a replacement
 *  @param replace_all If true, replace all occurences of the [what] substring, otherwise replace only the first one
 *  @return Reference to [str]
 */
std::string& replace(std::string& str, const std::string& what, const std::string& with, bool replace_all = true);

/** 
 * @brief Helper function to check if a string starts with the specified substring
 * @param str String where to check
 * @param substr Substring to look for
 * @return true if the string starts with the specified substring, false otherwise
 */
bool startsWith(const std::string& str, const std::string& substr);

/** 
 * @brief Helper function to check if a string ends with the specified substring
 * @param str String where to check
 * @param substr Substring to look for
 * @return true if the string ends with the specified substring, false otherwise
 */
bool endsWith(const std::string& str, const std::string& substr);

/** 
 * @brief Helper function to check if a string is Number
 * @param str String where to check
 * @return true if the string is Number, false otherwise
 */
bool isNumber(const std::string& str);

/**
 * @brief Helper function to check if a string is Integer
 * @param str String where to check
 * @return true if the string is Number, false otherwise
 */
bool isInteger(const std::string &str);

/**
 * @brief Helper function to check if a string is Unsigned Integer
 * @param str String where to check
 * @return true if the string is Number, false otherwise
 */
bool isUnsignedInteger(const std::string &str);

/**
 * @brief Helper function to check if a string is Float
 * @param str String where to check
 * @return true if the string is Number, false otherwise
 */
bool isFloat(const std::string& str);

/** 
 * @brief Helper function to create new upper case string from source string
 * @param str String where to upper
 * @return New string with upper cases, empty new string otherwise 
 */
std::string getUpperCase(const std::string& str);

/** 
 *  @brief Helper function to uppercase a string
 *  @param str String to uppercase
 *  @return Reference to the uppercase string
*/
std::string& toUpperCase(std::string& str);



} // namespace helpers
} // namespace mb

#endif // MB_STRING_H
