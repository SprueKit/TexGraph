// This code is in the public domain -- castano@gmail.com

#include "Vertex.h"
#include "Face.h"

using namespace nv;
using namespace HalfEdge;


// Set first edge of all colocals.
void Vertex::setEdge(Edge * e)
{
    for (VertexIterator it(colocals()); !it.isDone(); it.advance()) { 
        it.current()->edge = e;
    }
}

// Update position of all colocals.
void Vertex::setPos(const SprueEngine::Vec3& p)
{
    for (VertexIterator it(colocals()); !it.isDone(); it.advance()) {
        it.current()->pos = p;
    }
}


unsigned HalfEdge::Vertex::colocalCount() const
{
    unsigned count = 0;
    for (ConstVertexIterator it(colocals()); !it.isDone(); it.advance()) { ++count; }
    return count;
}

unsigned HalfEdge::Vertex::valence() const
{
    unsigned count = 0;
    for (ConstEdgeIterator it(edges()); !it.isDone(); it.advance()) { ++count; }
    return count;
}

const HalfEdge::Vertex * HalfEdge::Vertex::firstColocal() const
{
    unsigned firstId = id;
    const Vertex * vertex = this;

    for (ConstVertexIterator it(colocals()); !it.isDone(); it.advance())
    {
        if (it.current()->id < firstId) {
            firstId = vertex->id;
            vertex = it.current();
        }
    }

    return vertex;
}

HalfEdge::Vertex * HalfEdge::Vertex::firstColocal()
{
    Vertex * vertex = this;
    unsigned firstId = id;

    for (VertexIterator it(colocals()); !it.isDone(); it.advance())
    {
        if (it.current()->id < firstId) {
            firstId = vertex->id;
            vertex = it.current();
        }
    }

    return vertex;
}

bool HalfEdge::Vertex::isFirstColocal() const
{
    return firstColocal() == this;
}

bool HalfEdge::Vertex::isColocal(const Vertex * v) const {
    if (this == v) return true;
    if (!pos.Equals(v->pos)) return false;

    for (ConstVertexIterator it(colocals()); !it.isDone(); it.advance())
    {
        if (v == it.current()) {
            return true;
        }
    }

    return false;
}

float HalfEdge::Vertex::surroundingArea() const
{
    float area = 0.0f;
    const nv::HalfEdge::Edge* lastEdge = 0x0;
    for (auto edges = this->edges(); !edges.isDone(); edges.advance())
    {
        if (lastEdge)
            area += math::Triangle(pos, lastEdge->to()->pos, edges.current()->to()->pos).Area();
        lastEdge = edges.current();
        edges.advance();
    }
    return area;
}

float HalfEdge::Vertex::angleDefect() const
{
    float defect= 2 * 3.141596f;

    auto h = edges();
    while (!h.isDone())
    {
        defect -= h.current()->angle();
        h.advance();
    }

    return defect;
}

float HalfEdge::Vertex::dualArea() const
{
    float area = 0.0f;
    auto h = edges();
    while (!h.isDone())
    {
        if (h.current()->face)
            area += h.current()->face->area();        
        h.advance();
    } 
    return area / 3.0f;
}

float HalfEdge::Vertex::gaussCurvature() const
{
    return angleDefect() / dualArea();
}

float HalfEdge::Vertex::normalCurvature(float theta) const
{
    float cos2 = powf(cosf(theta * 3.141596f / 180.0f), 2);
    float sin2 = powf(sinf(theta * 3.141596f / 180.0f), 2);
    
    return cos2 + sin2;
    //return k1 * cos2 + k2 * sin2;
}