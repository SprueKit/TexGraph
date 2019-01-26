#include "Triangle.h"

#include <SprueEngine/Math/MathDef.h>

namespace SprueEngine
{

float point_segment_distance(const Vec3 &x0, const Vec3 &x1, const Vec3 &x2)
{
    Vec3 dx(x2 - x1);
    float m2 = dx.LengthSq();
    // find parameter value of closest point on segment
    float s12 = (float)((x2 - x0).Dot(dx) / m2);
    if (s12 < 0)
        s12 = 0;
    else if (s12 > 1)
        s12 = 1;
    // and find the distance
    return (x0 - (s12 * x1 + (1 - s12) * x2)).LengthSq();
}


BoundingBox Triangle::GetBounds() const
{
    const Vec3 min = Vec3::MinVector(a, Vec3::MinVector(b, c));
    const Vec3 max = Vec3::MaxVector(a, Vec3::MaxVector(b, c));
    return BoundingBox(min, max);
}

Vec3 Triangle::ClosestPoint(const Vec3 &p) const
{
    /** The code for Triangle-float3 test is from Christer Ericson's Real-Time Collision Detection, pp. 141-142. */

    // Check if P is in vertex region outside A.
    Vec3 ab = b - a;
    Vec3 ac = c - a;
    Vec3 ap = p - a;
    float d1 = ab.Dot(ap);
    float d2 = ac.Dot(ap);
    if (d1 <= 0.f && d2 <= 0.f)
        return a; // Barycentric coordinates are (1,0,0).

    // Check if P is in vertex region outside B.
    Vec3 bp = p - b;
    float d3 = ab.Dot(bp);
    float d4 = ac.Dot(bp);
    if (d3 >= 0.f && d4 <= d3)
        return b; // Barycentric coordinates are (0,1,0).

    // Check if P is in edge region of AB, and if so, return the projection of P onto AB.
    float vc = d1*d4 - d3*d2;
    if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
    {
        float v = d1 / (d1 - d3);
        return a + v * ab; // The barycentric coordinates are (1-v, v, 0).
    }

    // Check if P is in vertex region outside C.
    Vec3 cp = p - c;
    float d5 = ab.Dot(cp);
    float d6 = ac.Dot(cp);
    if (d6 >= 0.f && d5 <= d6)
        return c; // The barycentric coordinates are (0,0,1).

    // Check if P is in edge region of AC, and if so, return the projection of P onto AC.
    float vb = d5*d2 - d1*d6;
    if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
    {
        float w = d2 / (d2 - d6);
        return a + w * ac; // The barycentric coordinates are (1-w, 0, w).
    }

    // Check if P is in edge region of BC, and if so, return the projection of P onto BC.
    float va = d3*d6 - d5*d4;
    if (va <= 0.f && d4 - d3 >= 0.f && d5 - d6 >= 0.f)
    {
        float w = (d4 - d3) / (d4 - d3 + d5 - d6);
        return b + w * (c - b); // The barycentric coordinates are (0, 1-w, w).
    }

    // P must be inside the face region. Compute the closest point through its barycentric coordinates (u,v,w).
    float denom = 1.f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

bool Triangle::IntersectRay(const Ray& ray, float* dist, Vec3* hitPos, Vec3* outBary) const
{
    Vec3 e1(b - a);
    Vec3 e2(c - a);
    Vec3 normal = e1.Cross(e2);
    if (normal.Normalized().Dot(ray.dir) > 0.0f)
        return false;

    Vec3 p(ray.dir.Cross(e2));
    float det = e1.Dot(p);
    if (det < EPSILON)
        return false;

    Vec3 t(ray.pos - a);
    float u = t.Dot(p);
    if (u < 0.0f || u > det)
        return false;

    Vec3 q(t.Cross(e1));
    float v = ray.dir.Dot(q);
    if (v < 0.0f || u + v > det)
        return false;

    float distance = e2.Dot(q) / det;
    if (distance >= 0.0f)
    {
        if (outBary)
        {
            *outBary = Vec3(1 - (u / det) - (v / det), u / det, v / det);
            if (hitPos)
                *hitPos = a * outBary->x + b * outBary->y + c * outBary->z;
        }
        if (dist)
            *dist = distance;
        return true;
    }
    return false;
}

bool Triangle::IntersectRayEitherSide(const Ray& ray, float* dist, Vec3* hitPos, Vec3* outBary) const
{
    Vec3 e1(b - a);
    Vec3 e2(c - a);

    Vec3 normal = e1.Cross(e2);
    if (normal.Normalized().Dot(ray.dir) > 0.0f)
    {
        Flip();
        e1 = (b - a);
        e2 = (c - a);
        //return false;
    }

    Vec3 p(ray.dir.Cross(e2));
    float det = e1.Dot(p);
    if (det < EPSILON)
        return false;

    Vec3 t(ray.pos - a);
    float u = t.Dot(p);
    if (u < 0.0f || u > det)
        return false;

    Vec3 q(t.Cross(e1));
    float v = ray.dir.Dot(q);
    if (v < 0.0f || u + v > det)
        return false;

    float distance = e2.Dot(q) / det;
    if (distance >= 0.0f)
    {
        if (outBary)
        {
            *outBary = Vec3(1 - (u / det) - (v / det), u / det, v / det);
            if (hitPos)
                *hitPos = a * outBary->x + b * outBary->y + c * outBary->z;
        }
        if (dist)
            *dist = distance;
        return true;
    }
    return false;
}

float Triangle::Distance(const Vec3& p) const
{
    return sqrtf(Distance2(p));
}

float Triangle::Distance2(const Vec3& p) const
{
    Vec3 x13(a - c);
    Vec3 x23(b - c);
    Vec3 x03(p - c);
    const float m13 = x13.LengthSq();
    const float m23 = x23.LengthSq();
    const float d = x13.Dot(x23);
    const float invdet = 1.0f / SprueMax(m13 * m23 - d * d, 1e-30f);
    const float a = x13.Dot(x03);
    const float b = x23.Dot(x03);
    // the barycentric coordinates themselves
    float w23 = invdet * (m23 * a - d * b);
    float w31 = invdet * (m13 * b - d * a);
    float w12 = 1 - w23 - w31;

    if (w23 >= 0 && w31 >= 0 && w12 >= 0)
    { // if we're inside the triangle
        return (p - (this->a * w23 + this->b * w31 + this->c*w12)).LengthSq();
    }
    else
    { // we have to clamp to one of the edges
        if (w23 > 0) // this rules out edge 2-3 for us
            return SprueMin(point_segment_distance(p, this->a, this->b), point_segment_distance(p, this->a, this->c));
        else if (w31>0) // this rules out edge 1-3
            return SprueMin(point_segment_distance(p, this->a, this->b), point_segment_distance(p, this->b, this->c));
        else // w12 must be >0, ruling out edge 1-2
            return SprueMin(point_segment_distance(p, this->a, this->c), point_segment_distance(p, this->b, this->c));
    }
}

float Triangle::SignedDistance(const Vec3& p) const
{
    float dist = Distance(p);
    const Vec3 center = (a + b + c) / 3.0f;
    const float sign = (p - c).Normalized().Dot(GetNormal()) > 0.0f ? 1.0f : -1.0f;
    return dist * sign;
}

float Triangle::SignDistance(const Vec3& p, const float inputDist) const
{
    const Vec3 center = (a + b + c) / 3.0f;
    const float sign = (p - c).Normalized().Dot(GetNormal()) > 0.0f ? 1.0f : -1.0f;
    return inputDist * sign;
}

}