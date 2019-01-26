// This code is in the public domain -- castano@gmail.com

#pragma once

#include <SprueEngine/MathGeoLib/AllMath.h>

#include <stdint.h>

namespace nv {

    namespace HalfEdge { class Mesh; class Vertex; }

    bool isQuadMesh(const HalfEdge::Mesh * mesh);
    bool isTriangularMesh(const HalfEdge::Mesh * mesh);

    uint32_t countMeshTriangles(const HalfEdge::Mesh * mesh);
    const HalfEdge::Vertex * findBoundaryVertex(const HalfEdge::Mesh * mesh);

    HalfEdge::Mesh * unifyVertices(const HalfEdge::Mesh * inputMesh);
    HalfEdge::Mesh * triangulate(const HalfEdge::Mesh * inputMesh);

    float triangleArea(const SprueEngine::Vec2& v0, const SprueEngine::Vec2& v1, const SprueEngine::Vec2& v2);
    float triangleArea(const SprueEngine::Vec2& v0, const SprueEngine::Vec2& v1);

} // nv namespace
