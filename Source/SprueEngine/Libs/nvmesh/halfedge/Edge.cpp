// This code is in the public domain -- castanyo@yahoo.es

#include "Edge.h"
#include "Vertex.h"

using namespace nv;
using namespace HalfEdge;

SprueEngine::Vec3 Edge::midPoint() const
{
    return (to()->pos + from()->pos) * 0.5f;
}

float Edge::length() const
{
    return (to()->pos - from()->pos).Length(); 
}

// Return angle between this edge and the previous one.
float Edge::angle() const {
    SprueEngine::Vec3 p = vertex->pos;
    SprueEngine::Vec3 a = prev->vertex->pos;
    SprueEngine::Vec3 b = next->vertex->pos;

    SprueEngine::Vec3 v0 = a - p;
    SprueEngine::Vec3 v1 = b - p;

    return acosf(v0.Dot(v1) / (v0.Length() * v1.Length()));
}

bool Edge::isValid() const
{
    // null face is OK.
    if (next == nullptr || prev == nullptr || pair == nullptr || vertex == nullptr) return false;
    if (next->prev != this) return false;
    if (prev->next != this) return false;
    if (pair->pair != this) return false;
    return true;
}

float Edge::cotangentWeight() const
{
    const SprueEngine::Vec3 P = from()->pos;
    const SprueEngine::Vec3 Q = to()->pos;
    const SprueEngine::Vec3 R = next->to()->pos;
    const SprueEngine::Vec3 S = pair->next->to()->pos;

    const SprueEngine::Vec3 RP = P - R;
    const SprueEngine::Vec3 RQ = Q - R;
    const SprueEngine::Vec3 SP = P - S;
    const SprueEngine::Vec3 SQ = Q - S;

#define TRIANGLE_AREA(A, B, D) (0.5f * sqrtf(A.LengthSq() * B.LengthSq() - (D * D)))

    const float dotA = RP.Dot(RQ);
    const float dotB = SP.Dot(SQ);

    float cotR = dotA / TRIANGLE_AREA(RP, RQ, dotA);//triangleArea(RP, RQ);
    float cotS = dotB / TRIANGLE_AREA(SP, SQ, dotB); //triangleArea(SP, SQ);

    return cotR + cotS;
}

/*
Edge * Edge::nextBoundary() {
    nvDebugCheck(this->m_pair == NULL);

}

Edge * Edge::prevBoundary() {
    nvDebugCheck(this->m_pair == NULL);

}
*/


