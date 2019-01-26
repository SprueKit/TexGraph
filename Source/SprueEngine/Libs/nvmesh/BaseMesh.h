// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#pragma once
#ifndef NV_MESH_BASEMESH_H
#define NV_MESH_BASEMESH_H

#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/ClassDef.h>
#include <SprueEngine/StringHash.h>

#include <iostream>
#include <vector>

namespace nv
{

    /// Base mesh without connectivity.
    class SPRUE BaseMesh
    {
    public:
        struct Vertex;

        BaseMesh() {}

        BaseMesh(unsigned vertexNum) :
            m_vertexArray(vertexNum) {}

        // Vertex methods.
        unsigned vertexCount() const { return m_vertexArray.size(); }
        const Vertex & vertexAt(unsigned i) const { return m_vertexArray[i]; }
        Vertex & vertexAt(unsigned i) { return m_vertexArray[i]; }
        const std::vector<Vertex> & vertices() const { return m_vertexArray; }
        std::vector<Vertex> & vertices() { return m_vertexArray; }

    protected:
        std::vector<Vertex> m_vertexArray;
    };


    /// BaseMesh vertex.
    struct BaseMesh::Vertex
    {
        Vertex() : id(-1), pos(0.0f), nor(0.0f), tex(0.0f) {}

        unsigned id;		// @@ Vertex should be an index into the vertex data.
        SprueEngine::Vec3 pos;
        SprueEngine::Vec3 nor;
        SprueEngine::Vec2 tex;

        unsigned hash()
        {
            return SprueEngine::StringHash::fnv1a(&pos.x, sizeof(SprueEngine::Vec3));
        }
    };

    inline bool operator==(const BaseMesh::Vertex & a, const BaseMesh::Vertex & b)
    {
        return a.pos == b.pos && a.nor == b.nor && a.tex == b.tex;
    }

    inline bool operator!=(const BaseMesh::Vertex & a, const BaseMesh::Vertex & b)
    {
        return !a.pos.Equals(b.pos) && !a.nor.Equals(b.nor) && !a.tex.Equals(b.tex);
    }

} // nv namespace

#endif // NV_MESH_BASEMESH_H
