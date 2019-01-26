//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "SprueEngine/Deserializer.h"

#include "SprueEngine/Variant.h"

namespace SprueEngine
{

    static const float invQ = 1.0f / 32767.0f;

    Deserializer::Deserializer() :
        position_(0),
        size_(0)
    {
        
    }

    Deserializer::Deserializer(unsigned size) :
        position_(0),
        size_(size)
    {
    }

    Deserializer::~Deserializer()
    {
    }

    const std::string& Deserializer::GetName() const
    {
        return "";
    }

    unsigned Deserializer::GetChecksum()
    {
        return 0;
    }

    Vec2 Deserializer::ReadVector2()
    {
        float data[2];
        Read(data, sizeof data);
        return Vec2(data);
    }

    Vec3 Deserializer::ReadVector3()
    {
        float data[3];
        Read(data, sizeof data);
        return Vec3(data);
    }

    Vec3 Deserializer::ReadPackedVector3(float maxAbsCoord)
    {
        float invV = maxAbsCoord / 32767.0f;
        short coords[3];
        Read(coords, sizeof coords);
        Vec3 ret(coords[0] * invV, coords[1] * invV, coords[2] * invV);
        return ret;
    }

    Vec4 Deserializer::ReadVector4()
    {
        float data[4];
        Read(data, sizeof data);
        return Vec4(data);
    }

    Quat Deserializer::ReadQuaternion()
    {
        float data[4];
        Read(data, sizeof data);
        return Quat(&data[0]);
    }

    Quat Deserializer::ReadPackedQuaternion()
    {
        short coords[4];
        Read(coords, sizeof coords);
        Quat ret(coords[0] * invQ, coords[1] * invQ, coords[2] * invQ, coords[3] * invQ);
        ret.Normalize();
        return ret;
    }

    Mat3x3 Deserializer::ReadMatrix3()
    {
        float data[9];
        Read(data, sizeof data);
        return Mat3x3(data);
    }

    Mat3x4 Deserializer::ReadMatrix3x4()
    {
        float data[12];
        Read(data, sizeof data);
        return Mat3x4(data);
    }

    RGBA Deserializer::ReadColor()
    {
        float data[4];
        Read(data, sizeof data);
        return RGBA(data[0], data[1], data[2], data[3]);
    }

    BoundingBox Deserializer::ReadBoundingBox()
    {
        float data[6];
        Read(data, sizeof data);
        return BoundingBox(Vec3(&data[0]), Vec3(&data[3]));
    }

    std::string Deserializer::ReadString()
    {
        std::string ret;

        while (!IsEof())
        {
            char c = ReadByte();
            if (!c)
                break;
            else
                ret += c;
        }

        return ret;
    }

    std::string Deserializer::ReadFileID()
    {
        std::string ret;
        ret.resize(4);
        Read(&ret[0], 4);
        return ret;
    }

    std::vector<unsigned char> Deserializer::ReadBuffer()
    {
        std::vector<unsigned char> ret(ReadVLE());
        if (ret.size())
            Read(&ret[0], (unsigned)ret.size());
        return ret;
    }

    unsigned Deserializer::ReadVLE()
    {
        unsigned ret;
        unsigned char byte;

        byte = ReadUByte();
        ret = (unsigned)(byte & 0x7f);
        if (byte < 0x80)
            return ret;

        byte = ReadUByte();
        ret |= ((unsigned)(byte & 0x7f)) << 7;
        if (byte < 0x80)
            return ret;

        byte = ReadUByte();
        ret |= ((unsigned)(byte & 0x7f)) << 14;
        if (byte < 0x80)
            return ret;

        byte = ReadUByte();
        ret |= ((unsigned)byte) << 21;
        return ret;
    }

    std::string Deserializer::ReadLine()
    {
        std::string ret;

        while (!IsEof())
        {
            char c = ReadByte();
            if (c == 10)
                break;
            if (c == 13)
            {
                // Peek next char to see if it's 10, and skip it too
                if (!IsEof())
                {
                    char next = ReadByte();
                    if (next != 10)
                        Seek(position_ - 1);
                }
                break;
            }

            ret += c;
        }

        return ret;
    }

    Variant Deserializer::ReadVariant()
    {
        VariantType type = (VariantType)ReadUByte();

#define VAR_HANDLER(TYPE, VARTYPE) case VARTYPE : return Variant(Read ## TYPE ())
#define BYTE_HANDLER(TYPE, VARTYPE) case VARTYPE : { TYPE ret; Read(&ret, sizeof(TYPE)); return ret; }
        switch (type)
        {
        VAR_HANDLER(Int, VT_Int);
        VAR_HANDLER(Float, VT_Float);
        VAR_HANDLER(UInt, VT_UInt);
        VAR_HANDLER(Bool, VT_Bool);
        case VT_Vec2:
            return ReadVector2();
        case VT_Vec3:
            return ReadVector3();
        case VT_Vec4:
            return ReadVector4();
        case VT_Quat:
            return ReadQuaternion();
        case VT_Mat3:
            return ReadMatrix3();
        case VT_Mat3x4:
            return ReadMatrix3x4();
        case VT_Color:
            return ReadColor();
        BYTE_HANDLER(IntVec2, VT_IntVec2);
        BYTE_HANDLER(Plane, VT_Plane);
        BYTE_HANDLER(Disc, VT_Disc);
        BYTE_HANDLER(BoundingBox, VT_BoundingBox);
        BYTE_HANDLER(Ray, VT_Ray);
        BYTE_HANDLER(ResponseCurve, VT_ResponseCurve);
        VAR_HANDLER(VariantVector, VT_VariantVector);
        VAR_HANDLER(VariantMap, VT_VariantMap);
        VAR_HANDLER(String, VT_String);
        VAR_HANDLER(StringHash, VT_StringHash);
        case VT_ResourceHandle: {
                ResourceHandle handle;
                handle.Type = ReadStringHash();
                handle.Name = ReadString();
                return handle;
            } break;
        case VT_ColorCurves: {
                ColorCurves curves;
                curves.Deserialize(this);
                return curves;
            } break;
        case VT_ColorRamp: {
                ColorRamp ramp;
                ramp.Deserialize(this);
                return ramp;
            } break;
        case VT_RangedInt: {
                RangedInt range;
                range.setLowerBound(ReadInt());
                range.setUpperBound(ReadInt());
                range.setInclusive(ReadBool());
                return range;
            } break;
        case VT_RangedFloat: {
                RangedFloat range;
                range.setLowerBound(ReadFloat());
                range.setUpperBound(ReadFloat());
                range.setInclusive(ReadBool());
                return range;
            } break;
        }
#undef VAR_HANDLER
#undef BYTE_HANDLER
        SPRUE_ASSERT(false, "Unhandled Variant deserialize");
        return Variant();
    }

    VariantMap Deserializer::ReadVariantMap()
    {
        VariantMap ret;
        unsigned num = ReadVLE();

        for (unsigned i = 0; i < num; ++i)
        {
            StringHash key = ReadStringHash();
            ret[key] = ReadVariant();
        }

        return ret;
    }

    VariantVector Deserializer::ReadVariantVector()
    {
        VariantVector ret(ReadVLE());
        for (unsigned i = 0; i < ret.size(); ++i)
            ret[i] = ReadVariant();
        return ret;
    }

    StringHash Deserializer::ReadStringHash()
    {
        return ReadUInt();
    }
}
