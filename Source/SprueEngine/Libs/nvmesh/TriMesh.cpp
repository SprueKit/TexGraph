// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#include "TriMesh.h"

using namespace nv;


/// Triangle mesh.
SprueEngine::Vec3 TriMesh::faceNormal(unsigned f) const
{
    const Face & face = this->faceAt(f);
    const SprueEngine::Vec3 & p0 = this->vertexAt(face.v[0]).pos;
    const SprueEngine::Vec3 & p1 = this->vertexAt(face.v[1]).pos;
    const SprueEngine::Vec3 & p2 = this->vertexAt(face.v[2]).pos;
    return (p1 - p0).Cross(p2 - p0).Normalized();
}

/// Get face vertex.
const TriMesh::Vertex & TriMesh::faceVertex(unsigned f, unsigned v) const
{
    SPRUE_ASSERT(v < 3, "Invalid vertex index for nv::TriMesh::faceVertex(unsigned, unsigned)");
    const Face & face = this->faceAt(f);
    return this->vertexAt(face.v[v]);
}

