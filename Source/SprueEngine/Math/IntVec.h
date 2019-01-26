#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Math/MathDef.h>

#include <memory>

namespace SprueEngine
{

    /// Two component integer vector.
    struct SPRUE IntVec2
    {
        int x;
        int y;

        IntVec2() : x(0), y(0) { }
        IntVec2(int x, int y) : x(x), y(y) { }
        IntVec2(const IntVec2& rhs) : x(rhs.x), y(rhs.y) { }

        inline IntVec2 operator+(const IntVec2& rhs) const { return IntVec2(x + rhs.x, y + rhs.y); }
        inline IntVec2 operator-(const IntVec2& rhs) const { return IntVec2(x - rhs.x, y - rhs.y); }
        inline IntVec2 operator*(const IntVec2& rhs) const { return IntVec2(x * rhs.x, y * rhs.y); }
        inline IntVec2 operator*(float rhs) const { return IntVec2((int)(x * rhs), (int)(y * rhs)); }
        inline IntVec2 operator/(const IntVec2& rhs) const { return IntVec2(x / rhs.x, y / rhs.y); }
        inline IntVec2 operator/(float rhs) const { return IntVec2(x / rhs, y / rhs); }
        inline IntVec2 operator*(int rhs) const { return IntVec2(x * rhs, y * rhs); }
        inline IntVec2& operator+=(const IntVec2& rhs) { x += rhs.x, y += rhs.y; return *this; }
        inline IntVec2& operator-=(const IntVec2& rhs) { x -= rhs.x, y -= rhs.y; return *this; }
        inline IntVec2& operator*=(const IntVec2& rhs) { x *= rhs.x, y *= rhs.y; return *this; }
        inline IntVec2& operator/=(const IntVec2& rhs) { x /= rhs.x, y /= rhs.y; return *this; }
        inline IntVec2& operator*=(int rhs) { x *= rhs, y *= rhs; return *this; }
        inline IntVec2& operator=(const IntVec2& rhs) { x = rhs.x; y = rhs.y; return *this; }

#ifndef CppSharp
        inline bool operator<(const IntVec2& rhs) const { return x < rhs.x && y > rhs.y; }
        inline bool operator>(const IntVec2& rhs) const { return x > rhs.x && y > rhs.y; }
#endif

#ifndef CppSharp
        static IntVec2 Min(const IntVec2& lhs, const IntVec2& rhs) { return IntVec2(SprueMin(lhs.x, rhs.x), SprueMin(lhs.y, rhs.y)); }
        static IntVec2 Max(const IntVec2& lhs, const IntVec2& rhs) { return IntVec2(SprueMax(lhs.x, rhs.x), SprueMax(lhs.y, rhs.y)); }
#endif
    };

    /// Four component integer vector, primary usage is for bone weights.
    struct SPRUE IntVec4
    {
        int x_, y_, z_, w_;

        IntVec4() : x_(-1), y_(-1), z_(-1), w_(-1) { }

        IntVec4(int x, int y, int z, int w) : x_(x), y_(y), z_(z), w_(w) {}

        int& operator[](int index) { return ((int*)this)[index]; }
    };

}