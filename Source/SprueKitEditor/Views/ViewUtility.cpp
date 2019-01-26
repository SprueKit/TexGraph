#include "ViewUtility.h"

#include <SprueEngine/Math/MathDef.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

namespace SprueEditor
{

unsigned gridSize = 16;
unsigned gridSubdivisions = 3;
float gridScale = 8.0f;
Color gridColor(0.1f, 0.1f, 0.1f);
Color gridSubdivisionColor(0.3f, 0.3f, 0.3f);
Color gridXColor(0.7f, 0.1f, 0.1f);
Color gridYColor(0.1f, 0.7f, 0.1f);
Color gridZColor(0.1f, 0.1f, 0.7f);

void CreateGrid(Scene* scene, int gridSize, const Vector3& gridScale, bool is2D)
{
    Node* gridNode = 0x0;
    if (!(gridNode = scene->GetChild("GRID")))
    {
        gridNode = scene->CreateChild("GRID");

        CustomGeometry* grid = gridNode->GetOrCreateComponent<CustomGeometry>();
        grid->SetNumGeometries(1);
        grid->SetMaterial(scene->GetContext()->GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/VColUnlit.xml"));
        grid->SetViewMask(0x80000000);
        grid->SetOccludee(false);
    }

    CustomGeometry* grid = gridNode->GetComponent<CustomGeometry>();
    gridNode->SetScale(gridScale);

    unsigned size = unsigned(floorf(gridSize / 2) * 2);
    float halfSizeScaled = size / 2;
    float scale = 1.0;
    unsigned subdivisionSize = unsigned(powf(2.0f, float(gridSubdivisions)));

    if (subdivisionSize > 0)
    {
        size *= subdivisionSize;
        scale /= subdivisionSize;
    }

    unsigned halfSize = size / 2;

    grid->BeginGeometry(0, LINE_LIST);
    float lineOffset = -halfSizeScaled;
    for (unsigned i = 0; i <= size; ++i)
    {
        bool lineCenter = i == halfSize;
        bool lineSubdiv = !SprueEquals(fmodf(i, subdivisionSize), 0.0);

        if (!is2D)
        {
            grid->DefineVertex(Vector3(lineOffset, 0.0, halfSizeScaled));
            grid->DefineColor(lineCenter ? gridZColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
            grid->DefineVertex(Vector3(lineOffset, 0.0, -halfSizeScaled));
            grid->DefineColor(lineCenter ? gridZColor : (lineSubdiv ? gridSubdivisionColor : gridColor));

            grid->DefineVertex(Vector3(-halfSizeScaled, 0.0, lineOffset));
            grid->DefineColor(lineCenter ? gridXColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
            grid->DefineVertex(Vector3(halfSizeScaled, 0.0, lineOffset));
            grid->DefineColor(lineCenter ? gridXColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
        }
        else
        {
            grid->DefineVertex(Vector3(lineOffset, halfSizeScaled, 0.0));
            grid->DefineColor(lineCenter ? gridYColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
            grid->DefineVertex(Vector3(lineOffset, -halfSizeScaled, 0.0));
            grid->DefineColor(lineCenter ? gridYColor : (lineSubdiv ? gridSubdivisionColor : gridColor));

            grid->DefineVertex(Vector3(-halfSizeScaled, lineOffset, 0.0));
            grid->DefineColor(lineCenter ? gridXColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
            grid->DefineVertex(Vector3(halfSizeScaled, lineOffset, 0.0));
            grid->DefineColor(lineCenter ? gridXColor : (lineSubdiv ? gridSubdivisionColor : gridColor));
        }

        lineOffset += scale;
    }

    float quarterSize = halfSizeScaled / 2;
    float eightSize = halfSizeScaled / 4;

    // Draw X axis forward
    grid->DefineVertex(Vector3(halfSizeScaled, 0.01f, 0));
    grid->DefineColor(gridXColor);
    grid->DefineVertex(Vector3(quarterSize, 0.01f, eightSize));
    grid->DefineColor(gridXColor);

    grid->DefineVertex(Vector3(halfSizeScaled, 0.01f, 0));
    grid->DefineColor(gridXColor);
    grid->DefineVertex(Vector3(quarterSize, 0.01f, -eightSize));
    grid->DefineColor(gridXColor);

    // Draw Z axis forward
    grid->DefineVertex(Vector3(0, 0.01f, halfSizeScaled));
    grid->DefineColor(gridZColor);
    grid->DefineVertex(Vector3(eightSize, 0.01f, quarterSize));
    grid->DefineColor(gridZColor);

    grid->DefineVertex(Vector3(0, 0.01f, halfSizeScaled));
    grid->DefineColor(gridZColor);
    grid->DefineVertex(Vector3(-eightSize, 0.01f, quarterSize));
    grid->DefineColor(gridZColor);
    grid->Commit();
}

void CreateBounds(Scene* scene, const BoundingBox& bounds, const Matrix3x4& transform)
{
    Node* boundsNode = 0x0;
    if (!(boundsNode = scene->GetChild("BOUNDS")))
    {
        boundsNode = scene->CreateChild("BOUNDS");
        CustomGeometry* geom = boundsNode->GetOrCreateComponent<CustomGeometry>();
        geom->SetNumGeometries(1);
        geom->SetMaterial(scene->GetContext()->GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/VColUnlit.xml"));
        geom->SetViewMask(0x80000000);
        geom->SetOccludee(false);
    }

    boundsNode->SetEnabled(true);

    CustomGeometry* geom = boundsNode->GetComponent<CustomGeometry>();

    const Vector3& min = bounds.min_;
    const Vector3& max = bounds.max_;

    Vector3 v0(transform * min);
    Vector3 v1(transform * Vector3(max.x_, min.y_, min.z_));
    Vector3 v2(transform * Vector3(max.x_, max.y_, min.z_));
    Vector3 v3(transform * Vector3(min.x_, max.y_, min.z_));
    Vector3 v4(transform * Vector3(min.x_, min.y_, max.z_));
    Vector3 v5(transform * Vector3(max.x_, min.y_, max.z_));
    Vector3 v6(transform * Vector3(min.x_, max.y_, max.z_));
    Vector3 v7(transform * max);
    
    geom->BeginGeometry(0, LINE_LIST);

    geom->DefineVertex(v0); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v1); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v1); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v2); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v2); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v3); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v3); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v0); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v4); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v5); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v5); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v7); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v7); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v6); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v6); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v4); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v0); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v4); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v1); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v5); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v2); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v7); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v3); geom->DefineColor(Color::GREEN);
    geom->DefineVertex(v6); geom->DefineColor(Color::GREEN);
    

    geom->Commit();
}

}