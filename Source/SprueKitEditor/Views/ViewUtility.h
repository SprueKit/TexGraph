#pragma once

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Matrix3x4.h>

namespace Urho3D
{
    class Scene;
    class BoundingBox;
}

namespace SprueEditor
{

void CreateGrid(Urho3D::Scene* scene, int gridSize, const Urho3D::Vector3& scale, bool is2D);

/// Used to create a stored geometry for a bounding box. Prevents flickering during mouse movement.
void CreateBounds(Urho3D::Scene* scene, const Urho3D::BoundingBox& bounds, const Urho3D::Matrix3x4& transform);

}