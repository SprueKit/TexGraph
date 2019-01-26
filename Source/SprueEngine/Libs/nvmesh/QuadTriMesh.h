// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#pragma once
#ifndef NV_MESH_QUADTRIMESH_H
#define NV_MESH_QUADTRIMESH_H

#include <SprueEngine/Libs/nvmesh/BaseMesh.h>

namespace nv
{
    class Stream;

    /// Mixed quad/triangle mesh.
    class SPRUE QuadTriMesh : public BaseMesh
    {
    public:
        struct Face;
        typedef BaseMesh::Vertex Vertex;

        QuadTriMesh() {};
        QuadTriMesh(unsigned faceCount, unsigned vertexCount) : BaseMesh(vertexCount), m_faceArray(faceCount) {}

        // Face methods.
        unsigned faceCount() const { return m_faceArray.size(); }

        const Face & faceAt(unsigned i) const { return m_faceArray[i]; }
        Face & faceAt(unsigned i) { return m_faceArray[i]; }

        const std::vector<Face> & faces() const { return m_faceArray; }
        std::vector<Face> & faces() { return m_faceArray; }

        bool isQuadFace(unsigned i) const;

        const Vertex & faceVertex(unsigned f, unsigned v) const;

    private:

        std::vector<Face> m_faceArray;

    };


    /// QuadTriMesh face.
    struct QuadTriMesh::Face
    {
        unsigned id;
        unsigned v[4];

        bool isQuadFace() const { return v[3] != -1; }
    };

} // nv namespace


#endif // NV_MESH_QUADTRIMESH_H
