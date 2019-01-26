#include "DebugRenderer.h"

#include "../../UrhoHelpers.h"

#include <Urho3D/Math/Color.h>

namespace SprueEditor
{

    void DebugRenderer::DrawAxes(const SprueEngine::Mat3x4& transform, const SprueEngine::Vec3& center)
    {
        Urho3D::Vector3 origin = ToUrhoVec(transform * center);
        Urho3D::Vector3 x = ToUrhoVec(transform * (center + SprueEngine::Vec3(1, 0, 0)));
        Urho3D::Vector3 y = ToUrhoVec(transform * (center + SprueEngine::Vec3(0, 1, 0)));
        Urho3D::Vector3 z = ToUrhoVec(transform * (center + SprueEngine::Vec3(0, 0, 1)));
        debug_->AddLine(origin, x, Urho3D::Color::RED, false);
        debug_->AddLine(origin, y, Urho3D::Color::GREEN, false);
        debug_->AddLine(origin, z, Urho3D::Color::BLUE, false);
    }

    void DebugRenderer::DrawBoundingBox(const SprueEngine::Mat3x4& transform, const SprueEngine::BoundingBox& bounds, const SprueEngine::RGBA& color)
    {
        Urho3D::BoundingBox bnds(ToUrhoVec(bounds.minPoint), ToUrhoVec(bounds.maxPoint));
        debug_->AddBoundingBox(bnds, ToUrhoMat3x4(transform), ToUrhoColor(color), false);
    }

    void DebugRenderer::DrawCross(const SprueEngine::Vec3& center, float radius, const SprueEngine::RGBA& color)
    {
        debug_->AddCross(ToUrhoVec(center), radius, ToUrhoColor(color), false);
    }

    void DebugRenderer::DrawLine(const SprueEngine::Vec3& from, const SprueEngine::Vec3& to, const SprueEngine::RGBA& color)
    {
        debug_->AddLine(ToUrhoVec(from), ToUrhoVec(to), ToUrhoColor(color), false);
    }

    void DebugRenderer::DrawSphere(const SprueEngine::Vec3& center, float radius, const SprueEngine::RGBA& color)
    {
        Urho3D::Sphere sphere(ToUrhoVec(center), radius);
        debug_->AddSphere(sphere, ToUrhoColor(color), false);
    }

    void DebugRenderer::DrawDisc(const SprueEngine::Vec3& center, float radius, const SprueEngine::Vec3& normal, const SprueEngine::RGBA& color)
    {
        debug_->AddCircle(ToUrhoVec(center), ToUrhoVec(normal), radius, ToUrhoColor(color), 64, false);
    }
}