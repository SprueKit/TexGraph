// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#include "QuadTriMesh.h"

using namespace nv;


bool QuadTriMesh::isQuadFace(unsigned i) const 
{ 
    return m_faceArray[i].isQuadFace();
}

const QuadTriMesh::Vertex & QuadTriMesh::faceVertex(unsigned f, unsigned v) const 
{
    if (isQuadFace(f)) SPRUE_ASSERT(v < 4, "Invalid index provided for nv::QuadTriMesh::faceVertex(unsigned, unsigned)");
    else SPRUE_ASSERT(v < 3, "Invalid index provided for nv::QuadTriMesh::faceVertex(unsigned, unsigned)");

    const Face & face = this->faceAt(f);
    return this->vertexAt(face.v[v]);
}
