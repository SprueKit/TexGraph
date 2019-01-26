#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{
    class Deserializer;
    class Serializer;

    #define ToRGBA8(color) RGBA8(color.r,color.g,color.b,color.a)

    #define ToRGBA(color) RGBA(((float)r) / 255.0f, ((float)g) / 255.0f, ((float)b) / 255.0f, ((float)a) / 255.0f)

    struct SPRUE RGBA8
    {
        unsigned char r, g, b, a;

        RGBA8() : r(0), g(0), b(0), a(255) { }

        RGBA8(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) : r(r), g(g), b(b), a(a) { }

        RGBA8(float r, float g, float b, float a = 1.0f) : r((unsigned char)(r * 255)), g((unsigned char)(g * 255)), b((unsigned char)(b * 255)), a((unsigned char)(a * 255)) { }

        /// For construction from byte stream
        RGBA8(unsigned char* src, unsigned start, unsigned ct)
        {
            r = src[start];
            g = ct > 1 ? src[start + 1] : 0;
            b = ct > 2 ? src[start + 2] : 0;
            a = ct > 3 ? src[start + 3] : 0;
        }
    };

    struct SPRUE RGBA
    {
        float r, g, b, a;

        RGBA() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) { }

        RGBA(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) { }

        /// For construction from a byte stream
        static RGBA FromData(unsigned char* src, int start, int ct) {
            RGBA ret;
            ret.r = src[start] / 255.0f;
            ret.g = ct > 1 ? src[start + 1] / 255.0f : 0.0f;
            ret.b = ct > 2 ? src[start + 2] / 255.0f : 0.0f;
            ret.a = ct > 3 ? src[start + 3] / 255.0f : 1.0f;
            return ret;
        }

        RGBA& operator*=(const RGBA& rhs) {
            r *= rhs.r;
            g *= rhs.g;
            b *= rhs.b;
            a *= rhs.a;
            return *this;
        }
        RGBA operator*(const RGBA& rhs) const { RGBA ret = *this; ret *= rhs; return ret; }

        RGBA& operator*=(float rhs) { r *= rhs; g *= rhs; b *= rhs; a *= rhs; return *this; }
        RGBA operator*(float rhs) const { RGBA ret = *this; ret *= rhs; return ret; }

        RGBA& operator+=(float rhs) { r += rhs; g += rhs; b += rhs; a += rhs; return *this; }
        RGBA operator+(float rhs) { RGBA ret = *this; ret += rhs; return ret; }

        RGBA& operator-=(float rhs) { r -= rhs; g -= rhs; b -= rhs; a -= rhs; return *this; }
        RGBA operator-(float rhs) { RGBA ret = *this; ret -= rhs; return ret; }
    
        RGBA& operator/=(const RGBA& rhs) {
            r /= SprueMax(rhs.r, EPSILON);
            g /= SprueMax(rhs.g, EPSILON);
            b /= SprueMax(rhs.b, EPSILON);
            a /= SprueMax(rhs.a, EPSILON);
            return *this;
        }
        RGBA& operator/=(float rhs) {
            r /= SprueMax(rhs, EPSILON);
            g /= SprueMax(rhs, EPSILON);
            b /= SprueMax(rhs, EPSILON);
            a /= SprueMax(rhs, EPSILON);
            return *this;
        }
        RGBA operator/(const RGBA& rhs) const { RGBA ret = *this; ret /= rhs; return ret; }
        RGBA operator/(float rhs) const { RGBA ret = *this; ret /= rhs; return ret; }

        RGBA& operator+=(const RGBA& rhs) {
            r += rhs.r;
            g += rhs.g;
            b += rhs.b;
            a += rhs.a;
            return *this;
        }
        RGBA operator+(const RGBA& rhs) const { RGBA ret = *this; ret += rhs; return ret; }

        bool operator==(const RGBA& rhs) const { 
            return SprueEquals(r, rhs.r) &&
            SprueEquals(g, rhs.g) &&
            SprueEquals(b, rhs.b) &&
            SprueEquals(a, rhs.a);
        }
        bool operator!=(const RGBA& rhs) const {
            return !SprueEquals(r, rhs.r) &&
                !SprueEquals(g, rhs.g) &&
                !SprueEquals(b, rhs.b) &&
                !SprueEquals(a, rhs.a);
        }
        RGBA& operator-=(const RGBA& rhs) {
            r -= rhs.r;
            g -= rhs.g;
            b -= rhs.b;
            a -= rhs.a;
            return *this;
        }
        RGBA operator-(const RGBA& rhs) const { RGBA ret = *this; ret -= rhs; return ret; }

        float AverageRGB() const { return (r + g + b) / 3.0f; }
        float Average() const { return (r + g + b + a) / 4.0f; }
        float Brightness() const { return (r * 0.2126f + g * 0.7152f + b * 0.0722f); }
        RGBA& Clip() { r = CLAMP01(r); g = CLAMP01(g); b = CLAMP01(b); a = CLAMP01(a); return *this; }
        float Distance(const RGBA& rhs) const;

        void Set(float val) { r = g = b = val; }
        void Set(float r, float g, float b) { this->r = r; this->g = g; this->b = b; }
        void Set(float r, float g, float b, float a) { this->r = r; this->g = g; this->b = b; this->a = a; }
        void Set(const Vec3& vec) { Set(vec.x, vec.y, vec.z); }

        void FromNormal(const Vec3& norm) { Set(norm.x * 0.5f + 0.5f, norm.z * 0.5f + 0.5f, norm.y * 0.5f + 0.5f); }
        Vec3 ToNormal() { Vec3 ret(r, b, g); ret -= 0.5f; ret *= 2.0f; return ret; }

        bool IsValid() const;

#ifndef CppSharp
        static const RGBA Clear;
        static const RGBA Black;
        static const RGBA Red;
        static const RGBA Green;
        static const RGBA Blue;
        static const RGBA White;
        static const RGBA Gray;
        static const RGBA Gold;
        static const RGBA Yellow;
        static const RGBA Cyan;
        static const RGBA Magenta;
        static const RGBA Invalid;
#endif
    };

    /// Manages a gradient ramp with multiple stops.
    struct SPRUE ColorRamp
    {
        ColorRamp() {
            colors.push_back(std::make_pair(0.0f, RGBA(0, 0, 0)));
            colors.push_back(std::make_pair(1.0f, RGBA(1, 1, 1)));
        }

        std::vector<std::pair<float, RGBA> > colors;

        RGBA Get(float position) const;

        void Deserialize(Deserializer* src);
        void Serialize(Serializer* dest);
    };

    struct SPRUE ColorCurve
    {
        std::vector<Vec2> knots_;

        float GetY(float t) const;
        void MakeLinear() { knots_.clear(); knots_.push_back(Vec2(0, 0)); knots_.push_back(Vec2(1, 1)); CalculateDerivatives(); }
        void CalculateDerivatives();

        void Deserialize(Deserializer*);
        void Serialize(Serializer*) const;
    private:
        std::vector<float> derivatives_;
    };

    struct SPRUE ColorCurves
    {
        ColorCurve R;
        ColorCurve G;
        ColorCurve B;
        ColorCurve A;

        ColorCurves() { R.MakeLinear(); G.MakeLinear();  B.MakeLinear(); A.MakeLinear(); }
        
        void MakeIdentical(int src) {
            R = (*this)[src];
            G = (*this)[src];
            B = (*this)[src];
        }

        RGBA Curve(const RGBA& in)
        {
            return RGBA(
                R.GetY(in.r),
                G.GetY(in.g),
                B.GetY(in.b),
                A.GetY(in.a));
        }

        ColorCurve& operator[](int idx) {
            switch (idx) {
            case 0: return R;
            case 1: return G;
            case 2: return B;
            case 3: return A;
            }
            return R;
        }
        void Deserialize(Deserializer*);
        void Serialize(Serializer*) const;
    };
}