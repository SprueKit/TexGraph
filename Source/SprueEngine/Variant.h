#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Math/Color.h>
#include <SprueEngine/Math/IntVec.h>
#include <SprueEngine/Math/RangedValue.h>
#include <SprueEngine/Resource.h>
#include <SprueEngine/ResponseCurve.h>
#include <SprueEngine/StringHash.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable: 4172)

/// WARNING!
/// This file is macro-intensive. Without the macros this whole thing would be a tedious typo-prone shit-heap of repetitive code.
/// Even with the macros it's a repetitive shit-heap.
///
/// If you add a new block of macros, please save the kittens and #undef your macro once you've finished using it :)

namespace SprueEngine
{
    class Serializer;
    class Deserializer;
    class VectorBuffer;

    enum VariantType
    {
        VT_None = 0,
        VT_Byte,
        VT_Bool,
        VT_Int,
        VT_UInt,
        VT_Float,
        VT_RangedInt,
        VT_RangedFloat,
        VT_IntVec2,
        VT_Vec2,
        VT_Vec3,
        VT_Vec4,
        VT_Mat3,
        VT_Quat,
        VT_Mat3x4,
        VT_Plane,
        VT_BoundingBox,
        VT_Color,
        VT_Ray,
        VT_Disc,
        VT_String,
        VT_VariantVector,
        VT_VariantMap,
        VT_StringHash,
        VT_ResponseCurve,
        VT_ResourceHandle,
        VT_VoidPtr,
        VT_ColorCurves,
        VT_ColorRamp,
        VT_VectorBuffer
    };

class Variant;

typedef std::vector<Variant> VariantVector;
typedef std::map<StringHash, Variant> VariantMap;

/// Massively influenced by Urho3D's Variant class for the backing storage
class SPRUE Variant
{
public:
    typedef std::string String;
    struct { // sizeof(int_ptr) * 4 basically (16 bytes on 32 bit, 64 bytes on 64 bit)
        union {
            int int_;
            unsigned uint_;
            float float_;
            bool bool_;
            void* ptr_;
        };

        union {
            int int2_;
            unsigned uint2_;
            float float2_;
        };

        union {
            int int3_;
            unsigned uint3_;
            float float3_;
        };
        union {
            int int4_;
            unsigned uint4_;
            float float4_;
        };
    } data_;
    VariantType type_ = VT_None;

public:
    Variant() { data_.ptr_ = 0x0; type_ = VT_None; }
    Variant(const Variant& rhs) { type_ = VT_None; data_.ptr_ = 0x0; *this = rhs; }
    ~Variant() { setType(VT_None); }
    void operator=(const Variant& rhs);

    VariantType getType() const { return type_; }

    //Variant& operator=(bool value) { setType(VT_Bool); data_.bool_ = value; return *this; } Variant(bool value) { *this = value; }
#define VAR_ASSIGN(TYPE, VARTYPE, DATA) Variant& operator=(TYPE value) { setType(VARTYPE); data_. DATA = value; return *this; } Variant(TYPE value) : Variant() { *this = value; }
    Variant& operator=(bool value) { setType(VT_Bool); data_.bool_ = value; return *this; } Variant(bool value) { *this = value; }
    //VAR_ASSIGN(bool, VT_Bool, bool_);
    VAR_ASSIGN(unsigned char, VT_Byte, uint_);
    VAR_ASSIGN(int, VT_Int, int_);
    VAR_ASSIGN(unsigned, VT_UInt, uint_);
    VAR_ASSIGN(float, VT_Float, float_);
    VAR_ASSIGN(StringHash, VT_StringHash, uint_);
    VAR_ASSIGN(void*, VT_VoidPtr, ptr_);
#undef VAR_ASSIGN
    Variant(const char* msg) { *this = std::string(msg); }

#define MEM_ASSIGN(TYPE, VARTYPE) Variant& operator=(const TYPE& value) { setType(VARTYPE); memcpy(&data_, &value, sizeof(TYPE)); return *this; } Variant(TYPE value) : Variant() { *this = value; }
    MEM_ASSIGN(RangedInt, VT_RangedInt);
    MEM_ASSIGN(RangedFloat, VT_RangedFloat);
    MEM_ASSIGN(IntVec2, VT_IntVec2);
    MEM_ASSIGN(Vec2, VT_Vec2);
    MEM_ASSIGN(Vec3, VT_Vec3);
    MEM_ASSIGN(Vec4, VT_Vec4);
    MEM_ASSIGN(Quat, VT_Quat);
    MEM_ASSIGN(RGBA, VT_Color);
#undef MEM_ASSIGN

#define REINTERPRET_ASSIGN(TYPE, VARTYPE) Variant& operator=(const TYPE& value) { setType(VARTYPE); *((TYPE *)(data_.ptr_)) = value; return *this; } Variant(TYPE value) : Variant() { *this = value; }
    REINTERPRET_ASSIGN(Mat3x3, VT_Mat3);
    REINTERPRET_ASSIGN(Mat3x4, VT_Mat3x4);
    REINTERPRET_ASSIGN(BoundingBox, VT_BoundingBox);
    REINTERPRET_ASSIGN(Plane, VT_Plane);
    REINTERPRET_ASSIGN(Ray, VT_Ray);
    REINTERPRET_ASSIGN(Disc, VT_Disc);
    REINTERPRET_ASSIGN(std::string, VT_String);
    REINTERPRET_ASSIGN(VariantVector, VT_VariantVector);
    REINTERPRET_ASSIGN(VariantMap, VT_VariantMap);
    REINTERPRET_ASSIGN(ResponseCurve, VT_ResponseCurve);
    REINTERPRET_ASSIGN(ColorCurves, VT_ColorCurves);
    REINTERPRET_ASSIGN(ColorRamp, VT_ColorRamp);
    REINTERPRET_ASSIGN(ResourceHandle, VT_ResourceHandle);
#undef REINTERPRET_ASSIGN

    Variant& operator=(const VectorBuffer*);
    Variant(const VectorBuffer*);

#define VAR_GET(TYPE, VARTYPE, NAME, DEFAULT, DATA) TYPE get ## NAME (TYPE defVal = DEFAULT) const { return type_ == VARTYPE ? data_.DATA : defVal; }
    VAR_GET(unsigned char, VT_Byte, Byte, 0, uint_);
    VAR_GET(bool, VT_Bool, Bool, false, bool_);
    VAR_GET(int, VT_Int, Int, 0, int_);
    VAR_GET(unsigned, VT_UInt, UInt, 0, uint_);
    VAR_GET(float, VT_Float, Float, 0, float_);
#undef VAR_GET

#define VAR_GET_NO_DEFAULT(TYPE, VARTYPE, NAME, DEFAULT, DATA) TYPE get ## NAME () const { return type_ == VARTYPE ? data_.DATA : DEFAULT; }
    VAR_GET_NO_DEFAULT(StringHash, VT_StringHash, StringHash, 0, uint_);
    VAR_GET_NO_DEFAULT(void*, VT_VoidPtr, VoidPtr, 0, ptr_);
#undef VAR_GET_NO_DEFAULT

#ifndef CppSharp
    #define MEM_GET(TYPE, VARTYPE, DEFAULT) TYPE get ## TYPE (TYPE defVal = DEFAULT) const { TYPE ret = defVal; if (type_ == VARTYPE) memcpy(&ret, &data_, sizeof(TYPE)); return ret; }
    MEM_GET(RangedInt, VT_RangedInt, RangedInt());
    MEM_GET(RangedFloat, VT_RangedFloat, RangedFloat());
    MEM_GET(IntVec2, VT_IntVec2, IntVec2());
    MEM_GET(Vec2, VT_Vec2, Vec2::zero);
    MEM_GET(Vec3, VT_Vec3, Vec3::zero);
    MEM_GET(Vec4, VT_Vec4, Vec4::zero);
    MEM_GET(Quat, VT_Quat, Quat::identity);
    MEM_GET(RGBA, VT_Color, RGBA());
#else
    #define MEM_GET(TYPE) const TYPE& get ## TYPE(TYPE defVal) const;
    MEM_GET(RangedInt);
    MEM_GET(RangedFloat);
    MEM_GET(Vec2);
    MEM_GET(Vec3);
    MEM_GET(Vec4);
    MEM_GET(Quat);
    MEM_GET(RGBA);
    MEM_GET(ResourceHandle);
#endif
#undef MEM_GET

#define REINTERPRET_GET_EXPLICIT(TRUETYPE, TYPE, VARTYPE, DEFAULT) TRUETYPE get ## TYPE () const { return type_ == VARTYPE ? *((TRUETYPE*)data_.ptr_) : DEFAULT; }
#define REINTERPRET_GET(TYPE, VARTYPE, DEFAULT) TYPE get ## TYPE () const { return type_ == VARTYPE ? *((TYPE *)(data_.ptr_)) : DEFAULT; }
    REINTERPRET_GET(Mat3x3, VT_Mat3, Mat3x3());
    REINTERPRET_GET(Mat3x4, VT_Mat3x4, Mat3x4());
    REINTERPRET_GET(BoundingBox, VT_BoundingBox, BoundingBox());
    REINTERPRET_GET(Plane, VT_Plane, Plane());
    REINTERPRET_GET(Ray, VT_Ray, Ray());
    REINTERPRET_GET(Disc, VT_Disc, Disc());
    //REINTERPRET_GET(String, VT_String, "");
    //REINTERPRET_GET_EXPLICIT(std::string, String, VT_String, "");
    std::string getString() const { return type_ == VT_String ? *((std::string*)(data_.ptr_)) : std::string(); }
    REINTERPRET_GET(VariantVector, VT_VariantVector, VariantVector());
    REINTERPRET_GET(VariantMap, VT_VariantMap, VariantMap());
    REINTERPRET_GET(ResponseCurve, VT_ResponseCurve, ResponseCurve());
    REINTERPRET_GET(ColorCurves, VT_ColorCurves, ColorCurves());
    REINTERPRET_GET(ColorRamp, VT_ColorRamp, ColorRamp());
    REINTERPRET_GET(ResourceHandle, VT_ResourceHandle, ResourceHandle());
#undef REINTERPRET_GET
    VectorBuffer* getVectorBuffer() const;

    /// Safely fetch a Vector2 (works with float, Vec2, Vec3, Vec3, RGBA)
    Vec2 getVec2Safe(bool fillingAllValues = false) const;

    /// Safely fetch a Vector3 (works with float, Vec2, Vec3, Vec4, RGBA)
    Vec3 getVec3Safe(bool fillingAllValues = false) const;

    /// Safely fetch a Vector4 (works with float, Vec2, Vec3, Vec4, RGBA)
    Vec4 getVec4Safe(bool fillingAllValues = false) const;

    /// Safely fetch a color (works with float, Vec2, Vec3, RGBA)
    RGBA getColorSafe(bool fillingAllValues = false) const;

    /// Safetly get a floating point value (int, first component of Vec2, Vec3, Vec4, RGBA)
    float getFloatSafe() const;

    /// Convert (if possible) to text.
    std::string ConvertToString() const;
    /// Set value (if possible) from text.
    bool FromString(VariantType type, const std::string& str);

    bool Write(Serializer*) const;
    bool Read(Deserializer*);

#ifndef CppSharp
    template <class T> T get() const
    {
        return T();
    }
#endif

    // Template specializations for templated get
#ifndef CppSharp // do not want to include these in CppSharp processing
#define TEMPLATE_GET(TYPE, NAME) template <> TYPE get< TYPE >() const { return get ## NAME (); }
    TEMPLATE_GET(unsigned char, Byte);
    TEMPLATE_GET(int, Int);
    TEMPLATE_GET(float, Float);
    TEMPLATE_GET(unsigned, UInt);
    TEMPLATE_GET(bool, Bool);
    TEMPLATE_GET(RangedInt, RangedInt);
    TEMPLATE_GET(RangedFloat, RangedFloat);
    TEMPLATE_GET(IntVec2, IntVec2);
    TEMPLATE_GET(Vec2, Vec2);
    TEMPLATE_GET(Vec3, Vec3);
    TEMPLATE_GET(Vec4, Vec4);
    TEMPLATE_GET(Quat, Quat);
    TEMPLATE_GET(RGBA, RGBA);
    TEMPLATE_GET(ResourceHandle, ResourceHandle);
    TEMPLATE_GET(Mat3x3, Mat3x3);
    TEMPLATE_GET(Mat3x4, Mat3x4);
    TEMPLATE_GET(Plane, Plane);
    TEMPLATE_GET(Disc, Disc);
    TEMPLATE_GET(BoundingBox, BoundingBox);
    TEMPLATE_GET(Ray, Ray);
    TEMPLATE_GET(VariantVector, VariantVector);
    TEMPLATE_GET(VariantMap, VariantMap);
    TEMPLATE_GET(StringHash, StringHash);
    //TEMPLATE_GET(std::string, String);
    template <> std::string get< std::string>() const { return getString(); }
    TEMPLATE_GET(ResponseCurve, ResponseCurve);
    TEMPLATE_GET(ColorCurves, ColorCurves);
    TEMPLATE_GET(ColorRamp, ColorRamp);
    TEMPLATE_GET(VectorBuffer*, VectorBuffer);
#undef TEMPLATE_GET
#endif

    /// Limited math operator support to enable use in MathDef's macros for Lerp and such
    Variant operator*(const Variant& rhs);
    Variant operator+(const Variant& rhs);
    Variant operator-(const Variant& rhs);
    Variant operator/(const Variant& rhs);

    void setType(VariantType type);

    /// Comparison operatos are implemented for Vec2/Vec3 and scalar (int, uint, float via getAsDoubleValue)
    bool operator<(const Variant& rhs) const;
    bool operator>(const Variant& rhs) const;
    bool operator==(const Variant& rhs) const;
    bool operator!=(const Variant& rhs) const;
    bool operator<=(const Variant& rhs) const;
    bool operator>=(const Variant& rhs) const;

    /// Reinterpret as a double (for comparisons mostly)
    double getAsDoubleValue() const;
    /// Reinterpret as an int (for comparisons)
    int getAsIntValue() const;

    static const char* VariantTypeToString(VariantType type);
    static VariantType VariantTypeFromString(const char* text);
};

template<typename T>
Variant GetFromVariantVector(const VariantVector& vector, int& index) { if (index < vector.size()) return vector[index++].get<T>(); return Variant(); }

#pragma warning(pop)

}