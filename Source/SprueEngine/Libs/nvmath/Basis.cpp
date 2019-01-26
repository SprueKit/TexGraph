// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#include "Basis.h"

#include <SprueEngine/Math/MathDef.h>

using namespace nv;


/// Normalize basis vectors.
void Basis::normalize(float epsilon /*= NV_EPSILON*/)
{
    normal = normal.Normalized();
    tangent = tangent.Normalized();
    bitangent = bitangent.Normalized();
}


/// Gram-Schmidt orthogonalization.
/// @note Works only if the vectors are close to orthogonal.
void Basis::orthonormalize(float epsilon /*= NV_EPSILON*/)
{
    // N' = |N|
    // T' = |T - (N' dot T) N'|
    // B' = |B - (N' dot B) N' - (T' dot B) T'|

    normal = normal.Normalized();

    tangent -= normal * normal.Dot(tangent);
    tangent = tangent.Normalized();

    bitangent -= normal * normal.Dot(bitangent);
    bitangent -= tangent * tangent.Dot(bitangent);
    bitangent = bitangent.Normalized();
}




/// Robust orthonormalization. 
/// Returns an orthonormal basis even when the original is degenerate.
void Basis::robustOrthonormalize(float epsilon /*= NV_EPSILON*/)
{
    // Normalize all vectors.
    normalize(epsilon);

    if (normal.LengthSq() < epsilon*epsilon)
    {
        // Build normal from tangent and bitangent.
        normal = tangent.Cross(bitangent);

        if (normal.LengthSq() < epsilon*epsilon)
        {
            // Arbitrary basis.
            tangent   = SprueEngine::Vec3(1, 0, 0);
            bitangent = SprueEngine::Vec3(0, 1, 0);
            normal    = SprueEngine::Vec3(0, 0, 1);
            return;
        }

        normal = normal.Normalized();
    }

    // Project tangents to normal plane.
    tangent -= normal * normal.Dot(tangent);
    bitangent -= normal * normal.Dot(bitangent);

    if (tangent.LengthSq() < epsilon*epsilon)
    {
        if (bitangent.LengthSq() < epsilon*epsilon)
        {
            // Arbitrary basis.
            buildFrameForDirection(normal);
        }
        else
        {
            // Build tangent from bitangent.
            bitangent = bitangent.Normalized();

            tangent = bitangent.Cross(normal);
            ////nvDebugCheck(isNormalized(tangent, epsilon));
        }
    }
    else
    {
        tangent = tangent.Normalized();
#if 0
        bitangent -= tangent * dot(tangent, bitangent);

        if (lengthSquared(bitangent) < epsilon*epsilon)
        {
            bitangent = cross(tangent, normal);
            //nvDebugCheck(isNormalized(bitangent, epsilon));
        }
        else
        {
            bitangent = nv::normalize(bitangent, epsilon);
        }
#else
        if (bitangent.LengthSq() < epsilon*epsilon)
        {
            // Build bitangent from tangent.
            bitangent = tangent.Cross(normal);
            ////nvDebugCheck(isNormalized(bitangent, epsilon));
        }
        else
        {
            bitangent = bitangent.Normalized();

            // At this point tangent and bitangent are orthogonal to normal, but we don't know whether their orientation.
            
            SprueEngine::Vec3 bisector;
            if ((tangent + bitangent).LengthSq() < epsilon*epsilon)
            {
                bisector = tangent;
            }
            else
            {
                bisector = (tangent + bitangent).Normalized();
            }
            SprueEngine::Vec3 axis = bisector.Cross(normal);

            ////nvDebugCheck(isNormalized(axis, epsilon));
            //nvDebugCheck(equal(dot(axis, tangent), -dot(axis, bitangent), epsilon));

            if (axis.Dot(tangent) > 0)
            {
                tangent = bisector + axis;
                bitangent = bisector - axis;
            }
            else
            {
                tangent = bisector - axis;
                bitangent = bisector + axis;
            }

            // Make sure the resulting tangents are still perpendicular to the normal.
            tangent -= normal * normal.Dot(tangent);
            bitangent -= normal * normal.Dot(bitangent);

            // Double check.
            //nvDebugCheck(equal(dot(normal, tangent), 0.0f, epsilon));
            //nvDebugCheck(equal(dot(normal, bitangent), 0.0f, epsilon));

            // Normalize.
            tangent = tangent.Normalized();
            bitangent = bitangent.Normalized();

            // If tangent and bitangent are not orthogonal, then derive bitangent from tangent, just in case...
            if (!SprueEquals(tangent.Dot(bitangent), 0.0f)) {
                bitangent = tangent.Cross(normal);
                bitangent = bitangent.Normalized();
            }
        }
#endif
    }

    /*// Check vector lengths.
    if (!isNormalized(normal, epsilon))
    {
    //nvDebug("%f %f %f\n", normal.x, normal.y, normal.z);
    //nvDebug("%f %f %f\n", tangent.x, tangent.y, tangent.z);
    //nvDebug("%f %f %f\n", bitangent.x, bitangent.y, bitangent.z);
    }*/

    //nvDebugCheck(isNormalized(normal, epsilon));
    //nvDebugCheck(isNormalized(tangent, epsilon));
    //nvDebugCheck(isNormalized(bitangent, epsilon));

    // Check vector angles.
    //nvDebugCheck(equal(dot(normal, tangent), 0.0f, epsilon));
    //nvDebugCheck(equal(dot(normal, bitangent), 0.0f, epsilon));
    //nvDebugCheck(equal(dot(tangent, bitangent), 0.0f, epsilon));

    // Check vector orientation.
    const float det = normal.Cross(tangent).Dot(bitangent);
    //nvDebugCheck(equal(det, 1.0f, epsilon) || equal(det, -1.0f, epsilon));
}


/// Build an arbitrary frame for the given direction.
void Basis::buildFrameForDirection(SprueEngine::Vec3 d, float angle/*= 0*/)
{
    //nvCheck(isNormalized(d));
    normal = d;

    // Choose minimum axis.
    if (fabsf(normal.x) < fabsf(normal.y) && fabsf(normal.x) < fabsf(normal.z))
    {
        tangent = SprueEngine::Vec3(1, 0, 0);
    }
    else if (fabsf(normal.y) < fabsf(normal.z))
    {
        tangent = SprueEngine::Vec3(0, 1, 0);
    }
    else
    {
        tangent = SprueEngine::Vec3(0, 0, 1);
    }

    // Ortogonalize
    tangent -= normal * normal.Dot(tangent);
    tangent = tangent.Normalized();

    bitangent = normal.Cross(tangent);

    // Rotate frame around normal according to angle.
    if (angle != 0.0f) {
        float c = cosf(angle);
        float s = sinf(angle);
        SprueEngine::Vec3 tmp = c * tangent - s * bitangent;
        bitangent = s * tangent + c * bitangent;
        tangent = tmp;
    }
}

bool Basis::isValid() const
{
    if (normal.IsZero()) return false;
    if (tangent.IsZero()) return false;
    if (bitangent.IsZero()) return false;

    if (SprueEquals(determinant(), 0.0f)) return false;

    return true;
}


/// Transform by this basis. (From this basis to object space).
SprueEngine::Vec3 Basis::transform(SprueEngine::Vec3 v) const
{
    SprueEngine::Vec3 o = tangent * v.x;
    o += bitangent * v.y;
    o += normal * v.z;
    return o;
}

/// Transform by the transpose. (From object space to this basis).
SprueEngine::Vec3 Basis::transformT(SprueEngine::Vec3 v)
{
    return SprueEngine::Vec3(tangent.Dot(v), bitangent.Dot(v), normal.Dot(v));
}

/// Transform by the inverse. (From object space to this basis).
/// @note Uses Cramer's rule so the inverse is not accurate if the basis is ill-conditioned.
SprueEngine::Vec3 Basis::transformI(SprueEngine::Vec3 v) const
{
    const float det = determinant();
    //nvDebugCheck(!equal(det, 0.0f, 0.0f));

    const float idet = 1.0f / det;

    // Rows of the inverse matrix.
    SprueEngine::Vec3 r0(
        (bitangent.y * normal.z - bitangent.z * normal.y),
        -(bitangent.x * normal.z - bitangent.z * normal.x),
        (bitangent.x * normal.y - bitangent.y * normal.x));

    SprueEngine::Vec3 r1(
        -(tangent.y * normal.z - tangent.z * normal.y),
        (tangent.x * normal.z - tangent.z * normal.x),
        -(tangent.x * normal.y - tangent.y * normal.x));

    SprueEngine::Vec3 r2(
        (tangent.y * bitangent.z - tangent.z * bitangent.y),
        -(tangent.x * bitangent.z - tangent.z * bitangent.x),
        (tangent.x * bitangent.y - tangent.y * bitangent.x));

    return SprueEngine::Vec3(v.Dot(r0), v.Dot(r1), v.Dot(r2)) * idet;
}


