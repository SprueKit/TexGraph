#include "MathDef.h"

namespace SprueEngine
{

    float Sum(float* values, unsigned ct)
    {
        float sum = 0.0f;
        for (unsigned i = 0; i < ct; ++i)
            sum += values[i];
        return sum;
    }

    float NRoot(float* values, unsigned ct)
    {
        float sum = Sum(values, ct);
        return powf(sum, 1.0f / (float)ct);
    }

    int ToArrayIndex(unsigned x, unsigned y, unsigned z, unsigned width, unsigned height, unsigned depth)
    {
        x = CLAMP(x, 0, width - 1);
        y = CLAMP(y, 0, height - 1);
        z = CLAMP(z, 0, depth - 1);
        return (z * width * height + y * width + x);
    }
            
    void FromArrayIndex(unsigned index, unsigned& x, unsigned& y, unsigned& z, unsigned width, unsigned height, unsigned depth)
    {
        z = index / (width * height);
        y = (index - z * width * height) / width;
        x = index - width * (y + height * z);
    }

    int RandomInt(unsigned& seed)
    {
        return seed = (seed * 1103515245 + 12345) & RAND_CAP;
    }

    float RandomFloat(unsigned& seed)
    {
        return ((float)RandomInt(seed)) / ((float)RAND_CAP);
    }

    float RandomFloat(unsigned& seed, float min, float max)
    {
        return min + ((float)RandomInt(seed)) / ((float)(RAND_CAP / (max - min)));
    }
}