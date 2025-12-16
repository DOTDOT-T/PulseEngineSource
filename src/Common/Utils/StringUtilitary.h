#ifndef __STRINGUTILITARY_H__
#define __STRINGUTILITARY_H__

#include <string>

class StringUtilitary
{
public:
    /// Remove leading and trailing whitespace
    static inline std::string Trim(const std::string &str)
    {
        const auto strBegin = str.find_first_not_of(" \t\n\r");
        if (strBegin == std::string::npos)
            return ""; // no content

        const auto strEnd = str.find_last_not_of(" \t\n\r");
        return str.substr(strBegin, strEnd - strBegin + 1);
    }

    /// Convert string to lowercase
    static inline std::string ToLower(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    /// Convert string to uppercase
    static inline std::string ToUpper(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    /// Check if string starts with a prefix
    static inline bool StartsWith(const std::string &str, const std::string &prefix)
    {
        return str.size() >= prefix.size() &&
               str.compare(0, prefix.size(), prefix) == 0;
    }

    /// Check if string ends with a suffix
    static inline bool EndsWith(const std::string &str, const std::string &suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    /// Remove prefix if present
    static inline std::string RemovePrefix(const std::string &str, const std::string &prefix)
    {
        std::string normStr = NormalizePath(str);
        std::string normPre = NormalizePath(prefix);

        if (StartsWith(normStr, normPre))
            return normStr.substr(normPre.size());
        return normStr;
    }

    /// Remove suffix if present
    static inline std::string RemoveSuffix(const std::string &str, const std::string &suffix)
    {
        std::string normStr = NormalizePath(str);
        std::string normPre = NormalizePath(prefix);
        if (EndsWith(normStr, normPre))
            return normStr.substr(0, normStr.size() - normPre.size());
        return normStr;
    }

    /// Normalize path separators to '/' (cross-platform)
    static inline std::string NormalizePath(const std::string &path)
    {
        std::string result = path;
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    }
};

#endif // __STRINGUTILITARY_H__