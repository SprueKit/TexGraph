#pragma once

#include <SprueEngine/Math/Color.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{

    class IDebugRender
    {
    public:
        virtual void DrawAxes(const Mat3x4& transform, const Vec3& center) = 0;
        virtual void DrawBoundingBox(const Mat3x4& transform, const BoundingBox& bounds, const RGBA& color) = 0;
        virtual void DrawCross(const Vec3& center, float radius, const RGBA& color) = 0;
        virtual void DrawLine(const Vec3& from, const Vec3& to, const RGBA& color) = 0;
        virtual void DrawSphere(const Vec3& center, float radius, const RGBA& color) = 0;
        virtual void DrawDisc(const Vec3& center, float radius, const Vec3& normal, const SprueEngine::RGBA& color) = 0;

        /// Refer to Bullet Physics' btDebugDraw.h for an example of implementing this
        void DrawSpherePatch(const Mat3x4& transform, const Vec3& center, const Vec3& up, const Vec3& axis, float radius, float minTheta, float maxTheta, float minPhi, float maxPhi, const RGBA& color);
    };

}