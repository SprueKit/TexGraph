// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#pragma once
#ifndef NV_MATH_BASIS_H
#define NV_MATH_BASIS_H

#include <SprueEngine/MathGeoLib/AllMath.h>

namespace nv
{

    /// Basis class to compute tangent space basis, ortogonalizations and to
    /// transform vectors from one space to another.
    class Basis
    {
    public:

        /// Create a null basis.
        Basis() : tangent(0, 0, 0), bitangent(0, 0, 0), normal(0, 0, 0) {}

        /// Create a basis given three vectors.
        Basis(SprueEngine::Vec3 n, SprueEngine::Vec3 t, SprueEngine::Vec3 b) : tangent(t), bitangent(b), normal(n) {}

        /// Create a basis with the given tangent vectors and the handness.
        Basis(SprueEngine::Vec3 n, SprueEngine::Vec3 t, float sign)
        {
            build(n, t, sign);
        }

        void normalize(float epsilon = FLT_EPSILON);
        void orthonormalize(float epsilon = FLT_EPSILON);
        void robustOrthonormalize(float epsilon = FLT_EPSILON);
        void buildFrameForDirection(SprueEngine::Vec3 d, float angle = 0);

        /// Calculate the determinant [ F G N ] to obtain the handness of the basis. 
        float handness() const
        {
            return determinant() > 0.0f ? 1.0f : -1.0f;
        }

        /// Build a basis from 2 vectors and a handness flag.
        void build(SprueEngine::Vec3 n, SprueEngine::Vec3 t, float sign)
        {
            normal = n;
            tangent = t;
            bitangent = sign * t.Cross(n);
        }

        /// Compute the determinant of this basis.
        float determinant() const
        {
            return 
                tangent.x * bitangent.y * normal.z - tangent.z * bitangent.y * normal.x +
                tangent.y * bitangent.z * normal.x - tangent.y * bitangent.x * normal.z + 
                tangent.z * bitangent.x * normal.y - tangent.x * bitangent.z * normal.y;
        }

        bool isValid() const;

        // Get transform matrix for this basis.
        SprueEngine::Mat3x4 matrix() const;

        // Transform by this basis. (From this basis to object space).
        SprueEngine::Vec3 transform(SprueEngine::Vec3 v) const;

        // Transform by the transpose. (From object space to this basis).
        SprueEngine::Vec3 transformT(SprueEngine::Vec3 v);

        // Transform by the inverse. (From object space to this basis).
        SprueEngine::Vec3 transformI(SprueEngine::Vec3 v) const;


        SprueEngine::Vec3 tangent;
        SprueEngine::Vec3 bitangent;
        SprueEngine::Vec3 normal;
    };

} // nv namespace

#endif // NV_MATH_BASIS_H
