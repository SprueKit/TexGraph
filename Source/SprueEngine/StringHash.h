#pragma once

#include <SprueEngine/ClassDef.h>
#include <cstdint>
#include <string>

namespace SprueEngine
{

/// FNV-1a hashed string, suitable as keys in containers
struct SPRUE StringHash
{
    unsigned value_;

    StringHash(const char* str) : value_(fnv1a(str)) { }
    StringHash(const std::string& str) : value_(fnv1a(str)) { }
    StringHash(unsigned value = -1) : value_(value) { }

    bool operator==(unsigned rhs) const { return value_ == rhs; }
    bool operator==(const StringHash& rhs) const { return value_ == rhs.value_; }
    bool operator!=(unsigned rhs) const { return value_ != rhs; }
    bool operator!=(const StringHash& rhs) const { return value_ != rhs.value_; }

    bool operator<(const StringHash& rhs) const { return value_ < rhs.value_; }
    bool operator>(const StringHash& rhs) const { return value_ > rhs.value_; }
    bool operator<=(const StringHash& rhs) const { return value_ <= rhs.value_; }
    bool operator>=(const StringHash& rhs) const { return value_ >= rhs.value_; }

    operator unsigned() const { return value_; }
    StringHash& operator=(unsigned value) { value_ = value; return *this; }

    /// http://create.stephan-brumme.com/fnv-hash/
    static inline uint32_t fnv1a(unsigned char oneByte, uint32_t hash = 0x811C9DC5)
    {
        return (oneByte ^ hash) * 0x01000193;
    }

    static inline uint32_t fnv1a(const void* data, size_t numBytes, uint32_t hash = 0x811C9DC5)
    {
        const unsigned char* ptr = (const unsigned char*)data;
        while (numBytes--)
            hash = fnv1a(*ptr++, hash);
        return hash;
    }

    /// hash a C-style string
    static uint32_t fnv1a(const char* text, uint32_t hash = 0x811C9DC5)
    {
        while (*text)
            hash = fnv1a((unsigned char)*text++, hash);
        return hash;
    }

    /// hash an std::string
    static uint32_t fnv1a(const std::string& text, uint32_t hash = 0x811C9DC5)
    {
        return fnv1a(text.c_str(), text.length(), hash);
    }

    /// hash a float
    static uint32_t fnv1a(float number, uint32_t hash = 0x811C9DC5)
    {
        return fnv1a(&number, sizeof(number), hash);
    }

    /// hash a double
    static uint32_t fnv1a(double number, uint32_t hash = 0x811C9DC5)
    {
        return fnv1a(&number, sizeof(number), hash);
    }

    /// hash a short (two bytes)
    static uint32_t fnv1a(unsigned short twoBytes, uint32_t hash = 0x811C9DC5)
    {
        const unsigned char* ptr = (const unsigned char*)&twoBytes;
        hash = fnv1a(*ptr++, hash);
        return fnv1a(*ptr, hash);
    }
    /// hash a 32 bit integer (four bytes)
    static uint32_t fnv1a(uint32_t fourBytes, uint32_t hash = 0x811C9DC5)
    {
        const unsigned char* ptr = (const unsigned char*)&fourBytes;
        hash = fnv1a(*ptr++, hash);
        hash = fnv1a(*ptr++, hash);
        hash = fnv1a(*ptr++, hash);
        return fnv1a(*ptr, hash);
    }
};

/// A pairing of a StringHash and the source text value
struct SPRUE Identifier : public std::pair<std::string, StringHash>
{
public:
    Identifier() { }
    Identifier(const std::string& text) { first = text; second = text; }
    Identifier(const Identifier& rhs) { first = rhs.first; second = rhs.second; }

    const StringHash& getHash() const { return second; }
    
    const std::string& getText() const { return first; }

    void setText(const std::string& text) { first = text; second = text; }

    Identifier& operator=(const Identifier& rhs) { first = rhs.first; second = rhs.second; return *this; }
    
    bool operator==(const Identifier& rhs) const { return second == rhs.second; }
    bool operator!=(const Identifier& rhs) const { return second != rhs.second; }
    bool operator<(const Identifier& rhs) const { return second< rhs.second; }
    bool operator>(const Identifier& rhs) const { return second> rhs.second; }
    bool operator<=(const Identifier& rhs) const { return second <= rhs.second; }
    bool operator>=(const Identifier& rhs) const { return second >= rhs.second; }

    bool operator==(const StringHash& rhs) const { return second == rhs; }
    bool operator!=(const StringHash& rhs) const { return second != rhs; }
    bool operator<(const StringHash& rhs) const { return second < rhs; }
    bool operator>(const StringHash& rhs) const { return second > rhs; }
    bool operator<=(const StringHash& rhs) const { return second <= rhs; }
    bool operator>=(const StringHash& rhs) const { return second >= rhs; }
};

}