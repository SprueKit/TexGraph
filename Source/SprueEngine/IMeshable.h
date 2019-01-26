#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{

class SPRUE IMeshable
{
public:
    virtual float CalculateDensity(const Vec3& position) const = 0;
    virtual bool OverrideCornerDetection(const Vec3& cell, int& corners) const { return false; }
    virtual bool OverrideEdgeSampling(const Vec3& minPosition, int edgeIndex, Vec3& normal, float& distance) const { return false; }
};

}