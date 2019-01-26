// This code is in the public domain -- castanyo@yahoo.es

#pragma once
#ifndef NV_MESH_HALFEDGE_MESH_H
#define NV_MESH_HALFEDGE_MESH_H

#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/Math/Color.h>

#include <map>
#include <vector>

/*
If I were to redo this again, there are a number of things that I would do differently.
- Edge map is only useful when importing a mesh to guarantee the result is two-manifold. However, when manipulating the mesh
  it's a pain to maintain the map up to date.
- Edge array only points to the even vertices. There's no good reason for that. The map becomes required to traverse all edges
  or you have to make sure edges are properly paired.
- Linked boundaries. It's cleaner to assume a NULL pair means a boundary edge. Makes easier to seal boundaries. The only reason
  why we link boundaries is to simplify traversal, but that could be done with two helper functions (nextBoundary, prevBoundary).
- Minimize the amount of state that needs to be set in a certain way:
    - boundary vertices point to boundary edge.
- Remove parenthesis! Make some members public.
- Remove member functions with side effects:
    - e->setNext(n) modifies e->next and n->prev, instead use "link(e, n)", or "e->next = n, n->prev = e"
*/


namespace nv
{
    class TriMesh;
    class QuadTriMesh;
    //template <typename T> struct Hash<Mesh::Key>;

    namespace HalfEdge
    {
        class Edge;
        class Face;
        class Vertex;

        /// Simple half edge mesh designed for dynamic mesh manipulation.
        class Mesh
        {
        public:

            Mesh();
            Mesh(const Mesh * mesh);
            ~Mesh();

            void clear();

            Vertex * addVertex(const SprueEngine::Vec3 & pos);
            //Vertex * addVertex(uint id, const Vector3 & pos);
            //void addVertices(const Mesh * mesh);
            void addVertices(const SprueEngine::Vec3* verts, uint32_t ct, const SprueEngine::Vec2* uv = 0x0, const SprueEngine::Vec3* norms = 0x0, const SprueEngine::RGBA* colors = 0x0);

            void linkColocals();
            void linkColocalsWithCanonicalMap(const std::vector<uint32_t> & canonicalMap);
            void resetColocalLinks();

            Face * addFace();
            Face * addFace(uint32_t v0, uint32_t v1, uint32_t v2);
            Face * addFace(uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3);
            Face * addFace(const std::vector<uint32_t> & indexArray);
            Face * addFace(const std::vector<uint32_t> & indexArray, uint32_t first, uint32_t num);
            //void addFaces(const Mesh * mesh);

            // These functions disconnect the given element from the mesh and delete it.
            void disconnect(Edge * edge);
            void disconnectPair(Edge * edge);
            void disconnect(Vertex * vertex);
            void disconnect(Face * face);

            void remove(Edge * edge);
            void remove(Vertex * vertex);
            void remove(Face * face);

            // Remove holes from arrays and reassign indices.
            void compactEdges();
            void compactVertices();
            void compactFaces();

            void triangulate();

            void linkBoundary();
            
            bool splitBoundaryEdges(); // Returns true if any split was made.

            // Sew the boundary that starts at the given edge, returns one edge that still belongs to boundary, or NULL if boundary closed.
            HalfEdge::Edge * sewBoundary(Edge * startEdge);


            // Vertices
            uint32_t vertexCount() const { return m_vertexArray.size(); }
            const Vertex * vertexAt(int i) const { return m_vertexArray[i]; }
            Vertex * vertexAt(int i) { return m_vertexArray[i]; }

            uint32_t colocalVertexCount() const { return m_colocalVertexCount; }

            // Faces
            uint32_t faceCount() const { return m_faceArray.size(); }
            const Face * faceAt(int i) const { return m_faceArray[i]; }
            Face * faceAt(int i) { return m_faceArray[i]; }

            // Edges
            uint32_t edgeCount() const { return m_edgeArray.size(); }
            const Edge * edgeAt(int i) const { return m_edgeArray[i]; }
            Edge * edgeAt(int i) { return m_edgeArray[i]; }

            class ConstVertexIterator;

            class VertexIterator
            {
                friend class ConstVertexIterator;
            public:
                VertexIterator(Mesh * mesh) : m_mesh(mesh), m_current(0) { }

                virtual void advance() { m_current++; }
                virtual bool isDone() const { return m_current == m_mesh->vertexCount(); }
                virtual Vertex * current() const { return m_mesh->vertexAt(m_current); }

            private:
                HalfEdge::Mesh * m_mesh;
                uint32_t m_current;
            };
            VertexIterator vertices() { return VertexIterator(this); }

            class ConstVertexIterator
            {
            public:
                ConstVertexIterator(const Mesh * mesh) : m_mesh(mesh), m_current(0) { }
                ConstVertexIterator(class VertexIterator & it) : m_mesh(it.m_mesh), m_current(it.m_current) { }

                virtual void advance() { m_current++; }
                virtual bool isDone() const { return m_current == m_mesh->vertexCount(); }
                virtual const Vertex * current() const { return m_mesh->vertexAt(m_current); }

            private:
                const HalfEdge::Mesh * m_mesh;
                uint32_t m_current;
            };
            ConstVertexIterator vertices() const { return ConstVertexIterator(this); }

            class ConstFaceIterator;

            class FaceIterator
            {
                friend class ConstFaceIterator;
            public:
                FaceIterator(Mesh * mesh) : m_mesh(mesh), m_current(0) { }

                virtual void advance() { m_current++; }
                virtual bool isDone() const { return m_current == m_mesh->faceCount(); }
                virtual Face * current() const { return m_mesh->faceAt(m_current); }

            private:
                HalfEdge::Mesh * m_mesh;
                uint32_t m_current;
            };
            FaceIterator faces() { return FaceIterator(this); }

            class ConstFaceIterator
            {
            public:
                ConstFaceIterator(const Mesh * mesh) : m_mesh(mesh), m_current(0) { }
                ConstFaceIterator(const FaceIterator & it) : m_mesh(it.m_mesh), m_current(it.m_current) { }

                virtual void advance() { m_current++; }
                virtual bool isDone() const { return m_current == m_mesh->faceCount(); }
                virtual const Face * current() const { return m_mesh->faceAt(m_current); }

            private:
                const HalfEdge::Mesh * m_mesh;
                uint32_t m_current;
            };
            ConstFaceIterator faces() const { return ConstFaceIterator(this); }

            class ConstEdgeIterator;

            class EdgeIterator
            {
                friend class ConstEdgeIterator;
            public:
                EdgeIterator(Mesh * mesh) : m_mesh(mesh), m_current(0) { }

                virtual void advance() { m_current++; }
                virtual bool isDone() const { return m_current == m_mesh->edgeCount(); }
                virtual Edge * current() const { return m_mesh->edgeAt(m_current); }

            private:
                HalfEdge::Mesh * m_mesh;
                uint32_t m_current;
            };
            EdgeIterator edges() { return EdgeIterator(this); }

            class ConstEdgeIterator
            {
            public:
                ConstEdgeIterator(const Mesh * mesh) : m_mesh(mesh), m_current(0) { }
                ConstEdgeIterator(const EdgeIterator & it) : m_mesh(it.m_mesh), m_current(it.m_current) { }

                virtual void advance() { m_current++; }
                virtual bool isDone() const { return m_current == m_mesh->edgeCount(); }
                virtual const Edge * current() const { return m_mesh->edgeAt(m_current); }

            private:
                const HalfEdge::Mesh * m_mesh;
                uint32_t m_current;
            };
            ConstEdgeIterator edges() const { return ConstEdgeIterator(this); }

            // @@ Add half-edge iterator.



            // Convert to tri mesh.
            TriMesh * toTriMesh() const;
            QuadTriMesh * toQuadTriMesh() const;

            bool isValid() const;

        public:

            // Error status:
            mutable uint32_t errorCount;
            mutable uint32_t errorIndex0;
            mutable uint32_t errorIndex1;

        private:

            bool canAddFace(const std::vector<uint32_t> & indexArray, uint32_t first, uint32_t num) const;
            bool canAddEdge(uint32_t i, uint32_t j) const;
            Edge * addEdge(uint32_t i, uint32_t j);

            Edge * findEdge(uint32_t i, uint32_t j) const;

            void linkBoundaryEdge(Edge * edge);
            Vertex * splitBoundaryEdge(Edge * edge, float t, const SprueEngine::Vec3 & pos);
            void splitBoundaryEdge(Edge * edge, Vertex * vertex);

        private:

            std::vector<Vertex *> m_vertexArray;
            std::vector<Edge *> m_edgeArray;
            std::vector<Face *> m_faceArray;

            std::map<std::pair<uint32_t, uint32_t>, Edge *> m_edgeMap;

            uint32_t m_colocalVertexCount;

        };
        /*
        // This is a much better hash than the default and greatly improves performance!
        template <> struct hash<Mesh::Key>
        {
        uint operator()(const Mesh::Key & k) const { return k.p0 + k.p1; }
        };
        */

    } // HalfEdge namespace

} // nv namespace

#endif // NV_MESH_HALFEDGE_MESH_H
