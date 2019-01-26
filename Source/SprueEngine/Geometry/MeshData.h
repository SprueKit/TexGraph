#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/BlockMap.h>
#include <SprueEngine/Math/Color.h>
#include <SprueEngine/Math/IntVec.h>
#include <SprueEngine/Math/Rect.h>
#include <SprueEngine/Libs/nvmesh/halfedge/Mesh.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <memory>
#include <vector>

namespace SprueEngine
{
    class DistanceField;
    struct MeshOctree;

    struct SPRUE MeshVertex
    {
        Vec3 Position;
        Vec3 Normal;

        MeshVertex() { }
        MeshVertex(const Vec3& pos, const Vec3& normal) : Position(pos), Normal(normal) { }
    };

    /// The spacing in the UV map to place UV coordinates into.
    struct SPRUE UVBox
    {
        /// The area of the object.
        Rect area_;
        /// By default these are implicit objects.
        bool isImplicit_ = true;

        /// Remaps a given normalized UV coordinate into the box
        inline Vec2 FitUV(const Vec2& uv) {
            return Vec2(DENORMALIZE(uv.x, area_.xMin, area_.xMax), DENORMALIZE(uv.y, area_.yMin, area_.yMax));
        }
    };

    typedef std::vector<MeshVertex> VertexBuffer;
    typedef std::vector<unsigned> IndexBuffer;

    class SPRUE MeshData
    {
        //NOCOPYDEF(MeshData);
    public:
        /// Construct.
        MeshData();
        /// Destruct.
        virtual ~MeshData();

        /// Returns the vertex positions.
        std::vector<Vec3>& GetPositionBuffer() { return positionBuffer_; }
        /// Returns the vertex normals.
        std::vector<Vec3>& GetNormalBuffer() { return normalBuffer_; }
        /// Returns the vertex tangents.
        std::vector<Vec4>& GetTangentBuffer() { return tangentBuffer_; }
        /// Returns the vertex colors.
        std::vector<RGBA>& GetColorBuffer() { return colorBuffer_; }
        /// Returns the vertex UV coordinates.
        std::vector<Vec2>& GetUVBuffer() { return uvBuffer_; }
        /// Returns the vertex bone indices.
        std::vector<IntVec4>& GetBoneIndexBuffer() { return boneIndices_; }
        /// Returns the vertex bone weights.
        std::vector<Vec4>& GetBoneWeightBuffer() { return boneWeights_; }
        /// Returns the index buffer, in triangle list layout.
        std::vector<unsigned int>& GetIndexBuffer() { return indexBuffer_; }

        /// Returns the vertex positions.
        const std::vector<Vec3>& GetPositionBuffer() const { return positionBuffer_; }
        /// Returns the vertex normals.
        const std::vector<Vec3>& GetNormalBuffer() const { return normalBuffer_; }
        /// Returns the vertex tangents.
        const std::vector<Vec4>& GetTangentBuffer() const { return tangentBuffer_; }
        /// Returns the vertex colors.
        const std::vector<RGBA>& GetColorBuffer() const { return colorBuffer_; }
        /// Returns the vertex UV coordinates.
        const std::vector<Vec2>& GetUVBuffer() const { return uvBuffer_; }
        /// Returns the vertex bone indices.
        const std::vector<IntVec4>& GetBoneIndexBuffer() const { return boneIndices_; }
        /// Returns the vertex bone weights.
        const std::vector<Vec4>& GetBoneWeightBuffer() const { return boneWeights_; }
        /// Returns the index buffer, in triangle list layout.
        const std::vector<unsigned int>& GetIndexBuffer() const { return indexBuffer_; }

        /// Returns true if this mesh should be voxelized.
        bool IsVoxelized() const { return doNotVoxelize_; }
        /// Sets whether to voxelize this mesh for voxel field inclusion instead of triangle-to-triangle CSG.
        void SetVoxelized(bool value) { doNotVoxelize_ = value; }

        /// Returns whether or not space warps allowed to work on this mesh.
        bool IsWarpEnabled() const { return !doNotWarp_; }
        /// Sets whether to enable spacing warping on this mesh or not.
        void SetWarpEnabled(bool value) { doNotWarp_ = !value; }

        /// Calculate vertex normals
        void CalculateNormals();

        /// Calculates the tangent vectors for the mesh
        void CalculateTangents();

        /// Normalize the bone weight values.
        void NormalizeBoneWeights();

        //DistanceField* GetDistanceField() { return distanceField_.get(); }
        //const DistanceField* GetDistanceField() const { return distanceField_.get(); }

        /// Convert the given input normal into a tangent space normal suitable for writing to a normal map
        static Vec3 ToTangentSpaceNormal(const Vec3& inputNormal, const Vec3& meshNormal, const Vec3& meshTangent, const Vec3& meshBitangent)
        {
            return Vec3(inputNormal.Dot(meshBitangent), inputNormal.Dot(meshTangent), inputNormal.Dot(meshNormal)).Normalized();
        }

        /// Intersects a raycast with the mesh, the ray must be in modelspace.
        float IntersectRay(const Ray& ray, bool ignoreBackfaces, Vec3* norm = 0x0, Vec3* bary = 0x0, unsigned* bestStartIdx = 0x0) const;
        void SweepRayForWinding(const Ray& ray, unsigned& winding) const;

        /// Returns a point interesting on the mesh that intersects or is contained by the given sphere.
        bool IntersectSphere(const Sphere& ray, bool ignoreBackfaces, Vec3* norm, Vec3* bary) const;

        /// Finds the closest point to any triangle in the mesh via brute force.
        Vec3 Closest(const Vec3& toPoint, Vec3* writePos = 0x0, Vec2* writeUV = 0x0, Vec3* writeNormals = 0x0) const;

        /// Sets the minimum enclosing bounding box based on the vertex positions.
        const BoundingBox& CalculateBounds();

        BoundingBox CalculateBounds() const;

        /// Returns the bounding box of the mesh (must have been previously calculated).
        const BoundingBox& GetBounds() const { return bounds_; }

        /// Creates a duplicate of this mesh data.
        std::shared_ptr<MeshData> Clone() const;

        /// Creates a duplicate of this mesh data.
        MeshData* CloneRaw() const;

        /// Subdivides the mesh.
        std::shared_ptr<MeshData> Subdivide(bool smooth) const;

        /// Subdivides for 'n-levels' of passes and the mesh and optionally smooths it once finished.
        std::shared_ptr<MeshData> Subdivide(int levels, bool smooth) const;

        /// Runs mesh decimation on the current mesh and returns the new decimated mesh.
        std::shared_ptr<MeshData> Decimate(float intensity) const;

        /// Smooths vertices based on their one-ring neighbors.
        void Smooth(float strength = 1.0f);
        /// Smooths vertices whose indices are in the set based on their one-ring neighbors.
        void Smooth(std::set<unsigned> indices, float strength);
        /// Adjusts the UV coordinates for displacement and scale (used in packing adjustment to fit into a rect).
        void TransformUV(const Vec2& displace, const Vec2& scale = Vec2(1.0f, 1.0f));
        /// Runs automatic UV coordinate calculations.
        void ComputeUVCoordinates(unsigned width, unsigned height, int quality = 0, int charts = 0, float stretch = 0.75f, float gutter = 2.0f);

        /// Returns the name of this mesh, used for identification.
        const std::string& GetName() const { return meshName_; }
        /// Sets the name of this mesh for identification purposes.
        void SetName(const std::string& name) { meshName_ = name; }

        /// Constructs and returns a new half-edge mesh representation of this mesh, positions and data will be duplicated.
        nv::HalfEdge::Mesh* BuildHalfEdgeMesh() const;

        void Clear();

        bool IsTwoSided() const { return isTwoSided_; }
        void SetTwoSided(bool value) { isTwoSided_ = value; }

        FilterableBlockMap<RGBA>* GetImage() { return image_; }
        void SetImage(FilterableBlockMap<RGBA>* image) { if (image_) delete image_; image_ = image; }

        bool PointIsInside(const Vec3& point) const;

    public:
        /// It might have a name, presently this serves no real purpose.
        std::string meshName_;
        /// Vertex buffer data.
        std::vector<Vec3> positionBuffer_;
        std::vector<Vec3> normalBuffer_;
        std::vector<Vec4> tangentBuffer_;
        std::vector<RGBA> colorBuffer_;
        std::vector<Vec2> uvBuffer_;
        std::vector<IntVec4> boneIndices_;
        std::vector<Vec4> boneWeights_;
        std::vector<unsigned> indexBuffer_;
        bool isTwoSided_ = false;
        FilterableBlockMap<RGBA>* image_ = 0x0;

        /// Returns an octree containing triangles of the mesh.
        std::shared_ptr<MeshOctree> GetOctree() const { return octree_; }

    private:
        //std::shared_ptr<DistanceField> distanceField_;
        std::shared_ptr<MeshOctree> octree_;
        BoundingBox bounds_;

        /// Mesh will be included explicitly as it is and will not be voxelized
        bool doNotVoxelize_;
        /// Mesh vertices will not be warped by space warping
        bool doNotWarp_;
    };

}