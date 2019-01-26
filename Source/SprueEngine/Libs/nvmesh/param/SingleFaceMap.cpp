// Copyright NVIDIA Corporation 2008 -- Ignacio Castano <icastano@nvidia.com>

#include "SingleFaceMap.h"

#include "../halfedge/Mesh.h"
#include "../halfedge/Vertex.h"
#include "../halfedge/Face.h"

using namespace nv;



void nv::computeSingleFaceMap(HalfEdge::Mesh * mesh)
{
    //nvDebugCheck(mesh != NULL);
    //nvDebugCheck(mesh->faceCount() == 1);

    HalfEdge::Face * face = mesh->faceAt(0);
    //nvCheck(face != NULL);

    SprueEngine::Vec3 p0 = face->edge->from()->pos;
    SprueEngine::Vec3 p1 = face->edge->to()->pos;

    SprueEngine::Vec3 X = (p1 - p0).Normalized();
    SprueEngine::Vec3 Z = face->normal();
    SprueEngine::Vec3 Y = Z.Cross(X).Normalized();

    uint32_t i = 0;
    for (HalfEdge::Face::EdgeIterator it(face->edges()); !it.isDone(); it.advance(), i++)
    {
        HalfEdge::Vertex * vertex = it.vertex();
        //nvCheck(vertex != NULL);

        if (i == 0)
        {
            vertex->tex = SprueEngine::Vec2(0, 0);
        }
        else
        {
            SprueEngine::Vec3 pn = vertex->pos;

            float xn = (pn - p0).Dot(X);
            float yn = (pn - p0).Dot(Y);

            vertex->tex = SprueEngine::Vec2(xn, yn);
        }
    }
}

