// This code is in the public domain -- castanyo@yahoo.es

#pragma once
#ifndef NV_MATH_BOX_H
#define NV_MATH_BOX_H

#include <SprueEngine/MathGeoLib/AllMath.h>

#include <float.h> // FLT_MAX

namespace nv
{
    class Vector;
    class Stream;
    class Sphere;

    // Axis Aligned Bounding Box.
    class Box
    {
    public:

        inline Box() {}
        inline Box(const Box & b) : minCorner(b.minCorner), maxCorner(b.maxCorner) {}
        inline Box(const SprueEngine::Vec3 & mins, const SprueEngine::Vec3 & maxs) : minCorner(mins), maxCorner(maxs) {}

        Box & operator=(const Box & b);

        operator const float * () const { return reinterpret_cast<const float *>(this); }

        // Clear the bounds.
        void clearBounds();

        // min < max
        bool isValid() const;

        // Build a cube centered on center and with edge = 2*dist
        void cube(const SprueEngine::Vec3 & center, float dist);

        // Build a box, given center and extents.
        void setCenterExtents(const SprueEngine::Vec3 & center, const SprueEngine::Vec3 & extents);

        // Get box center.
        SprueEngine::Vec3 center() const;

        // Return extents of the box.
        SprueEngine::Vec3 extents() const;

        // Return extents of the box.
        float extents(uint32_t axis) const;

        // Add a point to this box.
        void addPointToBounds(const SprueEngine::Vec3 & p);

        // Add a box to this box.
        void addBoxToBounds(const Box & b);

        // Add sphere to this box.
        void addSphereToBounds(const SprueEngine::Vec3 & p, float r);

        // Translate box.
        void translate(const SprueEngine::Vec3 & v);

        // Scale the box.
        void scale(float s);

        // Expand the box by a fixed amount.
        void expand(float r);

        // Get the area of the box.
        float area() const;
 
        // Get the volume of the box.
        float volume() const;

        // Return true if the box contains the given point.
        bool contains(const SprueEngine::Vec3& p) const;

        // Split the given box in 8 octants and assign the ith one to this box.
        void setOctant(const Box & box, const SprueEngine::Vec3& center, int i);


        // Clip the given segment against this box.
        bool clipSegment(const SprueEngine::Vec3& origin, const SprueEngine::Vec3& dir, float * t_near, float * t_far) const;


        friend Stream & operator<< (Stream & s, Box & box);

        const SprueEngine::Vec3& corner(int i) const { return (&minCorner)[i]; }

        SprueEngine::Vec3 minCorner;
        SprueEngine::Vec3 maxCorner;
    };

    float distanceSquared(const Box &box, const SprueEngine::Vec3 &point);
    bool overlap(const Box &box, const Sphere &sphere);

    // p is ray origin, id is inverse ray direction.
    bool intersect(const Box & box, const SprueEngine::Vec3 & p, const SprueEngine::Vec3 & id, float * t);

} // nv namespace


#endif // NV_MATH_BOX_H
