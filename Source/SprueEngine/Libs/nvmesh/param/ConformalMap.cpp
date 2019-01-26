// This code is in the public domain -- castano@gmail.com

#include "ConformalMap.h"
#include "Util.h"

#include "../../nvmath/Sparse.h"
#include "../../nvmath/Solver.h"

#include "../halfedge/Mesh.h"
#include "../halfedge/Vertex.h"
#include "../halfedge/Face.h"
#include "../MeshTopology.h"

using namespace nv;

namespace {

#define square(VALUE) ((VALUE) * (VALUE))

    static float triangleArea(SprueEngine::Vec3 a, SprueEngine::Vec3 b)
    {
        return 0.5f * sqrtf(a.LengthSq() * b.LengthSq() - square(a.Dot(b)));
    }


    static float computeHarmonicCoordinate(const HalfEdge::Edge * edge)
    {
        // Sum of the cotangents of the opposite angles.
        // w_ij = cot theta_ij + cot theta_ji

        //    R 
        //   / \
        //  /   \
        // P --- Q
        //  \   /
        //   \ /
        //    S 

        const SprueEngine::Vec3 P = edge->from()->pos;
        const SprueEngine::Vec3 Q = edge->to()->pos;
        const SprueEngine::Vec3 R = edge->next->to()->pos;
        const SprueEngine::Vec3 S = edge->pair->next->to()->pos;

        const SprueEngine::Vec3 RP = P - R;
        const SprueEngine::Vec3 RQ = Q - R;
        const SprueEngine::Vec3 SP = P - S;
        const SprueEngine::Vec3 SQ = Q - S;

        float cotR = RP.Dot(RQ) / triangleArea(RP, RQ);
        float cotS = SP.Dot(SQ) / triangleArea(SP, SQ);

        //float cotR = 0.5f * (lengthSquared(RP) + lengthSquared(RQ) - lengthSquared(PQ)) / triangleArea(RP, RQ);
        //float cotS = 0.5f * (lengthSquared(SP) + lengthSquared(SQ) - lengthSquared(SQ)) / triangleArea(SP, SQ);

        return cotR + cotS;
    }

    static SprueEngine::Vec2 computeMeanValueCoordinate(const HalfEdge::Edge * edge)
    {
        const SprueEngine::Vec3 P = edge->from()->pos;
        const SprueEngine::Vec3 Q = edge->to()->pos;
        const SprueEngine::Vec3 R = edge->next->to()->pos;
        const SprueEngine::Vec3 S = edge->pair->next->to()->pos;

        const SprueEngine::Vec3 QR = R - Q;
        const SprueEngine::Vec3 QS = S - Q;
        const SprueEngine::Vec3 QP = P - Q;

        const SprueEngine::Vec3 PR = R - P;
        const SprueEngine::Vec3 PS = S - P;
        const SprueEngine::Vec3 PQ = Q - P;

        float tanS0 = (PQ.Length() * PS.Length() - PQ.Dot(PS)) / triangleArea(PQ, PS);
        float tanR0 = (PQ.Length() * PR.Length() - PQ.Dot(PR)) / triangleArea(PQ, PR);

        float tanS1 = (QP.Length() * QS.Length() - QP.Dot(QS)) / triangleArea(QP, QS);
        float tanR1 = (QP.Length() * QR.Length() - QP.Dot(QR)) / triangleArea(QP, QR);

        float mv1 = (tanS0 + tanR0) / PQ.Length();
        float mv0 = (tanS1 + tanR1) / PQ.Length();

        return SprueEngine::Vec2(mv0, mv1);
    }

    static SprueEngine::Vec2 computeWachpressCoordinate(const HalfEdge::Edge * edge)
    {
        const SprueEngine::Vec3 P = edge->from()->pos;
        const SprueEngine::Vec3 Q = edge->to()->pos;
        const SprueEngine::Vec3 R = edge->next->to()->pos;
        const SprueEngine::Vec3 S = edge->pair->next->to()->pos;

        const SprueEngine::Vec3 QR = R - Q;
        const SprueEngine::Vec3 QS = S - Q;
        const SprueEngine::Vec3 QP = P - Q;

        const SprueEngine::Vec3 PR = R - P;
        const SprueEngine::Vec3 PS = S - P;
        const SprueEngine::Vec3 PQ = Q - P;

        float PRQ = triangleArea(PQ, PR);
        float PSQ = triangleArea(PQ, PS);

        float QRS = triangleArea(QS, QR);
        float PSR = triangleArea(PR, PS);

        float chi0 = PSR / (PSQ * PRQ);
        float chi1 = QRS / (PSQ * PRQ);

        return SprueEngine::Vec2(chi0, chi1);
    }

    enum Coordinate {
        Coordinate_Harmonic,
        Coordinate_MeanValue,
        Coordinate_Wachpress,
    };

} // namespace




bool nv::computeConformalMap(HalfEdge::Mesh * mesh)
{
    // @@ computeBarycentricMap(mesh, Coordinate_Harmonic);

    //nvDebugCheck(mesh != NULL);
    //nvDebugCheck(mesh->faceCount() > 1);

#if _DEBUG
    //nvDebugCheck(isTriangularMesh(mesh));

    MeshTopology topology(mesh);
    //nvDebugCheck(topology.isDisk());
#endif

    const uint32_t vertexCount = mesh->vertexCount();

    // Two equations: Ab=x, Ac=y
    SparseMatrix A(vertexCount, vertexCount);
    FullVector b(vertexCount);
    FullVector c(vertexCount);
    FullVector x(vertexCount);
    FullVector y(vertexCount);

    // Set right hand side to boundary parameterization or zero.
    for (uint32_t v = 0; v < vertexCount; v++)
    {
        HalfEdge::Vertex * vertex = mesh->vertexAt(v);

        if (vertex->isBoundary()) {
            x[v] = vertex->tex.x;
            y[v] = vertex->tex.y;
        }
        else {
            x[v] = 0;
            y[v] = 0;
        }
    }

    // Set initial solution. @@ It may make more sense to set this to the input parameterization.
    b = x;
    c = y;

    // Fill matrix.
    for (uint32_t v = 0; v < vertexCount; v++)
    {
        HalfEdge::Vertex * vertex = mesh->vertexAt(v);

        if (vertex->isBoundary()) {
            A.setCoefficient(v, v, 1.0f);
        }
        else {
            float sum = 0.0f;

            // Traverse edges around vertex.
            for (HalfEdge::Vertex::ConstEdgeIterator it(vertex->edges()); !it.isDone(); it.advance())
            {
                const HalfEdge::Edge * edge = it.current();

                const float energy = computeHarmonicCoordinate(edge);

                A.setCoefficient(v, edge->to()->id, energy);
                sum += energy;
            }

            // Set diagonal.
            A.setCoefficient(v, v, -sum);
        }
    }

    // Solve equations.
    SymmetricSolver(A, b, x);
    SymmetricSolver(A, c, y);

    // Set texture coordinates.
    for (uint32_t v = 0; v < vertexCount; v++)
    {
        HalfEdge::Vertex * vertex = mesh->vertexAt(v);
        vertex->tex.x = x[v];
        vertex->tex.y = y[v];
    }

    return true;
}
