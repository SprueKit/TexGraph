#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/GeneralUtility.h>
#include <math.h>

namespace SprueEngine
{

#define PI 3.141596f
#define PI2  6.283192f
#define HALFPI 1.570798f

#define RAD_TO_DEG 57.2958f
#define DEG_TO_RAD 0.0174533f

#define BIG_EPSILON 1e-5f
#define EPSILON 1e-7f

#define RAND_CAP ((1U << 32) - 1)

#define NORMALIZE(value, min, max) (((value) - (min)) / ((max) - (min)))
#define DENORMALIZE(value, min, max) ((value) * ((max) - (min)) + (min))

#define CLAMP(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

#define CLAMP01(value) (value < 0.0f ? 0.0f : ((value) > 1.0f ? 1.0f : (value)))

// These are only safe for incremental wraps
#define WRAPZERO(value, max) ((value) > (max) ? ((value) - (max) - 1) : (value))
#define WRAP(value, min, max) ((value) > (max) ? ((value) - (max) + (min) - 1) : (value))

#define SprueMin(a, b) ((a) < (b) ? (a) : (b))
#define SprueMax(a,b)  ((a) > (b) ? (a) : (b))

#define SprueSquare(a) ((a) * (a))
#define SpruePow3(a) ((a) * (a) * (a))
#define SpruePow4(a) ((a) * (a) * (a) * (a))

/// Interpolate arbitrary types supporting the operators.
#define SprueLerp(a, b, t) ((a) + ((b) - (a)) * (t))

/// Compare with epsilon.
#define SprueEquals(lhs, rhs) ((lhs) + EPSILON >= (rhs) && (lhs) - EPSILON <= (rhs))
/// Compare with epsilon.
#define SprueEqualsApprox(lhs, rhs, epsilon) ((lhs) + (epsilon) >= (rhs) && (lhs) - (epsilon) <= (rhs))
/// Compare with epsilon.
#define SprueLess(lhs, rhs) ((lhs < rhs) && (fabs((lhs) - (rhs)) > EPSILON))
/// Compare with epsilon.
#define SprueGreater(lhs, rhs) ((lhs > rhs) && (fabs((lhs) - (rhs)) > EPSILON))

/// Test if a value falls INSIDE a range.
#define SprueBetween(a, m, mm) ((a) > (m) && (a) < (mm))
/// Test if a value falls into a range, including the edges.
#define SprueBetweenInclusive(a, m, mm) ((a) >= (m) && (a) <= (mm))
/// Tests if two pairs of value overlaps (such as time)
#define SprueOverlap(x1, x2, y1, y2) ((x1) <= (y2) && (y1) <= (x2))

#define SprueFloatHashMagic 43758.5453f

template<typename T> T AddReplace(const T& lhs, T& rhs)
{
    T ret = lhs + rhs;
    rhs = lhs;
    return ret;
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

/// Calculate the value of a factorial (eg. 5!)
template <typename T> T factorial(T val) {
    return (val == 1 || val == 0) ? 1 : (factorial(val - 1) * val);
}

// Normalize an arbitrary list of floats
inline void Normalize(float* values, unsigned ct)
{
    float len2 = 0.0f;
    for (unsigned i = 0; i < ct; ++i)
        len2 += (values[i] * values[i]);
    
    float invLen = 1.0f / std::sqrtf(len2);
    for (unsigned i = 0; i < ct; ++i)
        values[i] *= invLen;
}

// As the above but treats subtracts normalized values from 1.0f
// End result is that the smaller values become the larger values
// Important for when a weight is determined by shortest distance
inline void InvertedNormalize(float* values, unsigned ct)
{
    float len2 = 0.0f;
    for (unsigned i = 0; i < ct; ++i)
        len2 += (values[i] * values[i]);

    float invLen = 1.0f / std::sqrtf(len2);
    for (unsigned i = 0; i < ct; ++i)
        values[i] = 1.0f - (values[i] * invLen);
}

SPRUE float Sum(float* values, unsigned ct);

inline float Average(float* values, unsigned ct) { return Sum(values, ct) / (float)ct; }

float NRoot(float* values, unsigned ct);

// From: http://iquilezles.org/www/articles/smin/smin.htm
// exponential smooth min (k = 32);
inline float expSmoothMin(float a, float b, float k = 32.0f)
{
    float res = expf(-k*a) + expf(-k*b);
    return -logf(res) / k;
}

// polynomial smooth min (k = 0.1);
inline float polySmoothMin(float a, float b, float k = 0.1f)
{
    float h = CLAMP(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
    return SprueLerp(b, a, h) - k*h*(1.0f - h);
}

// power smooth min (k = 8);
inline float powSmoothMin(float a, float b, float k = 8.0f)
{
    a = powf(a, k);
    b = powf(b, k);
    return powf((a*b) / (a + b), 1.0f / k);
}

inline float smoothStep(float min, float max, float t)
{
    t = CLAMP((t - min) / (max - min), 0.0f, 1.0f);
    return t*t*(3 - 2 * t);
}

/// Utility function for getting at 2D/3D array indices from coordinates
SPRUE int ToArrayIndex(unsigned x, unsigned y, unsigned z, unsigned width, unsigned height, unsigned depth);

/// Utility function for getting from a 1D array index to 2D/3D coordinates
SPRUE void FromArrayIndex(unsigned index, unsigned& x, unsigned& y, unsigned& z, unsigned width, unsigned height, unsigned depth);

SPRUE int RandomInt(unsigned& seed);

SPRUE float RandomFloat(unsigned& seed);

SPRUE float RandomFloat(unsigned& seed, float min, float max);


}