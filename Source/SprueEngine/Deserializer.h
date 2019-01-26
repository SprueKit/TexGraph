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
#include <SprueEngine/StringHash.h>
#include <SprueEngine/Variant.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <string>
#include <vector>

namespace SprueEngine
{
    /// Abstract stream for reading.
    class SPRUE Deserializer
    {
    protected:
        /// Construct with zero size.
        Deserializer();
        /// Construct with defined size.
        Deserializer(unsigned size);    
    public:
        /// Destruct.
        virtual ~Deserializer();
        /// Read bytes from the stream. Return number of bytes actually read.
        virtual unsigned Read(void* dest, unsigned size) = 0;
        /// Set position from the beginning of the stream.
        virtual unsigned Seek(unsigned position) = 0;
        /// Return name of the stream.
        virtual const std::string& GetName() const;
        /// Return a checksum if applicable.
        virtual unsigned GetChecksum();

        /// Return current position.
        unsigned GetPosition() const { return position_; }

        /// Return size.
        unsigned GetSize() const { return size_; }

        /// Return whether the end of stream has been reached.
        bool IsEof() const { return position_ >= size_; }

#define DECL_READ(TYPENAME, FUNCNAME) TYPENAME Read ## FUNCNAME () { TYPENAME ret; Read(&ret, sizeof(TYPENAME)); return ret; }

        DECL_READ(int, Int);
        DECL_READ(short, Short);
        DECL_READ(unsigned, UInt);
        DECL_READ(unsigned short, UShort);
        DECL_READ(unsigned char, UByte);
        DECL_READ(char, Byte);
        DECL_READ(bool, Bool);
        DECL_READ(float, Float);
        DECL_READ(double, Double);

#undef DECL_READ
        /// Read a Vector2.
        Vec2 ReadVector2();
        /// Read a Vector3.
        Vec3 ReadVector3();
        /// Read a Vector3 packed into 3 x 16 bits with the specified maximum absolute range.
        Vec3 ReadPackedVector3(float maxAbsCoord);
        /// Read a Vec4.
        Vec4 ReadVector4();
        /// Read a quaternion.
        Quat ReadQuaternion();
        /// Read a quaternion with each component packed in 16 bits.
        Quat ReadPackedQuaternion();
        /// Read a Matrix3.
        Mat3x3 ReadMatrix3();
        /// Read a Matrix3x4.
        Mat3x4 ReadMatrix3x4();
        /// Read a color.
        RGBA ReadColor();
        /// Read a bounding box.
        BoundingBox ReadBoundingBox();
        /// Read a null-terminated string.
        std::string ReadString();
        /// Read a four-letter file ID.
        std::string ReadFileID();
        /// Read a buffer with size encoded as VLE.
        std::vector<unsigned char> ReadBuffer();
        /// Read a text line.
        std::string ReadLine();
        unsigned ReadVLE();

        Variant ReadVariant();
        VariantMap ReadVariantMap();
        VariantVector ReadVariantVector();
        StringHash ReadStringHash();

        template<class T>
        FilterableBlockMap<T>* ReadBlockMap()
        {
            if (ReadBool())
            {
                unsigned width = ReadUInt();
                unsigned height = ReadUInt();
                unsigned depth = ReadUInt();
                
                FilterableBlockMap<T>* ret = new FilterableBlockMap<T>();
                ret->resize(width, height, depth);

                unsigned dataCt = width * height * depth;
                Read(map->getData(), dataCt * sizeof(T));
                return ret;
            }
            return 0x0;
        }

        template<class T>
        std::vector<T*> ReadEditables()
        {
            static_assert(std::is_base_of<IEditable, T>::value, "T must be derived from IEditable to use Deserializer::ReadEditables");

            std::vector<T*> ret;
            unsigned ct = ReadUInt();
            while (ct)
            {
                if (T* obj = Context::GetInstance()->Deserialize<T>(this))
                    ret.pus_back(obj);
                --ct;
            }
            return ret;
        }

    protected:
        /// Stream position.
        mutable unsigned position_ = 0;
        /// Stream size.
        unsigned size_ = 0;
    };

}
