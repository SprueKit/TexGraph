#pragma once

#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/Math/IntersectionInfo.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{

    /// An object that can be ray-selected.
    class ISelectable
    {
    public:
        /// Check a raycast against our bounds
        virtual bool TestRayFast(const Ray& ray, IntersectionInfo* info) const = 0;
        
        /// Perform a reasonable accurate ray test (default is to just use the TestRayFast against the bounds)
        virtual bool TestRayAccurate(const Ray& ray, IntersectionInfo* info) const { return TestRayFast(ray, info); }
    };

    /// An object that needs to enumerate selectable objects for consideration.
    class IContainSelectables
    {
    public:
        virtual void CollectSelectables(std::vector<ISelectable*>& selectables) = 0;
    };
}