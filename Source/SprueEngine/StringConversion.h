#pragma once

#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/Variant.h>
#include <SprueEngine/VectorBuffer.h>

namespace SprueEngine
{

    struct ResponseCurve;
    struct RGBA;
    struct RangedInt;
    struct RangedFloat;

#ifndef CppSharp
    std::string ToString(bool);
    std::string ToString(int);
    std::string ToString(unsigned);
    std::string ToString(float);
    std::string ToString(const IntVec2&);
    std::string ToString(const Vec2&);
    std::string ToString(const Vec3&);
    std::string ToString(const Quat&);
    std::string ToString(const RGBA&);
    std::string ToHexString(const RGBA&);
    std::string ToString(const Mat3x3&);
    std::string ToString(const Mat3x4&);
    std::string ToString(const Disc&);
    std::string ToString(const Plane&);
    std::string ToString(const BoundingBox&);
    std::string ToString(const Ray&);
    std::string ToString(const ResponseCurve&);
    std::string ToString(const Variant& variant);
    std::string ToString(const RangedInt&);
    std::string ToString(const RangedFloat&);
    std::string ToString(const ColorCurve&);
    std::string ToString(const ColorCurves&);
    std::string ToString(const ColorRamp&);
    std::string ToString(const VariantVector&);
    std::string ToString(const VariantMap&);
    std::string ToString(const StringHash&);
    std::string ToString(const ResourceHandle&);
    std::string ToString(const VectorBuffer&);

    template<typename T>
    std::string ToString(const std::vector<T>& list)
    {
        std::stringstream ss;
        for (T& value : list)
            ss << value << ';';
        return ss.str();
    }

    template<typename K, typename V>
    std::string ToString(const std::map<K, V>& table)
    {
        std::stringstream ss;
        for (std::map<K, V>::const_iterator cit = table.begin(); cit != table.end(); ++cit)
            ss << cit->first << ':' << cit->second << ';';
        return ss.str();
    }

    bool FromString(const std::string& text, bool* value);
    bool FromString(const std::string& text, int* value);
    bool FromString(const std::string& text, unsigned* value);
    bool FromString(const std::string& text, float* value);
    bool FromString(const std::string& text, IntVec2* value);
    bool FromString(const std::string& text, Vec2* value);
    bool FromString(const std::string& text, Vec3* value);
    bool FromString(const std::string& text, Quat*);
    bool FromString(const std::string& text, RGBA*);
    bool FromString(const std::string& text, Mat3x3*);
    bool FromString(const std::string& text, Mat3x4*);
    bool FromString(const std::string& text, Disc*);
    bool FromString(const std::string& text, Ray*);
    bool FromString(const std::string& text, Plane*);
    bool FromString(const std::string& text, BoundingBox*);
    bool FromString(const std::string& text, ResponseCurve*);
    bool FromString(const std::string& text, RangedInt*);
    bool FromString(const std::string& text, RangedFloat*);
    bool FromString(const std::string& text, ColorCurve*);
    bool FromString(const std::string& text, ColorCurves*);

    bool FromString(const std::string& text, ColorRamp*);
    bool FromString(const std::string& text, VariantVector*);
    bool FromString(const std::string& text, VariantMap*);
    bool FromString(const std::string& text, StringHash*);
    bool FromString(const std::string& text, ResourceHandle*);
    bool FromString(const std::string& text, VectorBuffer*);

    template<typename T>
    bool FromString(const std::string& text, std::vector<T>* list)
    {
        std::istringstream ss(text);
        std::string item;
        while (std::getline(ss, item, ';'))
        {
            if (!item.empty())
            {
                T temp;
                FromString(item, &temp);
                list->push_back(temp);
            }
        }
        return true;
    }

    template<typename K, typename V>
    bool FromString(const std::string& text, std::map<K, V>* list)
    {
        std::istringstream ss(text);
        std::string key;
        while (std::getline(ss, key, ':'))
        {
            if (!key.empty())
            {
                std::string value;
                std::getline(ss, value, ';');
                K tempKey;
                FromString(key, &tempKey);
                V tempValue;
                FromString(value, &tempValue);
                list->insert(std::make_pair<K, V>(tempKey, tempValue));
            }
        }
        return true;
    }
#endif

}