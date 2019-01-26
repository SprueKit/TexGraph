#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <stdint.h>

#include <CL/cl.h>

namespace SprueEngine
{

/// Based on OpenEXRs IEEE-754 Half precision floating point type.
/// Used for cut down on memory usage for a volume of voxels.
/// 65^3 is 274,625 cells
///     with float that's 1,098,500 bytes or 1.04mb
///     with half that's    549,200 bytes or 536kb
/// This is more meaningful because the OpenCL voxelizers need to work with 3 normals (9 floats)
/// and 3 distances along an the cardinal edges (3 floats) in addition to the density at the corners
/// which requires 3,570,125 floats at 163.42 mb instead of  81.7mb for half.
/// This data needs to read from the GPU, a smaller volume = less to read.
/// Other values like the edge distances can use even less precision with single byte values as they're in the 0.0-1.0 range.
struct SPRUE Half
{
    uint16_t value_;

    inline operator float() { return 0.0f; }
    Half() { value_ = 0; }
    Half(float value);
    Half(const Half& rhs) { value_ = rhs.value_; }
    explicit Half(uint16_t value) { value_ = value; }
    explicit Half(cl_half value) { value_ = value; }

    Half& operator+=(const Half& rhs);
    Half& operator+=(float rhs);
    Half& operator-=(const Half& rhs);
    Half& operator-=(float rhs);
    Half& operator*=(const Half& rhs);
    Half& operator*=(float rhs);
    Half& operator/=(const Half& rhs);
    Half& operator/=(float rhs);

    Half operator+(const Half& rhs);
    Half operator+(float rhs);
    Half operator-(const Half& rhs);
    Half operator-(float rhs);
    Half operator*(const Half& rhs);
    Half operator*(float rhs);
    Half operator/(const Half& rhs);
    Half operator/(float rhs);

    bool operator==(const Half& rhs);
    bool operator==(float rhs);
    bool operator!=(const Half& rhs);
    bool operator!=(float rhs);

    bool operator>(const Half& rhs);
    bool operator>(float rhs);
    bool operator>=(const Half& rhs);
    bool operator>=(float rhs);
    
    bool operator<(const Half& rhs);
    bool operator<(float rhs);
    bool operator<=(const Half& rhs);
    bool operator<=(float rhs);
};

struct SPRUE HalfVec4
{
    Half x, y, z, w;

    HalfVec4() { }
    HalfVec4(const Vec4& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; }
    HalfVec4(const Vec3& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = 0.0f; }

    inline Vec4 toVec4() { return Vec4(x, y, z, w); }
    inline Vec3 toVec3() { return Vec3(x, y, z); }
};

}