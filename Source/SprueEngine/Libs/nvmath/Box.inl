// This code is in the public domain -- castanyo@yahoo.es

#pragma once
#ifndef NV_MATH_BOX_INL
#define NV_MATH_BOX_INL

#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>
#include "Box.h"

#include <float.h> // FLT_MAX

namespace nv
{
    // Default ctor.
    //inline Box::Box() { };

    // Copy ctor.
    //inline Box::Box(const Box & b) : minCorner(b.minCorner), maxCorner(b.maxCorner) { }

    // Init ctor.
    //inline Box::Box(const SprueEngine::Vec3 & mins, const SprueEngine::Vec3 & maxs) : minCorner(mins), maxCorner(maxs) { }

    // Assignment operator.
    inline Box & Box::operator=(const Box & b) { minCorner = b.minCorner; maxCorner = b.maxCorner; return *this; }

    // Clear the bounds.
    inline void Box::clearBounds()
    {
        minCorner.Set(FLT_MAX, FLT_MAX, FLT_MAX);
        maxCorner.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    // min < max
    inline bool Box::isValid() const
    {
        return minCorner.x <= maxCorner.x && minCorner.y <= maxCorner.y && minCorner.z <= maxCorner.z;
    }

    // Build a cube centered on center and with edge = 2*dist
    inline void Box::cube(const SprueEngine::Vec3 & center, float dist)
    {
        setCenterExtents(center, SprueEngine::Vec3(dist));
    }

    // Build a box, given center and extents.
    inline void Box::setCenterExtents(const SprueEngine::Vec3 & center, const SprueEngine::Vec3 & extents)
    {
        minCorner = center - extents;
        maxCorner = center + extents;
    }

    // Get box center.
    inline SprueEngine::Vec3 Box::center() const
    {
        return (minCorner + maxCorner) * 0.5f;
    }

    // Return extents of the box.
    inline SprueEngine::Vec3 Box::extents() const
    {
        return (maxCorner - minCorner) * 0.5f;
    }

    // Return extents of the box.
    inline float Box::extents(uint32_t axis) const
    {
        //nvDebugCheck(axis < 3);
        if (axis == 0) return (maxCorner.x - minCorner.x) * 0.5f;
        if (axis == 1) return (maxCorner.y - minCorner.y) * 0.5f;
        if (axis == 2) return (maxCorner.z - minCorner.z) * 0.5f;
        //nvUnreachable();
        return 0.0f;
    }

    // Add a point to this box.
    inline void Box::addPointToBounds(const SprueEngine::Vec3 & p)
    {
        minCorner = std::min(minCorner, p);
        maxCorner = std::max(maxCorner, p);
    }

    // Add a box to this box.
    inline void Box::addBoxToBounds(const Box & b)
    {
        minCorner = std::min(minCorner, b.minCorner);
        maxCorner = std::max(maxCorner, b.maxCorner);
    }

    // Add sphere to this box.
    inline void Box::addSphereToBounds(const SprueEngine::Vec3 & p, float r) {
        minCorner = std::min(minCorner, p - SprueEngine::Vec3(r));
        maxCorner = std::min(maxCorner, p + SprueEngine::Vec3(r));
    }

    // Translate box.
    inline void Box::translate(const SprueEngine::Vec3 & v)
    {
        minCorner += v;
        maxCorner += v;
    }

    // Scale the box.
    inline void Box::scale(float s)
    {
        minCorner *= s;
        maxCorner *= s;
    }

    // Expand the box by a fixed amount.
    inline void Box::expand(float r) {
        minCorner -= SprueEngine::Vec3(r,r,r);
        maxCorner += SprueEngine::Vec3(r,r,r);
    }

    // Get the area of the box.
    inline float Box::area() const
    {
        const SprueEngine::Vec3 d = extents();
        return 8.0f * (d.x*d.y + d.x*d.z + d.y*d.z);
    }	

    // Get the volume of the box.
    inline float Box::volume() const
    {
        SprueEngine::Vec3 d = extents();
        return 8.0f * (d.x * d.y * d.z);
    }

    // Return true if the box contains the given point.
    inline bool Box::contains(const SprueEngine::Vec3 & p) const
    {
        return 
            minCorner.x < p.x && minCorner.y < p.y && minCorner.z < p.z &&
            maxCorner.x > p.x && maxCorner.y > p.y && maxCorner.z > p.z;
    }

    // Split the given box in 8 octants and assign the ith one to this box.
    inline void Box::setOctant(const Box & box, const SprueEngine::Vec3 & center, int i)
    {
        minCorner = box.minCorner;
        maxCorner = box.maxCorner;

        if (i & 4) minCorner.x = center.x;
        else       maxCorner.x = center.x;
        if (i & 2) minCorner.y = center.y;
        else       maxCorner.y = center.y;
        if (i & 1) minCorner.z = center.z;
        else       maxCorner.z = center.z;
    }

} // nv namespace


#endif // NV_MATH_BOX_INL
