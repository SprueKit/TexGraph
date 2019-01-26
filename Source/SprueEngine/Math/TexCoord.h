#pragma once

#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{

    /// Convert a direction vector into cubemap sampling coordinates, Z is cubemap face index
    Vec3 ToCubeMapCoordinates(const Vec3& vector);

}