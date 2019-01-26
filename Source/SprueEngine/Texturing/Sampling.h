#pragma once

#include <SprueEngine/MathGeoLib/AllMath.h>
#include <vector>

namespace SprueEngine
{

    void ComputeRaySamples(const Vec3& aNormal, float aAngle, int aSamples, std::vector<Vec3>& aSampleDirs);

}