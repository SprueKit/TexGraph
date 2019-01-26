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

#include "SprueEngine/Serializer.h"

#include "SprueEngine/Math/MathDef.h"
#include "SprueEngine/StringHash.h"
#include "SprueEngine/Variant.h"

namespace SprueEngine
{

    static const float q = 32767.0f;

    Serializer::~Serializer()
    {
    }

    bool Serializer::WriteVector2(const Vec2& value)
    {
        return Write(&value.x, sizeof value) == sizeof value;
    }

    bool Serializer::WriteVector3(const Vec3& value)
    {
        return Write(&value.x, sizeof value) == sizeof value;
    }

    bool Serializer::WritePackedVector3(const Vec3& value, float maxAbsCoord)
    {
        short coords[3];
        float v = 32767.0f / maxAbsCoord;

        coords[0] = (short)(CLAMP(value.x, -maxAbsCoord, maxAbsCoord) * v + 0.5f);
        coords[1] = (short)(CLAMP(value.y, -maxAbsCoord, maxAbsCoord) * v + 0.5f);
        coords[2] = (short)(CLAMP(value.z, -maxAbsCoord, maxAbsCoord) * v + 0.5f);
        return Write(&coords[0], sizeof coords) == sizeof coords;
    }

    bool Serializer::WriteVector4(const Vec4& value)
    {
        return Write(&value.x, sizeof value) == sizeof value;
    }

    bool Serializer::WriteQuaternion(const Quat& value)
    {
        return Write(&value.x, sizeof value) == sizeof value;
    }

    bool Serializer::WritePackedQuaternion(const Quat& value)
    {
        short coords[4];
        Quat norm = value;
        norm.Normalize();

        coords[0] = (short)(CLAMP(norm.w, -1.0f, 1.0f) * q + 0.5f);
        coords[1] = (short)(CLAMP(norm.x, -1.0f, 1.0f) * q + 0.5f);
        coords[2] = (short)(CLAMP(norm.y, -1.0f, 1.0f) * q + 0.5f);
        coords[3] = (short)(CLAMP(norm.z, -1.0f, 1.0f) * q + 0.5f);
        return Write(&coords[0], sizeof coords) == sizeof coords;
    }

    bool Serializer::WriteMatrix3(const Mat3x3& value)
    {
        return Write(value.v, sizeof value) == sizeof value;
    }

    bool Serializer::WriteMatrix3x4(const Mat3x4& value)
    {
        return Write(value.v, sizeof value) == sizeof value;
    }

    bool Serializer::WriteColor(const RGBA& value)
    {
        return Write(&value.r, sizeof value) == sizeof value;
    }

    bool Serializer::WriteBoundingBox(const BoundingBox& value)
    {
        bool success = true;
        success &= WriteVector3(value.minPoint.xyz());
        success &= WriteVector3(value.maxPoint.xyz());
        return success;
    }

    bool Serializer::WriteString(const std::string& value)
    {
        const char* chars = value.c_str();
        unsigned length = (unsigned)value.length();
        bool ret = Write(chars, length + 1) == length + 1;
        return ret;
    }

    bool Serializer::WriteFileID(const std::string& value)
    {
        bool success = true;
        unsigned length = (unsigned)SprueMin((int)value.length(), 4);

        success &= Write(value.c_str(), length) == length;
        for (unsigned i = (unsigned)value.length(); i < 4; ++i)
            success &= WriteByte(' ');
        return success;
    }

    bool Serializer::WriteBuffer(const std::vector<unsigned char>& value)
    {
        bool success = true;
        unsigned size = (unsigned)value.size();

        success &= WriteVLE(size);
        if (size)
            success &= Write(&value[0], size) == size;
        return success;
    }

    bool Serializer::WriteVLE(unsigned value)
    {
        unsigned char data[4];

        if (value < 0x80)
            return WriteUByte((unsigned char)value);
        else if (value < 0x4000)
        {
            data[0] = (unsigned char)(value | 0x80);
            data[1] = (unsigned char)(value >> 7);
            return Write(data, 2) == 2;
        }
        else if (value < 0x200000)
        {
            data[0] = (unsigned char)(value | 0x80);
            data[1] = (unsigned char)((value >> 7) | 0x80);
            data[2] = (unsigned char)(value >> 14);
            return Write(data, 3) == 3;
        }
        else
        {
            data[0] = (unsigned char)(value | 0x80);
            data[1] = (unsigned char)((value >> 7) | 0x80);
            data[2] = (unsigned char)((value >> 14) | 0x80);
            data[3] = (unsigned char)(value >> 21);
            return Write(data, 4) == 4;
        }
    }

    bool Serializer::WriteLine(const std::string& value)
    {
        bool success = true;
        success &= Write(value.c_str(), (unsigned)value.length()) == value.length();
        success &= WriteUByte(13);
        success &= WriteUByte(10);
        return success;
    }

    bool Serializer::WriteStringHash(const StringHash& value)
    {
        return Write(&value.value_, sizeof(uint32_t)) == sizeof(uint32_t);
    }

    bool Serializer::WriteVariantVector(const VariantVector& value)
    {
        bool success = true;
        success &= WriteVLE(value.size());
        for (const auto& value : value)
            success &= WriteVariant(value);
        return success;
    }

    bool Serializer::WriteVariantMap(const VariantMap& value)
    {
        bool success = true;
        success &= WriteVLE((unsigned)value.size());
        for (VariantMap::const_iterator i = value.begin(); i != value.end(); ++i)
        {
            WriteStringHash(i->first);
            WriteVariant(i->second);
        }
        return success;
    }

    bool Serializer::WriteVariant(const Variant& var)
    {
        bool success = true;

        success &= WriteUByte(var.getType());
        success &= WriteVariantData(var);
        return success;
    }

    bool Serializer::WriteVariantData(const Variant& value)
    {
#define VAR_HANDLER(TYPE, VARTYPE) case VARTYPE: return Write ## TYPE (value.get ## TYPE ())
#define BYTE_HANDLER(TYPE, VARTYPE) case VARTYPE: { const TYPE& val = value.get ## TYPE (); return Write(&value, sizeof(TYPE)); }
        switch (value.getType())
        {
        case VT_None:
            return true;
        VAR_HANDLER(Int, VT_Int);
        VAR_HANDLER(Float, VT_Float);
        VAR_HANDLER(UInt, VT_UInt);
        VAR_HANDLER(Bool, VT_Bool);
        case VT_Vec2:
            return WriteVector2(value.getVec2());
        case VT_Vec3:
            return WriteVector3(value.getVec3());
        case VT_Vec4:
            return WriteVector4(value.getVec4());
        case VT_Quat:
            return WriteQuaternion(value.getQuat());
        case VT_Mat3:
            return WriteMatrix3(value.getMat3x3());
        case VT_Mat3x4:
            return WriteMatrix3x4(value.getMat3x4());
        case VT_Color:
            return WriteColor(value.getRGBA());
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
                ResourceHandle handle = value.getResourceHandle();
                WriteStringHash(handle.Type);
                WriteString(handle.Name);
                return true;
            } break;
        case VT_ColorCurves:
            value.getColorCurves().Serialize(this);
            return true;
        case VT_ColorRamp:
            value.getColorRamp().Serialize(this);
            return true;
        case VT_RangedInt:
            {
                RangedInt range = value.getRangedInt();
                WriteInt(range.getLowerBound());
                WriteInt(range.getUpperBound());
                WriteBool(range.getInclusive());
            }
            return true;
        case VT_RangedFloat:
            {
                RangedFloat range = value.getRangedFloat();
                WriteFloat(range.getLowerBound());
                WriteFloat(range.getUpperBound());
                WriteBool(range.getInclusive());
            }
            return true;
        default:
            SPRUE_ASSERT(false, "Unhandled variant serialize");
            break;
        }
        return false;
#undef VAR_HANDLER
#undef BYTE_HANDLER
    }
}
