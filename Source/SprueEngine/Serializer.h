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

#pragma once

#include <SprueEngine/BlockMap.h>
#include <SprueEngine/Math/Color.h>
#include <SprueEngine/Variant.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <string>
#include <vector>

namespace SprueEngine
{
    struct StringHash;

    /// Abstract stream for writing.
    /// TODO: Consider merging with Deserializer as an Read/Write stream (mode set at initialization)
    ///     This would be similar to Unreal Engine's simple "Serialize" methods, has problems though that first need to be solved
    ///     vector<T>, multimap<T,T>, map<T,T>, BlockMap<T>
    class SPRUE Serializer
    {
    public:
        /// Destruct.
        virtual ~Serializer();

        /// Write bytes to the stream. Return number of bytes actually written.
        virtual unsigned Write(const void* data, unsigned size) = 0;

#define DECL_WRITE(TYPENAME, FUNCNAME) bool Write ## FUNCNAME (TYPENAME value) { return Write(&value, sizeof(TYPENAME)) == sizeof(TYPENAME); }

        DECL_WRITE(int, Int);
        DECL_WRITE(short, Short);
        DECL_WRITE(unsigned, UInt);
        DECL_WRITE(unsigned short, UShort);
        DECL_WRITE(unsigned char, UByte);
        DECL_WRITE(char, Byte);
        DECL_WRITE(bool, Bool);
        DECL_WRITE(float, Float);
        DECL_WRITE(double, Double);

#undef DECL_WRITE

        /// Write a Vector2.
        bool WriteVector2(const Vec2& value);
        /// Write a Vector3.
        bool WriteVector3(const Vec3& value);
        /// Write a Vector3 packed into 3 x 16 bits with the specified maximum absolute range.
        bool WritePackedVector3(const Vec3& value, float maxAbsCoord);
        /// Write a Vector3.
        bool WriteVector4(const Vec4& value);
        /// Write a quaternion.
        bool WriteQuaternion(const Quat& value);
        /// Write a quaternion with each component packed in 16 bits.
        bool WritePackedQuaternion(const Quat& value);
        /// Write a Matrix3.
        bool WriteMatrix3(const Mat3x3& value);
        /// Write a Matrix3x4.
        bool WriteMatrix3x4(const Mat3x4& value);
        /// Write a color.
        bool WriteColor(const RGBA& value);
        /// Write a bounding box.
        bool WriteBoundingBox(const BoundingBox& value);
        /// Write a null-terminated string.
        bool WriteString(const std::string& value);
        /// Write a four-letter file ID. If the string is not long enough, spaces will be appended.
        bool WriteFileID(const std::string& value);
        /// Write a buffer, with size encoded as VLE.
        bool WriteBuffer(const std::vector<unsigned char>& buffer);
        /// Write a text line. Char codes 13 & 10 will be automatically appended.
        bool WriteLine(const std::string& value);

        bool WriteVLE(unsigned value);

        bool WriteVariant(const Variant& var);

        bool WriteStringHash(const StringHash& value);

        bool WriteVariantVector(const VariantVector& value);

        bool WriteVariantMap(const VariantMap& value);

        template<class T>
        bool WriteBlockMap(const FilterableBlockMap<T>* map)
        {
            if (map)
            {
                bool success = WriteBool(true);
                success &= WriteUInt(map->getWidth());
                success &= WriteUInt(map->getHeight());
                success &= WriteUInt(map->getDepth());
                unsigned dataCt = map->getWidth() * map->getHeight() * map->getDepth();
                success &= Write(map->getData(), dataCt * sizeof(T));
            }
            else
            {
                return WriteBool(false);
            }
        }

        template<class T>
        void WriteEditables(const std::vector<T*>& editables)
        {
            static_assert(std::is_base_of<IEditable, T>::value, "T must be derived from IEditable to use Serializer::WriteEditables");

            WriteUInt(editables.size());
            for (auto editable : editables)
                editable->Serialize(this);
        }

    private:
        bool WriteVariantData(const Variant& value);
    };

}
