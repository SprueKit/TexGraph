#pragma once

#include <SprueEngine/Math/Color.h>
#include <SprueEngine/IDebugRender.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <Urho3D/Graphics/DebugRenderer.h>

namespace SprueEditor
{

    class DebugRenderer : public SprueEngine::IDebugRender
    {
    public:
        DebugRenderer(Urho3D::DebugRenderer* renderer) : debug_(renderer) {

        }

        virtual void DrawAxes(const SprueEngine::Mat3x4& transform, const SprueEngine::Vec3& center) override;
        virtual void DrawBoundingBox(const SprueEngine::Mat3x4& transform, const SprueEngine::BoundingBox& bounds, const SprueEngine::RGBA& color) override;
        virtual void DrawCross(const SprueEngine::Vec3& center, float radius, const SprueEngine::RGBA& color) override;
        virtual void DrawLine(const SprueEngine::Vec3& from, const SprueEngine::Vec3& to, const SprueEngine::RGBA& color) override;
        virtual void DrawSphere(const SprueEngine::Vec3& center, float radius, const SprueEngine::RGBA& color) override;
        virtual void DrawDisc(const SprueEngine::Vec3& center, float radius, const SprueEngine::Vec3& normal, const SprueEngine::RGBA& color) override;

    private:
        Urho3D::DebugRenderer* debug_;
    };

}