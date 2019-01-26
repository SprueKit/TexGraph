#pragma once

#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{

struct Triangle
{
    mutable Vec3 a, b, c;

    Triangle() { }
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c) : a(a), b(b), c(c) { }

    BoundingBox GetBounds() const;
    Vec3 ClosestPoint(const Vec3& p) const;
    Vec3 GetNormal() const { return (b - a).Cross(c - a).Normalized(); }
    bool IntersectRay(const Ray& ray, float* distance = 0x0, Vec3* hitPos = 0x0, Vec3* outBary = 0x0) const;
    float Distance(const Vec3& p) const;
    float Distance2(const Vec3& p) const;
    float SignedDistance(const Vec3& p) const;
    float SignDistance(const Vec3& p, const float inputDist) const;
    bool IntersectRayEitherSide(const Ray& ray, float* dist = 0x0, Vec3* hitPos = 0x0, Vec3* outBary = 0x0) const;
    void Flip() const { std::swap(a, c); }

    Vec3* data() { return &a; }
};

}