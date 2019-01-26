#pragma once

#include <algorithm>
#include <ctype.h>
#include <set>
#include <string>
#include <sstream>
#include <vector>

namespace SprueEngine
{

/// by Lukasz Wiklendt
/// http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
template <typename T>
std::vector<size_t> sort_indexes_less(const std::vector<T> &v) {

    // initialize original index locations
    std::vector<size_t> idx(v.size());
    for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

    // sort indexes based on comparing values in v
    std::sort(idx.begin(), idx.end(),
        [&v](size_t i1, size_t i2) {return v[i1] < v[i2]; });

    return idx;
}

/// 
template <typename T>
std::vector<size_t> cage_sort(const std::vector<T> &v) {

    // initialize original index locations
    std::vector<size_t> idx(v.size());
    for (size_t i = 0; i != idx.size(); ++i) 
        idx[i] = i;

    // sort indexes based on comparing values in v
    std::sort(idx.begin(), idx.end(),
        [&v](size_t i1, size_t i2) {
            return v[i1] < v[i2]; 
        }
    );

    return idx;
}

/// Returns the value sorted list of indices into the given vector.
template <typename T>
std::vector<size_t> sort_indexes_greater(const std::vector<T> &v) {

    // initialize original index locations
    std::vector<size_t> idx(v.size());
    for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

    // sort indexes based on comparing values in v
    std::sort(idx.begin(), idx.end(),
        [&v](size_t i1, size_t i2) {return v[i1] > v[i2]; });

    return idx;
}

/// Tests if a string is comprised entirely of whitespace.
inline bool IsWhitespace(const std::string& str)
{
    if (str.empty())
        return true;
    for (unsigned i = 0; i < str.length(); ++i)
    {
        if (str[i] != ' ' && str[i] != '\t')
            return true;
    }
    return false;
}

template<typename T>
void DeleteVector(std::vector<T*>& vec)
{
    for (unsigned i = 0; i < vec.size(); ++i)
        delete vec[i];
    vec.clear();
}

/// Tests if a string starts with the given string.
inline bool StartsWith(const std::string& str, const std::string& test)
{
    return str.compare(0, test.length(), test) == 0;
}

/// Tests if a string ends with the given string.
inline bool EndsWith(const std::string& str, const std::string& other)
{
    if (str.length() >= other.length())
        return 0 == str.compare(str.length() - other.length(), other.length(), other);
    else
        return false;
}

/// Converts all characters into lowercase.
inline std::string ToLower(const std::string& str)
{
    std::string ret = str;
    for (unsigned i = 0; i < ret.length(); ++i)
        ret[i] = tolower(ret[i]);
    return ret;
}

/// Converts all characters into uppercase.
inline std::string ToUpper(const std::string& str)
{
    std::string ret = str;
    for (unsigned i = 0; i < ret.length(); ++i)
        ret[i] = toupper(ret[i]);
    return ret;
}

/// Remove all whitespace before a string.
inline std::string TrimLeft(const std::string& str)
{
    return str.substr(str.find_first_not_of(" \f\n\r\t\v"));
}

/// Remove all whitespace after a string.
inline std::string TrimRight(const std::string& str)
{
    return str.substr(0, str.find_last_not_of(" \f\n\r\t\v") + 1);
}

/// Remove all whitespace before and after a string.
inline std::string Trim(const std::string& str)
{
    return TrimLeft(TrimRight(str));
}

/// Split the given string by the given delimiter.
std::vector<std::string>& Split(const std::string&s, char delim, std::vector<std::string>& elems);

/// Split the given string by the given delimiter.
std::vector<std::string> Split(const std::string& s, char delim);

bool FileAccessible(const std::string& file);

/// Reads a file into a single string (using STL).
std::string ReadFile(const std::string& file);

/// Converts a string into a new string that is safe to use as an XML node/attribute name.
std::string ToXmlSafe(const std::string& str);

/// Skips over characters until the given stop value is hit, returned pointer will be AT that value. Null terminator checked.
char* SkipUntil(char* value, char stopValue);

/// Skips over characters until the given stop value is hit, returned pointer will be AFTER that value. Null terminator checked.
char* SkipUntilAfter(char* value, char stopValue);

/// Skips over characters until one of the given stop values is hit, returned pointer will be AT that value. Null terminator checked.
char* SkipUntil(char* value, std::set<char> stopValues);

/// Skips over characters until one of the given stop values is hit, returned pointer will be AFTER that value. Null terminator checked.
char* SkipUntilAfter(char* value, std::set<char> stopValues);

std::string MakeRelativePath(const std::string& path, const std::string& relativeTo);

std::string MakeAbsolutePath(const std::string& subPath, const std::string& relativeTo);

std::string FolderOf(const std::string& path);

std::string FileName(const std::string& path);

bool IsPathRooted(const std::string& path);

std::string ReplaceString(const std::string& src, const std::string& term, const std::string& replacement);

}