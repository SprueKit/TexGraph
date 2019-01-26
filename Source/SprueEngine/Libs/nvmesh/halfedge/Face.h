// This code is in the public domain -- castanyo@yahoo.es

#pragma once
#ifndef NV_MESH_HALFEDGE_FACE_H
#define NV_MESH_HALFEDGE_FACE_H

#include <SprueEngine/Libs/nvmesh/halfedge/Edge.h>

namespace nv
{
    namespace HalfEdge { class Vertex; class Face; class Edge; }

    /// Face of a half-edge mesh.
    class HalfEdge::Face
    {
        Face(const Face&) = delete;
    public:

        unsigned id;
        unsigned short group;
        unsigned short material;
        Edge * edge;


        Face(unsigned id) : id(id), group(~0), material(~0), edge(nullptr) {}

        float area() const;
        float parametricArea() const;
        float boundaryLength() const;
        SprueEngine::Vec3 normal() const;
        SprueEngine::Vec3 centroid() const;

        bool isValid() const;

        bool contains(const Edge * e) const;
        unsigned edgeIndex(const Edge * e) const;
        
        Edge * edgeAt(unsigned idx);
        const Edge * edgeAt(unsigned idx) const;

        unsigned edgeCount() const;
        bool isBoundary() const;
        unsigned boundaryCount() const;


        // The iterator that visits the edges of this face in clockwise order.
        class EdgeIterator //: public Iterator<Edge *>
        {
        public:
            EdgeIterator(Edge * e) : m_end(nullptr), m_current(e) { }

            virtual void advance()
            {
                if (m_end == nullptr) m_end = m_current;
                m_current = m_current->next;
            }

            virtual bool isDone() const { return m_end == m_current; }
            virtual Edge * current() const { return m_current; }
            Vertex * vertex() const { return m_current->vertex; }

        private:
            Edge * m_end;
            Edge * m_current;
        };

        class FaceIterator
        {
        public:
            FaceIterator(Edge* e) : m_end(nullptr), m_current(e) { }

            virtual void advance()
            {
                if (m_end == nullptr) 
                    m_end = m_current;
                m_current = m_current->next;
            }

            virtual bool isDone() const { return m_end == m_current; }
            virtual Face* current() const { return (m_current->pair != 0x0) ? (Face*)m_current->pair->face : (Face*)0x0; }

        private:
            Edge * m_end;
            Edge * m_current;
        };

        FaceIterator neighbors() { return FaceIterator(edge); }

        EdgeIterator edges() { return EdgeIterator(edge); }
        EdgeIterator edges(Edge * e)
        { 
            SPRUE_ASSERT(contains(e), "Edge is not possible");
            return EdgeIterator(e); 
        }

        // The iterator that visits the edges of this face in clockwise order.
        class ConstEdgeIterator //: public Iterator<const Edge *>
        {
        public:
            ConstEdgeIterator(const Edge * e) : m_end(nullptr), m_current(e) { }
            ConstEdgeIterator(const EdgeIterator & it) : m_end(nullptr), m_current(it.current()) { }

            virtual void advance()
            {
                if (m_end == nullptr) m_end = m_current;
                m_current = m_current->next;
            }

            virtual bool isDone() const { return m_end == m_current; }
            virtual const Edge * current() const { return m_current; }
            const Vertex * vertex() const { return m_current->vertex; }

        private:
            const Edge * m_end;
            const Edge * m_current;
        };

        ConstEdgeIterator edges() const { return ConstEdgeIterator(edge); }
        ConstEdgeIterator edges(const Edge * e) const
        { 
            SPRUE_ASSERT(contains(e), "Edge iterator is not possible");
            return ConstEdgeIterator(e); 
        }

    private:
        float triangleArea(const SprueEngine::Vec2& v0, const SprueEngine::Vec2& v1, const SprueEngine::Vec2& v2) const;
        float triangleArea(const SprueEngine::Vec2& v0, const SprueEngine::Vec2& v1) const;
    };

} // nv namespace

#endif // NV_MESH_HALFEDGE_FACE_H
