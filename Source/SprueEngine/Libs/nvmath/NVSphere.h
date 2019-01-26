// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#pragma once
#ifndef NV_MATH_SPHERE_H
#define NV_MATH_SPHERE_H

#include <SprueEngine/MathGeoLib/AllMath.h>

namespace nv
{
    
    class Sphere
    {
    public:
        Sphere() {}
        Sphere(SprueEngine::Vec3 center, float radius) : center(center), radius(radius) {}

        Sphere(SprueEngine::Vec3 center) : center(center), radius(0.0f) {}
        Sphere(SprueEngine::Vec3 p0, SprueEngine::Vec3 p1);
        Sphere(SprueEngine::Vec3 p0, SprueEngine::Vec3 p1, SprueEngine::Vec3 p2);
        Sphere(SprueEngine::Vec3 p0, SprueEngine::Vec3 p1, SprueEngine::Vec3 p2, SprueEngine::Vec3 p3);

        SprueEngine::Vec3 center;
        float radius;
    };

    // Returns negative values if point is inside.
    float distanceSquared(const Sphere & sphere, const SprueEngine::Vec3 &point);


    // Welz's algorithm. Fairly slow, recursive implementation uses large stack.
    Sphere miniBall(const SprueEngine::Vec3 * pointArray, uint32_t pointCount);

    Sphere approximateSphere_Ritter(const SprueEngine::Vec3 * pointArray, uint32_t pointCount);
    Sphere approximateSphere_AABB(const SprueEngine::Vec3 * pointArray, uint32_t pointCount);
    Sphere approximateSphere_EPOS6(const SprueEngine::Vec3 * pointArray, uint32_t pointCount);
    Sphere approximateSphere_EPOS14(const SprueEngine::Vec3 * pointArray, uint32_t pointCount);


} // nv namespace


#endif // NV_MATH_SPHERE_H
