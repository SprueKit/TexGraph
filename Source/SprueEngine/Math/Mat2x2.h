#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{

    struct SPRUE Mat2x2
    {
        float m[2][2];

        Mat2x2(float a, float b, float c, float d)
        {
            m[0][0] = a;
            m[0][1] = b;
            m[1][0] = c;
            m[1][1] = d;
        }

        Vec2 operator*(const Vec2& vec) const {
            return Vec2(vec.x * m[0][0] + vec.y * m[0][1], vec.x * m[1][0] + vec.y * m[1][1]);
        }
    };

}