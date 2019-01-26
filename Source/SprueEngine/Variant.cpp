#include "SprueEngine/Deserializer.h"
#include "SprueEngine/StringConversion.h"
#include "SprueEngine/Serializer.h"
#include "SprueEngine/Variant.h"
#include "VectorBuffer.h"

namespace SprueEngine
{

std::string Variant::ConvertToString() const
{
    switch (type_)
    {
    case VT_Byte:
        return SprueEngine::ToString((int)getByte());
    case VT_Bool:
        return SprueEngine::ToString(getBool());
    case VT_Int:
        return SprueEngine::ToString(getInt());
    case VT_UInt:
        return SprueEngine::ToString(getUInt());
    case VT_Float:
        return SprueEngine::ToString(getFloat());
    case VT_RangedInt:
        return SprueEngine::ToString(getRangedInt());
    case VT_RangedFloat:
        return SprueEngine::ToString(getRangedFloat());
    case VT_IntVec2:
        return SprueEngine::ToString(getIntVec2());
    case VT_Vec2:
        return SprueEngine::ToString(getVec2());
    case VT_Vec3:
        return SprueEngine::ToString(getVec3());
    case VT_Color:
        return SprueEngine::ToString(getRGBA());
    case VT_Quat:
        return SprueEngine::ToString(getQuat());
    case VT_Mat3:
        return SprueEngine::ToString(getMat3x3());
    case VT_Mat3x4:
        return SprueEngine::ToString(getMat3x4());
    case VT_BoundingBox:
        return SprueEngine::ToString(getBoundingBox());
    case VT_Plane:
        return SprueEngine::ToString(getPlane());
    case VT_Disc:
        return SprueEngine::ToString(getDisc());
    case VT_Ray:
        return SprueEngine::ToString(getRay());
    case VT_ResponseCurve:
        return SprueEngine::ToString(getResponseCurve());
    case VT_ColorCurves:
        return SprueEngine::ToString(getColorCurves());
    case VT_ColorRamp:
        return SprueEngine::ToString(getColorRamp());
    case VT_VariantVector:
        return SprueEngine::ToString(getVariantVector());
    case VT_VariantMap:
        return SprueEngine::ToString(getVariantMap());
    case VT_StringHash:
        return SprueEngine::ToString(getStringHash());
    case VT_ResourceHandle:
        return SprueEngine::ToString(getResourceHandle());
    case VT_String:
    {
        std::string* str = ((std::string*)data_.ptr_);
        return *str;
    }
    case VT_VoidPtr:
    {
        std::stringstream ss;
        ss << std::hex << (unsigned)data_.ptr_;
        return ss.str();
    }
    case VT_VectorBuffer:
        return SprueEngine::ToString(*getVectorBuffer());
        //return getString();
    }
    SPRUE_ASSERT(false, "Encountered a Variant type that is not handled for Variant::ConvertToString");
    return "";
}

bool Variant::FromString(VariantType type, const std::string& str)
{
    setType(type);

    switch (type)
    {
#define VAR_HANDLER(TYPE, VARTYPE) case VARTYPE : { TYPE junk; SprueEngine::FromString(str, &junk); *this = junk; return true; }
        VAR_HANDLER(int, VT_Byte)
        VAR_HANDLER(bool, VT_Bool)
        VAR_HANDLER(int, VT_Int)
        VAR_HANDLER(float, VT_Float)
        VAR_HANDLER(unsigned, VT_UInt)
        VAR_HANDLER(RangedInt, VT_RangedInt)
        VAR_HANDLER(RangedFloat, VT_RangedFloat)
        VAR_HANDLER(IntVec2, VT_IntVec2)
        VAR_HANDLER(Vec2, VT_Vec2)
        VAR_HANDLER(Vec3, VT_Vec3)
        VAR_HANDLER(RGBA, VT_Color)
        VAR_HANDLER(Quat, VT_Quat)
        VAR_HANDLER(Mat3x3, VT_Mat3)
        VAR_HANDLER(Mat3x4, VT_Mat3x4)
        VAR_HANDLER(BoundingBox, VT_BoundingBox)
        VAR_HANDLER(Plane, VT_Plane)
        VAR_HANDLER(Disc, VT_Disc)
        VAR_HANDLER(Ray, VT_Ray)
        VAR_HANDLER(ResponseCurve, VT_ResponseCurve)
        VAR_HANDLER(ColorRamp, VT_ColorRamp)
        VAR_HANDLER(VariantVector, VT_VariantVector)
        VAR_HANDLER(VariantMap, VT_VariantMap)
        VAR_HANDLER(StringHash, VT_StringHash)
        VAR_HANDLER(ResourceHandle, VT_ResourceHandle)
        VAR_HANDLER(ColorCurves, VT_ColorCurves)
#undef VAR_HANDLER
    case VT_String:
        *this = str;
        return true;
    case VT_VoidPtr:
        data_.ptr_ = (void*)std::stoul(str, 0, 16);
        return true;
    case VT_VectorBuffer: {
        VectorBuffer buffer;
        SprueEngine::FromString(str, &buffer);
        *this = &buffer;
        return true;
    }
    }
    
    SPRUE_ASSERT(false, "Encountered a Variant type that is not handled for Variant::FromString");
    return false;
}

void Variant::setType(VariantType type)
{
    if (type_ == type)
        return;

#define VAR_RELEASE(TYPE, VARTYPE) case VARTYPE: delete ((TYPE *)(data_.ptr_)); break;
    if (data_.ptr_ != 0x0)
    switch (type_)
    {
        VAR_RELEASE(Mat3x3, VT_Mat3);
        VAR_RELEASE(Mat3x4, VT_Mat3x4);
        VAR_RELEASE(BoundingBox, VT_BoundingBox);
        VAR_RELEASE(Plane, VT_Plane);
        VAR_RELEASE(Ray, VT_Ray);
        VAR_RELEASE(Disc, VT_Disc);
        VAR_RELEASE(std::string, VT_String);
        VAR_RELEASE(VariantVector, VT_VariantVector);
        VAR_RELEASE(VariantMap, VT_VariantMap);
        VAR_RELEASE(ColorCurves, VT_ColorCurves);
        VAR_RELEASE(ColorRamp, VT_ColorRamp);
        VAR_RELEASE(ResourceHandle, VT_ResourceHandle);
        VAR_RELEASE(VectorBuffer, VT_VectorBuffer);
    }
    memset(&data_, 0, sizeof(data_));
#undef VAR_RELEASE

#define VAR_ALLOC(TYPE, VARTYPE) case VARTYPE: data_.ptr_ = new TYPE(); break;
    type_ = type;
    switch (type_)
    {
        VAR_ALLOC(Mat3x3, VT_Mat3);
        VAR_ALLOC(Mat3x4, VT_Mat3x4);
        VAR_ALLOC(BoundingBox, VT_BoundingBox);
        VAR_ALLOC(Plane, VT_Plane);
        VAR_ALLOC(Ray, VT_Ray);
        VAR_ALLOC(Disc, VT_Disc);
        VAR_ALLOC(std::string, VT_String);
        VAR_ALLOC(VariantVector, VT_VariantVector);
        VAR_ALLOC(VariantMap, VT_VariantMap);
        VAR_ALLOC(ColorCurves, VT_ColorCurves);
        VAR_ALLOC(ColorRamp, VT_ColorRamp);
        VAR_ALLOC(ResourceHandle, VT_ResourceHandle);
        VAR_ALLOC(VectorBuffer, VT_VectorBuffer);
    }
#undef VAR_ALLOC
}

#define VAR_ASSIGN(TYPE, VARTYPE) case VARTYPE: *((TYPE*)(data_.ptr_)) = *((TYPE*)(rhs.data_.ptr_)); break
#define VAR_COPY(TYPE, VARTYPE) case VARTYPE: memcpy(&data_, &rhs.data_, sizeof(data_)); break

void Variant::operator=(const Variant& rhs)
{
    setType(rhs.type_);
    switch (type_)
    {
        VAR_ASSIGN(Mat3x3, VT_Mat3);
        VAR_ASSIGN(Mat3x4, VT_Mat3x4);
        VAR_ASSIGN(BoundingBox, VT_BoundingBox);
        VAR_ASSIGN(Plane, VT_Plane);
        VAR_ASSIGN(Ray, VT_Ray);
        VAR_ASSIGN(Disc, VT_Disc);
        VAR_ASSIGN(std::string, VT_String);
        VAR_ASSIGN(VariantVector, VT_VariantVector);
        VAR_ASSIGN(VariantMap, VT_VariantMap);
        VAR_ASSIGN(ColorCurves, VT_ColorCurves);
        VAR_ASSIGN(ColorRamp, VT_ColorRamp);
        VAR_ASSIGN(ResourceHandle, VT_ResourceHandle);
        VAR_ASSIGN(VectorBuffer, VT_VectorBuffer);
    default:
        memcpy(&data_, &rhs.data_, sizeof(data_));
    }
}

bool Variant::Write(Serializer* dst) const
{
    dst->WriteUByte(type_);
    if (type_ == VT_None)
        return true;
    
#define VAR_WRITE(TYPE, VARTYPE, TYPENAME, FUNCNAME) case VARTYPE: return dst->Write ## FUNCNAME (get ## TYPENAME ());
    switch (type_)
    {
        VAR_WRITE(bool, VT_Bool, Bool, Bool)
        VAR_WRITE(int, VT_Int, Int, Int)
        VAR_WRITE(unsigned, VT_UInt, UInt, UInt)
        VAR_WRITE(float, VT_Float, Float, Float)
        VAR_WRITE(std::string, VT_String, String, String)
        VAR_WRITE(RGBA, VT_Color, RGBA, Color)
        VAR_WRITE(Vec2, VT_Vec2, Vec2, Vector2)
        VAR_WRITE(Vec3, VT_Vec3, Vec3, Vector3)
        VAR_WRITE(Quat, VT_Quat, Quat, Quaternion)
        VAR_WRITE(Mat3x3, VT_Mat3, Mat3x3, Matrix3)
        VAR_WRITE(Mat3x4, VT_Mat3x4, Mat3x4, Matrix3x4)
        case VT_RangedInt: {
            RangedInt val = getRangedInt();
            dst->WriteInt(val.getLowerBound());
            dst->WriteInt(val.getUpperBound()); }
            return true;
        case VT_RangedFloat: {
            RangedFloat val = getRangedFloat();
            dst->WriteFloat(val.getLowerBound());
            dst->WriteFloat(val.getUpperBound()); }
            return true;
        case VT_ColorCurves:
            getColorCurves().Serialize(dst);
            return true;
        case VT_ColorRamp:
            getColorRamp().Serialize(dst);
            return true;
        case VT_ResourceHandle: {
            ResourceHandle res = getResourceHandle();
            dst->WriteStringHash(res.Type);
            dst->WriteString(res.Name);
            return true;
        }
        case VT_VectorBuffer: {
            dst->WriteBuffer(getVectorBuffer()->GetBuffer());
            return true;
        }
    default:
        SPRUE_ASSERT(false, "Serialization of variant type not handled");
        return false;
    }
#undef VAR_WRITE
}

bool Variant::Read(Deserializer* src)
{
    setType((VariantType)src->ReadUByte());
    if (type_ == VT_None)
        return true;

#define VAR_READ(TYPE, VARTYPE, FUNCNAME) case VARTYPE: *this = src->Read ## FUNCNAME (); return true;
    switch (type_)
    {
        VAR_READ(bool, VT_Bool, Bool)
        VAR_READ(int, VT_Int, Int)
        VAR_READ(unsigned, VT_UInt, UInt)
        VAR_READ(float, VT_Float, Float)
        VAR_READ(String, VT_String, String)
        VAR_READ(RGBA, VT_Color, Color)
        VAR_READ(Vec2, VT_Vec2, Vector2)
        VAR_READ(Vec3, VT_Vec3, Vector3)
        VAR_READ(Quat, VT_Quat, Quaternion)
        VAR_READ(Mat3x3, VT_Mat3, Matrix3)
        VAR_READ(Mat3x4, VT_Mat3x4, Matrix3x4)
        case VT_RangedInt:
            *this = RangedInt(src->ReadInt(), src->ReadInt());
            return true;
        case VT_RangedFloat:
            *this = RangedFloat(src->ReadFloat(), src->ReadFloat());
            return true;
        case VT_ColorCurves: {
            ColorCurves curves;
            curves.Deserialize(src);
            *this = curves;
        } return true;
        case VT_ColorRamp: {
            ColorRamp ramp;
            ramp.Deserialize(src);
            *this = ramp;
        } return true;
        case VT_ResourceHandle: {
            ResourceHandle handle;
            handle.Type = src->ReadStringHash();
            handle.Name = src->ReadString();
            *this = handle;
        } return true;
        case VT_VectorBuffer: {
            VectorBuffer buffer(src->ReadBuffer());
            *this = &buffer;
        } return true;
    default:
        SPRUE_ASSERT(false, "Deserialization of variant type not handled");
        return false;
    }
#undef VAR_READ

    return true;
}

Vec2 Variant::getVec2Safe(bool fillingAllValues) const
{
    if (type_ == VT_Vec4)
        return getVec4().xy();
    if (type_ == VT_Vec2)
        return getVec2();
    if (type_ == VT_Vec3)
    {
        Vec3 val = getVec3();
        return Vec2(val.x, val.y);
    }
    if (type_ == VT_Float)
    {
        float fValue = getFloat();
        return fillingAllValues ? Vec2(fValue, fValue) : Vec2(fValue, 0.0f);
    }
    return Vec2::zero;
}

Vec3 Variant::getVec3Safe(bool fillingAllValues) const {
    if (type_ == VT_Vec4)
        return getVec4().xyz();
    if (type_ == VT_Vec3)
        return getVec3();
    else if (type_ == VT_Vec2)
    {
        Vec2 val = getVec2();
        return Vec3(val.x, val.y, 0.0f);
    }
    if (type_ == VT_Float)
    {
        float fValue = getFloat();
        return fillingAllValues ? Vec3(fValue, fValue, fValue) : Vec3(fValue, 0.0f, 0.0f);
    }
    return Vec3::zero;
}

Vec4 Variant::getVec4Safe(bool fillingAllValues) const
{
    if (type_ == VT_Vec4)
        return getVec4();
    if (type_ == VT_Vec3)
        return Vec4(getVec3());
    else if (type_ == VT_Vec2)
    {
        Vec2 val = getVec2();
        return Vec4(val.x, val.y, 0.0f, 0.0f);
    }
    if (type_ == VT_Float)
    {
        float fValue = getFloat();
        return fillingAllValues ? Vec4(fValue, fValue, fValue, fValue) : Vec4(fValue, 0.0f, 0.0f, 0.0f);
    }
    return Vec3::zero;
}

RGBA Variant::getColorSafe(bool fillingAllValues) const {
    if (type_ == VT_Color)
        return getRGBA();
    if (type_ == VT_Float)
    {
        float fValue = getFloat();
        return fillingAllValues ? RGBA(fValue, fValue, fValue) : RGBA(fValue, 0.0f, 0.0f);
    }
    return RGBA::Clear;
}

float Variant::getFloatSafe() const {
    if (type_ == VT_Float)
        return getFloat();
    else if (type_ == VT_Byte)
        return getByte();
    else if (type_ == VT_Int)
        return getInt();
    else if (type_ == VT_UInt)
        return getUInt();
    else if (type_ == VT_Color)
        return getRGBA().r;
    else if (type_ == VT_Vec2)
        return getVec2().x;
    else if (type_ == VT_Vec3)
        return getVec3().x;
    return 0.0f;
}

Variant Variant::operator*(const Variant& rhs)
{
    if (getType() == VT_Float)
    {
        if (rhs.getType() == VT_Float)
            return Variant(getFloat() * rhs.getFloat());
        else if (rhs.getType() == VT_Color)
            return Variant(rhs.getRGBA() * getFloat());
    }
    else if (getType() == VT_Color)
    {
        if (rhs.getType() == VT_Float)
            return Variant(getRGBA() * rhs.getFloat());
        else if (rhs.getType() == VT_Color)
            return Variant(getRGBA() * rhs.getRGBA());
    }
    return Variant();
}

Variant Variant::operator+(const Variant& rhs)
{
    if (getType() == VT_Float)
    {
        if (rhs.getType() == VT_Float)
            return Variant(getFloat() + rhs.getFloat());
        else if (rhs.getType() == VT_Color)
            return Variant(rhs.getRGBA() + getFloat());
    }
    else if (getType() == VT_Color)
    {
        if (rhs.getType() == VT_Float)
            return Variant(getRGBA() + rhs.getFloat());
        else if (rhs.getType() == VT_Color)
            return Variant(getRGBA() + rhs.getRGBA());
    }
    return Variant();
}

Variant Variant::operator-(const Variant& rhs)
{
    if (getType() == VT_Float)
    {
        if (rhs.getType() == VT_Float)
            return Variant(getFloat() - rhs.getFloat());
        else if (rhs.getType() == VT_Color)
            return Variant(rhs.getRGBA() - getFloat());
    }
    else if (getType() == VT_Color)
    {
        if (rhs.getType() == VT_Float)
            return Variant(getRGBA() - rhs.getFloat());
        else if (rhs.getType() == VT_Color)
            return Variant(getRGBA() - rhs.getRGBA());
    }
    return Variant();
}

Variant Variant::operator/(const Variant& rhs)
{
    if (getType() == VT_Float)
    {
        if (rhs.getType() == VT_Float)
            return Variant(getFloat() / rhs.getFloat());
        else if (rhs.getType() == VT_Color)
            return Variant(rhs.getRGBA() / getFloat());
    }
    else if (getType() == VT_Color)
    {
        if (rhs.getType() == VT_Float)
            return Variant(getRGBA() / rhs.getFloat());
        else if (rhs.getType() == VT_Color)
            return Variant(getRGBA() / rhs.getRGBA());
    }
    return Variant();
}

bool Variant::operator<(const Variant& rhs) const {
    if (this->getType() == VT_Vec3 && rhs.getType() == VT_Vec3)
        return getVec3() < rhs.getVec3();
    if (this->getType() == VT_Vec2 && rhs.getType() == VT_Vec2)
        return getVec2() < rhs.getVec2();
    return getAsDoubleValue() < rhs.getAsDoubleValue();
}

bool Variant::operator>(const Variant& rhs) const {
    if (this->getType() == VT_Vec3 && rhs.getType() == VT_Vec3)
        return getVec3() > rhs.getVec3();
    if (this->getType() == VT_Vec2 && rhs.getType() == VT_Vec2)
        return getVec2() > rhs.getVec2();
    return getAsDoubleValue() > rhs.getAsDoubleValue();
}

bool Variant::operator==(const Variant& rhs) const {
    if (this->getType() == VT_Vec3 && rhs.getType() == VT_Vec3)
        return getVec3() == rhs.getVec3();
    if (this->getType() == VT_Quat && rhs.getType() == VT_Quat)
        return getQuat().Equals(rhs.getQuat());
    if (this->getType() == VT_Vec2 && rhs.getType() == VT_Vec2)
        return getVec2() == rhs.getVec2();
    if (this->getType() == VT_String && rhs.getType() == VT_String)
        return getString().compare(rhs.getString()) == 0;
    if (this->getType() == VT_VoidPtr && rhs.getType() == VT_VoidPtr)
        return getVoidPtr() == rhs.getVoidPtr();
    return getAsDoubleValue() == rhs.getAsDoubleValue();
}

bool Variant::operator!=(const Variant& rhs) const {
    if (this->getType() == VT_Vec3 && rhs.getType() == VT_Vec3)
        return !(getVec3() == rhs.getVec3());
    if (this->getType() == VT_Vec2 && rhs.getType() == VT_Vec2)
        return !(getVec2() == rhs.getVec2());
    if (this->getType() == VT_String && rhs.getType() == VT_String)
        return getString().compare(rhs.getString()) != 0;
    if (this->getType() == VT_VoidPtr && rhs.getType() == VT_VoidPtr)
        return getVoidPtr() != rhs.getVoidPtr();
    return getAsDoubleValue() != rhs.getAsDoubleValue();
}

bool Variant::operator<=(const Variant& rhs) const {
    if (this->getType() == VT_Vec3 && rhs.getType() == VT_Vec3)
    {
        Vec3 l = getVec3(); Vec3 r = rhs.getVec3();
        return l < r || l == r;
    }
    if (this->getType() == VT_Vec2 && rhs.getType() == VT_Vec2)
    {
        Vec2 l = getVec2(); Vec2 r = rhs.getVec2();
        return l < r || l == r;
    }
    return getAsDoubleValue() <= rhs.getAsDoubleValue();
}

bool Variant::operator>=(const Variant& rhs) const {
    if (this->getType() == VT_Vec3 && rhs.getType() == VT_Vec3)
    {
        Vec3 l = getVec3(); Vec3 r = rhs.getVec3();
        return l > r || l == r;
    }
    if (this->getType() == VT_Vec2 && rhs.getType() == VT_Vec2)
    {
        Vec2 l = getVec2(); Vec2 r = rhs.getVec2();
        return l > r || l == r;
    }
    return getAsDoubleValue() >= rhs.getAsDoubleValue();
}

double Variant::getAsDoubleValue() const
{
    if (type_ == VT_Int)
        return getInt();
    if (type_ == VT_Float)
        return getFloat();
    if (type_ == VT_UInt)
        return getUInt();
    if (type_ == VT_Bool)
        return getBool();
    return 0.0;
}

int Variant::getAsIntValue() const {
    if (type_ == VT_Int)
        return getInt();
    if (type_ == VT_Float)
        return getFloat();
    if (type_ == VT_UInt)
        return getUInt();
    if (type_ == VT_Bool)
        return getBool();
    return 0;
}

#define VT_TO_STRING(TYPE) case TYPE: return #TYPE ;
const char* Variant::VariantTypeToString(VariantType type)
{
    switch (type)
    {
        VT_TO_STRING(VT_None)
        VT_TO_STRING(VT_Byte)
        VT_TO_STRING(VT_Bool)
        VT_TO_STRING(VT_Int)
        VT_TO_STRING(VT_UInt)
        VT_TO_STRING(VT_Float)
        VT_TO_STRING(VT_RangedInt)
        VT_TO_STRING(VT_RangedFloat)
        VT_TO_STRING(VT_IntVec2)
        VT_TO_STRING(VT_Vec2)
        VT_TO_STRING(VT_Vec3)
        VT_TO_STRING(VT_Mat3)
        VT_TO_STRING(VT_Quat)
        VT_TO_STRING(VT_Mat3x4)
        VT_TO_STRING(VT_Plane)
        VT_TO_STRING(VT_BoundingBox)
        VT_TO_STRING(VT_Color)
        VT_TO_STRING(VT_Ray)
        VT_TO_STRING(VT_Disc)
        VT_TO_STRING(VT_String)
        VT_TO_STRING(VT_VariantVector)
        VT_TO_STRING(VT_VariantMap)
        VT_TO_STRING(VT_StringHash)
        VT_TO_STRING(VT_ResponseCurve)
        VT_TO_STRING(VT_ResourceHandle)
        VT_TO_STRING(VT_VoidPtr)
        VT_TO_STRING(VT_ColorCurves)
        VT_TO_STRING(VT_ColorRamp)
    }
    return 0x0;
}

#define VT_FROM_STRING(TYPE) if (strcmp(#TYPE, text) == 0) return TYPE;
VariantType Variant::VariantTypeFromString(const char* text)
{
    VT_FROM_STRING(VT_None)
    VT_FROM_STRING(VT_Byte)
    VT_FROM_STRING(VT_Bool)
    VT_FROM_STRING(VT_Int)
    VT_FROM_STRING(VT_UInt)
    VT_FROM_STRING(VT_Float)
    VT_FROM_STRING(VT_RangedInt)
    VT_FROM_STRING(VT_RangedFloat)
    VT_FROM_STRING(VT_IntVec2)
    VT_FROM_STRING(VT_Vec2)
    VT_FROM_STRING(VT_Vec3)
    VT_FROM_STRING(VT_Mat3)
    VT_FROM_STRING(VT_Quat)
    VT_FROM_STRING(VT_Mat3x4)
    VT_FROM_STRING(VT_Plane)
    VT_FROM_STRING(VT_BoundingBox)
    VT_FROM_STRING(VT_Color)
    VT_FROM_STRING(VT_Ray)
    VT_FROM_STRING(VT_Disc)
    VT_FROM_STRING(VT_String)
    VT_FROM_STRING(VT_VariantVector)
    VT_FROM_STRING(VT_VariantMap)
    VT_FROM_STRING(VT_StringHash)
    VT_FROM_STRING(VT_ResponseCurve)
    VT_FROM_STRING(VT_ResourceHandle)
    VT_FROM_STRING(VT_VoidPtr)
    VT_FROM_STRING(VT_ColorCurves)
    VT_FROM_STRING(VT_ColorRamp)

    return VT_None;
}

Variant::Variant(const VectorBuffer* buffer)
{
    setType(VT_VectorBuffer);
    *((VectorBuffer*)data_.ptr_) = *buffer;
}

Variant& Variant::operator=(const VectorBuffer* buffer)
{
    setType(VT_VectorBuffer);
    *((VectorBuffer*)data_.ptr_) = *buffer;
    return *this;
}

VectorBuffer* Variant::getVectorBuffer() const
{
    return (VectorBuffer*)data_.ptr_;
}

}