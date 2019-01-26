#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Math/MathDef.h>
#include <math.h>

namespace SprueEngine
{

struct SPRUE LowestFloat
{

};

struct SPRUE LowestSquareFloat
{
    float value;
    float sqValue;

    // Check if new value is lesser than accept
    bool Update(float newSqValue)
    {
        if (newSqValue < sqValue)
        {
            value = sqrtf(newSqValue);
            sqValue = newSqValue;
            return true;
        }
        return false;
    }
};

struct SPRUE HighestFloat
{
    float value;
    float sqValue;

    bool Update(float newSqValue)
    {
        if (newSqValue > sqValue)
        {
            value = sqrtf(newSqValue);
            sqValue = newSqValue;
            return true;
        }
        return false;
    }
};

}