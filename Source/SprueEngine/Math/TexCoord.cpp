#include "TexCoord.h"

namespace SprueEngine
{
    
    Vec3 ToCubeMapCoordinates(const Vec3& vector)
    {
        const unsigned longestIndex = vector.Abs().MaxElementIndex();
        const float ma = fabsf(vector[longestIndex]);
        const bool dominantIsPositive = vector[longestIndex] > 0.0f;
        float sc = 0.0f, tc = 0.0f;
        float faceIndex = 0.0f;
        if (longestIndex == 0)
        {
            faceIndex = dominantIsPositive ? 0 : 1;
            sc = dominantIsPositive ? -vector.z : vector.z;
            tc = -vector.y;
        }
        else if (longestIndex == 1)
        {
            faceIndex = dominantIsPositive ? 2 : 3;
            sc = vector.x;
            tc = dominantIsPositive ? vector.z : -vector.z;
        }
        else if (longestIndex == 2)
        {
            faceIndex = dominantIsPositive ? 4 : 5;
            sc = dominantIsPositive ? vector.x : -vector.x;
            tc = -vector.y;
        }

        const float s = (sc / ma + 1.0f) / 2.0f;
        const float t = (tc / ma + 1.0f) / 2.0f;

        return Vec3(s, t, faceIndex);
    }
}