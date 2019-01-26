#include "StringConversion.h"

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Libs/Jzon.h>
#include <SprueEngine/Libs/Base64.h>

#include <algorithm>
#include <cstdio>

// Trusting sprintf usage here
#pragma warning(push)
#pragma warning(disable: 4996)

namespace SprueEngine
{

#define STR_CONVERT_BUFFERSIZE 1024
    static char STR_CONVERT_BUFFER[STR_CONVERT_BUFFERSIZE];

#define CLEAR_STR_BUFFER memset(STR_CONVERT_BUFFER, 0, sizeof(char) * STR_CONVERT_BUFFERSIZE);

    std::string ToString(bool value)
    {
        CLEAR_STR_BUFFER;
        return value ? "true" : "false";
    }

    std::string ToString(int value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%d", value);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(unsigned value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%d", value);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(float value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g", value);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const IntVec2& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%d %d", value.x, value.y);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const Vec2& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g", value.x, value.y);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const Vec3& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g", value.x, value.y, value.z);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const Quat& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g %g", value.x, value.y, value.z, value.w);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const RGBA& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g %g", value.r, value.g, value.b, value.a);
        return STR_CONVERT_BUFFER;
    }

    std::string ToHexString(const RGBA& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%02x %02x %02x", (int)(value.r * 255.0f), (int)(value.g * 255.0f), (int)(value.b * 255.0f));
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const Mat3x4& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g %g %g %g %g %g %g %g %g %g",
            value.v[0][0],
            value.v[0][1],
            value.v[0][2],
            value.v[0][3],
            value.v[1][0],
            value.v[1][1],
            value.v[1][2],
            value.v[1][3],
            value.v[2][0],
            value.v[2][1],
            value.v[2][2],
            value.v[2][3]);
        return std::string(STR_CONVERT_BUFFER);
    }

    std::string ToString(const Mat3x3& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g %g %g %g %g %g %g",
            value.v[0][0],
            value.v[0][1],
            value.v[0][2],
            value.v[1][0],
            value.v[1][1],
            value.v[1][2],
            value.v[2][0],
            value.v[2][1],
            value.v[2][2]);
        return std::string(STR_CONVERT_BUFFER);
    }

    std::string ToString(const Disc& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g %g", value.normal.x, value.normal.y, value.normal.z, value.r);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const Plane& plane)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g %g", plane.normal.x, plane.normal.y, plane.normal.z, plane.d);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const BoundingBox& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g %g %g %g", value.minPoint.x, value.minPoint.y, value.minPoint.z, value.maxPoint.x, value.maxPoint.y, value.maxPoint.z);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const Ray& value)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%g %g %g %g %g %g", value.pos.x, value.pos.y, value.pos.z, value.dir.x, value.dir.y, value.dir.z);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const ResponseCurve& curve)
    {
        CLEAR_STR_BUFFER;
        sprintf(STR_CONVERT_BUFFER, "%u %g %g %g %g %u %u", (unsigned)curve.type_, curve.xIntercept_, curve.yIntercept_, curve.slopeIntercept_, curve.exponent_, curve.flipX_ ? 1 : 0, curve.flipY_ ? 1 : 0);
        return STR_CONVERT_BUFFER;
    }

    std::string ToString(const ColorCurve& curve)
    {
        Jzon::Node object = Jzon::object();
        object.add("count", curve.knots_.size());
        Jzon::Node knots = Jzon::array();
        for (unsigned i = 0; i < curve.knots_.size(); ++i)
            knots.add(ToString(curve.knots_[i]));
        object.add("knots", knots);

        std::string ret;
        Jzon::Writer writer(Jzon::CondensedFormat);
        writer.writeString(object, ret);
        return ret;
    }

    std::string ToString(const ColorCurves& curves)
    {
        Jzon::Node root = Jzon::array();

#define WRITE_CURVE(NAME) { Jzon::Node object = Jzon::object(); \
        object.add("count", NAME .knots_.size()); \
        Jzon::Node knots = Jzon::array(); \
        for (unsigned i = 0; i < NAME .knots_.size(); ++i) \
            knots.add(ToString(NAME .knots_[i])); \
        object.add("knots", knots); root.add(object); }

        WRITE_CURVE(curves.R);
        WRITE_CURVE(curves.G);
        WRITE_CURVE(curves.B);
        WRITE_CURVE(curves.A);

#undef WRITE_CURVE
        
        std::string ret;
        Jzon::Writer writer(Jzon::CondensedFormat);
        writer.writeString(root, ret);
        return ret;
    }

    std::string ToString(const RangedInt& value)
    {
        Jzon::Node data = Jzon::array();
        data.add(value.lowerBound_);
        data.add(value.upperBound_);
        data.add(value.inclusive_);
        
        Jzon::Writer writer(Jzon::CondensedFormat);
        std::string ret;
        writer.writeString(data, ret);
        return ret;
    }

    std::string ToString(const RangedFloat& value)
    {
        Jzon::Node data = Jzon::array();
        data.add(value.lowerBound_);
        data.add(value.upperBound_);
        data.add(value.inclusive_);

        Jzon::Writer writer(Jzon::CondensedFormat);
        std::string ret;
        writer.writeString(data, ret);
        return ret;
    }

    std::string ToString(const ColorRamp& value)
    {
        Jzon::Node root = Jzon::array();        
        for (unsigned i = 0; i < value.colors.size(); ++i)
        {
            Jzon::Node valNode = Jzon::object();
            valNode.add("time", value.colors[i].first);
            valNode.add("color", ToString(value.colors[i].second));
            root.add(valNode);
        }
        Jzon::Writer writer(Jzon::CondensedFormat);
        std::string ret;
        writer.writeString(root, ret);
        return ret;
    }

    std::string ToString(const VariantVector& value)
    {
        Jzon::Node container = Jzon::array();
        for (unsigned i = 0; i < value.size(); ++i)
        {
            Jzon::Node object = Jzon::object();
            object.add("type", Variant::VariantTypeToString(value[i].getType()));
            object.add("value", value[i].ConvertToString());
            container.add(object);
        }
        std::string ret;
        Jzon::Writer writer(Jzon::CondensedFormat);
        writer.writeString(container, ret);
        return ret;
    }

    std::string ToString(const VariantMap& value)
    {
        Jzon::Node root = Jzon::array();
        for (auto item : value)
        {
            Jzon::Node object = Jzon::object();
            object.add("hash", item.first.value_);
            object.add("type", Variant::VariantTypeToString(item.second.getType()));
            object.add("value", item.second.ConvertToString());
            root.add(object);
        }
        std::string ret;
        Jzon::Writer writer(Jzon::CondensedFormat);
        writer.writeString(root, ret);
        return ret;
    }

    std::string ToString(const StringHash& value)
    {
        return ToString(value.value_);
    }

    std::string ToString(const ResourceHandle& value)
    {
        Jzon::Node root = Jzon::array();
        root.add(Context::GetInstance()->GetHashName(value.Type));
        root.add(value.Name);
        std::string ret;
        Jzon::Writer writer(Jzon::CondensedFormat);
        writer.writeString(root, ret);
        return ret;
    }

    std::string ToString(const Variant& variant)
    {
        return variant.ConvertToString();
    }

    std::string ToString(const VectorBuffer& buffer)
    {
        std::string outData(Base64::EncodedLength(buffer.GetSize()), ' ');
        if (Base64::Encode((const char*)buffer.GetData(), buffer.GetSize(), (char*)outData.c_str(), outData.length()))
            return outData;
        return std::string();
    }

    bool FromString(const std::string& text, bool* value)
    {
        *value = text.compare("true") == 0;
        return true;
    }

    bool FromString(const std::string& text, int* value)
    {
        const char* ptr = text.c_str();
        *value = (unsigned)strtol(ptr, 0x0, 10);
        return true;
    }

    bool FromString(const std::string& text, unsigned* value)
    {
        const char* ptr = text.c_str();
        *value = (unsigned)strtoul(ptr, 0x0, 10);
        return true;
    }

    bool FromString(const std::string& text, float* value)
    {
        const char* ptr = text.c_str();
        *value = (float)strtod(ptr, 0x0);
        return true;
    }

    bool FromString(const std::string& text, IntVec2* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->x = (int)strtol(ptr, &ptr, 10);
        value->y = (int)strtol(ptr, &ptr, 10);
        return true;
    }

    bool FromString(const std::string& text, Vec2* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->x = (float)strtod(ptr, &ptr);
        value->y = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, Vec3* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->x = (float)strtod(ptr, &ptr);
        value->y = (float)strtod(ptr, &ptr);
        value->z = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, Quat* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->x = (float)strtod(ptr, &ptr);
        value->y = (float)strtod(ptr, &ptr);
        value->z = (float)strtod(ptr, &ptr);
        value->w = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, RGBA* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->r = (float)strtod(ptr, &ptr);
        value->g = (float)strtod(ptr, &ptr);
        value->b = (float)strtod(ptr, &ptr);
        value->a = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, Mat3x3* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->v[0][0] = (float)strtod(ptr, &ptr);
        value->v[0][1] = (float)strtod(ptr, &ptr);
        value->v[0][2] = (float)strtod(ptr, &ptr);

        value->v[1][0] = (float)strtod(ptr, &ptr);
        value->v[1][1] = (float)strtod(ptr, &ptr);
        value->v[1][2] = (float)strtod(ptr, &ptr);

        value->v[2][0] = (float)strtod(ptr, &ptr);
        value->v[2][1] = (float)strtod(ptr, &ptr);
        value->v[2][2] = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, Mat3x4* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->v[0][0] = (float)strtod(ptr, &ptr);
        value->v[0][1] = (float)strtod(ptr, &ptr);
        value->v[0][2] = (float)strtod(ptr, &ptr);
        value->v[0][3] = (float)strtod(ptr, &ptr);

        value->v[1][0] = (float)strtod(ptr, &ptr);
        value->v[1][1] = (float)strtod(ptr, &ptr);
        value->v[1][2] = (float)strtod(ptr, &ptr);
        value->v[1][3] = (float)strtod(ptr, &ptr);

        value->v[2][0] = (float)strtod(ptr, &ptr);
        value->v[2][1] = (float)strtod(ptr, &ptr);
        value->v[2][2] = (float)strtod(ptr, &ptr);
        value->v[2][3] = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, Disc* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->normal.x = (float)strtod(ptr, &ptr);
        value->normal.y = (float)strtod(ptr, &ptr);
        value->normal.z = (float)strtod(ptr, &ptr);
        value->r = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, Ray* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->pos.x = (float)strtod(ptr, &ptr);
        value->pos.y = (float)strtod(ptr, &ptr);
        value->pos.z = (float)strtod(ptr, &ptr);
        value->dir.x = (float)strtod(ptr, &ptr);
        value->dir.y = (float)strtod(ptr, &ptr);
        value->dir.z = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, Plane* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->normal.x = (float)strtod(ptr, &ptr);
        value->normal.y = (float)strtod(ptr, &ptr);
        value->normal.z = (float)strtod(ptr, &ptr);
        value->d = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, BoundingBox* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->minPoint.x = (float)strtod(ptr, &ptr);
        value->minPoint.y = (float)strtod(ptr, &ptr);
        value->minPoint.z = (float)strtod(ptr, &ptr);
        value->maxPoint.x = (float)strtod(ptr, &ptr);
        value->maxPoint.y = (float)strtod(ptr, &ptr);
        value->maxPoint.z = (float)strtod(ptr, &ptr);
        return true;
    }

    bool FromString(const std::string& text, ResponseCurve* value)
    {
        char* ptr = const_cast<char*>(text.c_str());
        value->type_ = (CurveType)strtoul(ptr, &ptr, 10);
        value->xIntercept_ = (float)strtod(ptr, &ptr);
        value->yIntercept_ = (float)strtod(ptr, &ptr);
        value->slopeIntercept_ = (float)strtod(ptr, &ptr);
        value->exponent_ = (float)strtod(ptr, &ptr);
        value->flipX_ = strtoul(ptr, &ptr, 10) == 1;
        value->flipY_ = strtoul(ptr, &ptr, 10) == 1;
        return true;
    }

    bool FromString(const std::string& text, ColorCurve* value)
    {
        Jzon::Parser parser;
        Jzon::Node object = parser.parseString(text); // object
        int ct = object.get("count").toInt();

        Jzon::Node knots = object.get("knots");
        if (knots.isNull())
            return false;

        for (unsigned i = 0; i < knots.getCount(); ++i)
        {
            Vec2 pt;
            if (FromString(knots.get(i).toString(), &pt))
                value->knots_.push_back(pt);
        }

        return true;
    }

    bool FromString(const std::string& text, ColorCurves* value)
    {
        Jzon::Parser parser;
        Jzon::Node root = parser.parseString(text);

        ColorCurve* channels[4] = { &value->R, &value->G, &value->B, &value->A };

        if (root.isArray() && root.getCount() == 4)
        {
            for (int chan = 0; chan < 4; ++chan)
            {
                Jzon::Node channel = root.get(chan);
                int ct = channel.get("count").toInt();
                channels[chan]->knots_.resize(ct);

                Jzon::Node values = channel.get("knots");
                if (values.isArray() && values.getCount())
                {
                    for (int i = 0; i < values.getCount(); ++i)
                    {
                        Vec2 val;
                        FromString(values.get(i).toString(), &val);
                        channels[chan]->knots_.push_back(val);
                    }
                }
            }
            return true;
        }
        return false;
    }

    bool FromString(const std::string& text, ColorRamp* value)
    {
        Jzon::Parser parser;
        Jzon::Node root = parser.parseString(text);

        if (root.isArray() && root.getCount())
        {
            for (unsigned i = 0; i < root.getCount(); ++i)
            {
                Jzon::Node valNode = root.get(i);
                float time = valNode.get("time").toFloat();
                RGBA color;
                FromString(valNode.get("color").toString(), &color);
                value->colors.push_back({ time, color });
            }
            return true;
        }
        return false;
    }

    bool FromString(const std::string& text, VariantVector* value)
    {
        Jzon::Parser parser;
        Jzon::Node container = parser.parseString(text); //array
        if (container.isArray())
        {
            for (unsigned i = 0; i < container.getCount(); ++i)
            {
                Jzon::Node object = container.get(i);
                VariantType type = Variant::VariantTypeFromString(object.get("type").toString().c_str());
                
                Variant newValue;
                newValue.FromString(type, object.get("value").toString());
                value->push_back(newValue);
            }
            return true;
        }
        return false;
    }

    bool FromString(const std::string& text, VariantMap* value)
    {
        Jzon::Parser parser;
        Jzon::Node root = parser.parseString(text); //array
        if (root.isArray())
        {
            for (int i = 0; i < root.getCount(); ++i)
            {
                Jzon::Node object = root.get(i);
                StringHash hash((unsigned)object.get("hash").toInt());
                VariantType type = Variant::VariantTypeFromString(object.get("type").toString().c_str());

                Variant newValue;
                newValue.FromString(type, object.get("value").toString());
                value->insert({ hash, newValue });
            }
            return true;
        }
        return false;
    }

    bool FromString(const std::string& text, StringHash* value)
    {
        return FromString(text, &value->value_);
    }

    bool FromString(const std::string& text, ResourceHandle* value)
    {
        Jzon::Parser parser;
        Jzon::Node root = parser.parseString(text);

        if (root.isArray() && root.getCount() == 2)
        {
            StringHash hash(root.get(0).toString());
            value->Type = hash;
            value->Name = root.get(1).toString();
            return true;
        }
        return false;
    }

    bool FromString(const std::string& text, RangedInt* value)
    {
        Jzon::Parser parser;
        Jzon::Node data = parser.parseString(text); // array

        if (data.isArray() && data.getCount() == 3)
        {
            value->lowerBound_ = data.get(0).toInt();
            value->upperBound_ = data.get(1).toInt();
            value->inclusive_ = data.get(2).toBool();
            return true;
        }
        return false;
    }

    bool FromString(const std::string& text, RangedFloat* value)
    {
        Jzon::Parser parser;
        Jzon::Node data = parser.parseString(text); // array

        if (data.isArray() && data.getCount() == 3)
        {
            value->lowerBound_ = data.get(0).toFloat();
            value->upperBound_ = data.get(1).toFloat();
            value->inclusive_ = data.get(2).toBool();
            return true;
        }
        return false;
    }

    bool FromString(const std::string& text, VectorBuffer* buffer)
    {
        if (text.length() > 0)
        {
            buffer->Resize(Base64::DecodedLength(text));
            return Base64::Decode(text.c_str(), text.length(), (char*)buffer->GetData(), buffer->GetSize());
        }
        return false;
    }

#pragma warning(pop)

}