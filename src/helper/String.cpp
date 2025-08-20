#include "String.h"

namespace mb {
namespace helpers {

/** @brief Space */
const std::string SPACE_STRING = " ";

/** @brief Helper function to trim a string */
std::string& trim(std::string& str, const std::string& chars) {
    str.erase(0, str.find_first_not_of(chars, 0));
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

/** @brief Helper function to trim a string */
std::string& ltrim(std::string& str, const std::string& chars) {
    str.erase(0, str.find_first_not_of(chars, 0));
    return str;
}

/** @brief Helper function to trim a string */
std::string& rtrim(std::string& str, const std::string& chars) {
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

/** @brief Helper function to split a string */
std::vector<std::string> split(const std::string& str, char separator) {
    std::vector<std::string> values;
    size_t                   pos       = 0;
    size_t                   first_pos = 0;
    do {
        pos = str.find_first_of(separator, first_pos);
        if (pos != std::string::npos) {
            std::string value = str.substr(first_pos, (pos - first_pos));
            values.push_back(value);

            first_pos = pos + 1;
        }
    } while (pos != std::string::npos);
    std::string value = str.substr(first_pos);
    if (!value.empty()) values.push_back(value);
    return values;
}

/** @brief Helper function to replace a substring with another into a string */
std::string& replace(std::string& str, const std::string& what, const std::string& with, bool replace_all) {
    std::size_t pos       = 0;
    std::size_t start_pos = 0;
    do {
        pos = str.find(what, start_pos);
        if (pos != std::string::npos) {
            str.replace(pos, what.size(), with);
            start_pos = pos + 1u;
        }
    } while ((pos != std::string::npos) && (replace_all));

    return str;
}

/** @brief Helper function to check if a string starts with the specified substring */
bool startsWith(const std::string& str, const std::string& substr) {
    bool ret = false;
    if (str.size() >= substr.size()) ret = (str.find(substr, 0) == 0);
    return ret;
}

/** @brief Helper function to check if a string ends with the specified substring */
bool endsWith(const std::string& str, const std::string& substr) {
    bool ret = false;
    if (str.size() >= substr.size()) {
        size_t start_index = str.size() - substr.size();
        ret                = (str.find(substr, start_index) == start_index);
    }
    return ret;
}

/** @brief Helper function to check if a string is Number */
bool isNumber(const std::string& str) { return isInteger(str) || isUnsignedInteger(str) || isFloat(str); }

/**
 * @brief Helper function to check if a string is Integer
 * @param str String where to check
 * @return true if the string is Number, false otherwise
 */
bool isInteger(const std::string &str) {
    bool result = true;
    bool first = true;
    for (char c : str) {
        if (first) {
            if (c == '-' || c == '+') continue;
            first = false;
        }

        if (!std::isdigit(c)) {
			result = false;
			break;
		}
    }
    return result;
}

/**
 * @brief Helper function to check if a string is Unsigned Integer
 * @param str String where to check
 * @return true if the string is Number, false otherwise
 */
bool isUnsignedInteger(const std::string &str) {
    bool result = true;
    for (char c : str) {
        if (!std::isdigit(c)) {
			result = false;
			break;
		}
    }
    return result;
}

/** @brief Helper function to check if a string is Number */
bool isFloat(const std::string& str) {
    bool result = true;
    const char* c_str = str.c_str();
    bool comma = false; // Флаг наличия десятичной точки
    int i = 0;
    
    // Проверяем знак числа в начале строки
    if (c_str[i] == '-' || c_str[i] == '+') i++;

    if (i > 0 && str.size() < 2) result = false;

    // Проходим по каждому символу строки
    for (; c_str[i] != '\0'; i++) {
        if (c_str[i] == '.') {
            if (comma) {
                result = false; // Более одной точки — ошибка
                break;
            }
            comma = true;
        } 
        else if (!isdigit(c_str[i])) {
            result = false;
            break;
        }
    }

    return result; // Строка должна содержать хотя бы одну цифру
}


/** @brief Helper function to create new upper case string from source string */
std::string getUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
    return result;
}

/** @brief Helper function to uppercase a string */
std::string& toUpperCase(std::string& str) {
   std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	return str;
}

} // namespace helpers
} // namespace mb
