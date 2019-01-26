#pragma once

#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{
    struct IntersectionInfo {
        float t; // Intersection distance along the ray
        void* object; // Object that was hit
        void* parent; // Parent in the case of a deep hit.
        Vec3 hit; // Location of the intersection
    };
}