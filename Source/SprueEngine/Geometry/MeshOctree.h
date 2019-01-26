#pragma once

#include <SprueEngine/IMeshable.h>
#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/Math/Triangle.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{
    #define OCTREE_MAX_DEPTH 250

    struct MeshOctreeConstructionData
    {
        Vec3* vertices;     // Required for both vertex and triangle mode
        Vec3* normals;      // Required for vertex mode for querying with normal vector constraints
        unsigned vertexCt;  // Number of vertices in the vertex data above
        unsigned* indices;  // Index buffer, required for triangle mode
        unsigned indexCt;   // Number of indices in the index data bove
    };

    /// An octree structure that can work with either triangles or vertex clouds
    /// For accelerating queries
    struct MeshOctree : public IMeshable
    {
    private:
        MeshOctree() : children_(0x0), depth_(0), constructionData_(0x0) { }
    public:
        MeshOctree(MeshOctreeConstructionData& meshData, unsigned char maxDepth = 8);
        virtual ~MeshOctree()
        {
            if (children_)
                delete[] children_;
            children_ = 0x0;
        }

        BoundingBox bounds_;
        MeshOctree* children_ = 0x0;
        std::vector< std::pair<unsigned, Vec3> > contents_;
        float distances_[8];
        float unsignedDistances_[8];
        unsigned char depth_;
        unsigned char maxDepth_;
        bool triangleMode_; // sampling is performed as triangles

        /// Collection functions, the containing results values will be
        ///     Triangle Mode:  index into the index buffer for the start of the triangle
        ///     Vertex Mode:    index into the vertex position data for the vertex
        void CollectRaycast(const Ray& ray, std::vector<unsigned>& results, Vec3* withNormal = 0x0, float minNormalTolerance = EPSILON, float maxNormalTolerance = 1.0f) const;
        void CollectSphere(const Sphere& sphere, std::vector<unsigned>& results, Vec3* withNormal = 0x0, float minNormalTolerance = EPSILON, float maxNormalTolerance = 1.0f) const;
        void CollectBox(const BoundingBox& bounds, std::vector<unsigned>& results, Vec3* withNormal = 0x0, float minNormalTolerance = EPSILON, float maxNormalTolerance = 1.0f) const;
        void CollectPoint(const Vec3& point, std::vector<unsigned>& results, Vec3* withNormal = 0x0, float minNormalTolerance = EPSILON, float maxNormalTolerance = 1.0f) const;
        /// Returns true if a ray between two points strikes anything but the destination
        void CollectLine(const Vec3& from, const Vec3& to, std::vector<unsigned>& results, unsigned* hitVertex = 0x0) const;

        /// Determine the signed distance from the nearest triangle to point P
        float SignedDistance(const Vec3& point, Vec3* foundNormal = 0x0) const;
        /// Find the nearest distance of interpolated cell values
        bool NearestLerpedDistance(const Vec3& point, float& holder) const;

        /// Finds the signed distance from the closest triangle to the given position. Clockwise winding for plane facing.
        virtual float CalculateDensity(const Vec3& position) const;

        /// Returns false if the line segment collides with any of the listed triangles.
        bool TestLine(const Vec3& from, const Vec3& to, const std::vector<unsigned>& results, unsigned ignoreIndex, Vec3* hitPos = 0x0);
        bool TraceLine(const Vec3& from, const Vec3& to, unsigned ignoreIndex = -1, Vec3* hitPos = 0x0);

    protected:
        /// Subdivide into children
        void Split();
        void Collapse(); // Collapse the octree?

        /// Insertion functions
        bool Insert(std::pair<unsigned, Vec3>& item);
        bool Insert(std::pair<unsigned, Vec3>& item, const Triangle& tri, const BoundingBox& bounds);
        void CheckVertex(unsigned vertIndex, const Triangle& tri);
        float LerpDistances(const Vec3& point) const;

        void Collect(std::vector<unsigned>& results, Vec3* withNormal = 0x0, float minNormalTolerance = EPSILON, float maxNormalTolerance = 1.0f) const;
        /// Determine whether subdivision is valid
        bool ShouldSplit(const BoundingBox& bounds) const;
        MeshOctreeConstructionData* constructionData_;
    };
}