#include "GeneralUtility.h"

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef WIN32
    #include <Windows.h>
    #include <shellapi.h>
    #include <Shlwapi.h>
    #include <shlobj.h>

    #pragma comment(lib, "shlwapi.lib")
    #pragma comment(lib, "Shell32.lib")
#endif

namespace SprueEngine
{

    std::vector<std::string>& Split(const std::string&s, char delim, std::vector<std::string>& elems)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim))
        {
            if (!IsWhitespace(item))
                elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> Split(const std::string& s, char delim)
    {
        std::vector<std::string> elems;
        Split(s, delim, elems);
        return elems;
    }

    bool FileAccessible(const std::string& file)
    {
        std::ifstream strm(file);
        return strm.good();
    }

    std::string ReadFile(const std::string& file)
    {
        std::ifstream strm(file);
        // Check for accessibility
        if (!strm.good())
            return std::string();

        std::stringstream buffer;
        buffer << strm.rdbuf();
        return buffer.str();
    }

    std::string ToXmlSafe(const std::string& str)
    {
        std::string ret;
        ret.reserve(str.length());

        for (unsigned i = 0; i < str.length(); ++i)
        {
            if (!iswspace(str[i]) && str[i] != '.')
            {
                if (str[i] == '<')
                    ret.push_back('L');
                else if (str[i] == '>')
                    ret.push_back('G');
                else
                    ret.push_back(str[i]);
            }
        }
        return ret;
    }

    char* SkipUntil(char* value, char stopValue)
    {
        while (*value != stopValue && *value != '\0')
            ++value;
        return value;
    }
    
    char* SkipUntilAfter(char* value, char stopValue)
    {
        while (*value != stopValue  && *value != '\0')
            ++value;
        if (*value != '\0')
            ++value;
        return value;
    }

    char* SkipUntil(char* value, std::set<char> stopValues)
    {
        while (*value != '\0' && stopValues.find(*value) == stopValues.end())
            ++value;
        return value;
    }

    char* SkipUntilAfter(char* value, std::set<char> stopValues)
    {
        while (*value != '\0' && stopValues.find(*value) == stopValues.end())
            ++value;
        if (*value != '\0')
            ++value;
        return value;
    }

    std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
        std::string ret = str;
        if (from.empty())
            return ret;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            ret.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }

        return ret;
    }

    std::string SanitizePath(const std::string& path)
    {
#ifdef WIN32
        return replaceAll(path, "/", "\\");
#endif
    }

    std::string MakeRelativePath(const std::string& path, const std::string& relativeTo)
    {
#ifdef WIN32
        char output[MAX_PATH];
        memset(output, 0, sizeof output);
        if (PathRelativePathToA(output, SanitizePath(relativeTo).c_str(), FILE_ATTRIBUTE_DIRECTORY, SanitizePath(path).c_str(), FILE_ATTRIBUTE_NORMAL))
            return output;
        return path;
#endif
    }

    std::string MakeAbsolutePath(const std::string& subPath, const std::string& relativeTo)
    {
#ifdef WIN32
        std::string workingPath = relativeTo + "/" + subPath;
        char output[MAX_PATH];
        memset(output, 0, sizeof output);
        PathCanonicalizeA(output, SanitizePath(workingPath).c_str());
        return output;
#endif
    }

    std::string FolderOf(const std::string& path)
    {
#ifdef WIN32
        char output[MAX_PATH];
        memset(output, 0, sizeof(output));
        memcpy(output, path.c_str(), path.length());
        if (PathRemoveFileSpecA(output))
            return output;
        return path;
#endif
    }

    std::string FileName(const std::string& path)
    {
#ifdef WIN32
        return PathFindFileNameA(path.c_str());
#endif
    }

    bool IsPathRooted(const std::string& path)
    {
        if (path.length() > 2 && path[1] == ':')
            return true;
        return false;
    }

    std::string ReplaceString(const std::string& src, const std::string& term, const std::string& replacement)
    {
        std::string ret = src;
        size_t start_pos = 0;
        while ((start_pos = ret.find(term, start_pos)) != std::string::npos) {
            size_t end_pos = start_pos + term.length();
            ret.replace(start_pos, end_pos, replacement);
            start_pos += replacement.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        return ret;
    }
}