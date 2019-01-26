#include <SprueEngine/Geometry/MeshData.h>

#include <SprueEngine/Math/Triangle.h>
#include <SprueEngine/Math/Trig.h>
#include <SprueEngine/Meshing/Simplify.h>

#include <SprueEngine/Libs/nvmesh/halfedge/Vertex.h>
#include <SprueEngine/Libs/nvmesh/halfedge/Face.h>
#include <SprueEngine/Libs/nvmesh/halfedge/Edge.h>

#include <limits>
#include <algorithm>

namespace SprueEngine
{
    MeshData::MeshData()
        //distanceField_(0x0)
    {

    }

    MeshData::~MeshData()
    {
        if (image_)
            delete image_;
    }

    void MeshData::CalculateNormals()
    {
        if (positionBuffer_.empty())
            return;

        normalBuffer_.clear();
        normalBuffer_.resize(positionBuffer_.size());

        std::map<Vec3, Vec3> normalAccumulation;

        for (unsigned int i = 0; i < indexBuffer_.size(); i += 3)
        {
            const int i0 = indexBuffer_[i];
            const int i1 = indexBuffer_[i + 1];
            const int i2 = indexBuffer_[i + 2];

            Vec3& v0 = positionBuffer_[i0];
            Vec3& v1 = positionBuffer_[i1];
            Vec3& v2 = positionBuffer_[i2];

            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            Vec3 normal = edge1.Cross(edge2).Normalized();

            normalAccumulation[v0] += normal;
            normalAccumulation[v1] += normal;
            normalAccumulation[v2] += normal;
        }

        for (auto record : normalAccumulation)
            record.second.Normalized();

        for (unsigned i = 0; i < indexBuffer_.size(); ++i)
        {
            const int i0 = indexBuffer_[i];
            Vec3& v0 = positionBuffer_[i0];
            normalBuffer_[i0] = normalAccumulation[v0];
        }
    }

    void MeshData::CalculateTangents()
    {
        if (positionBuffer_.size() == 0 || uvBuffer_.size() == 0 || indexBuffer_.size() == 0)
            return;

        tangentBuffer_.resize(positionBuffer_.size());

        const unsigned vertexCount = tangentBuffer_.size();
        Vec3* tan1 = new Vec3[vertexCount * 2];
        Vec3* tan2 = tan1 + vertexCount;
        memset(tan1, 0, sizeof(Vec3) * vertexCount * 2);

        for (unsigned i = 0; i < indexBuffer_.size(); i += 3)
        {
            unsigned i1 = indexBuffer_[i];
            unsigned i2 = indexBuffer_[i + 1];
            unsigned i3 = indexBuffer_[i + 2];

            const Vec3& v1 = positionBuffer_[i1];
            const Vec3& v2 = positionBuffer_[i2];
            const Vec3& v3 = positionBuffer_[i3];

            const Vec2& w1 = uvBuffer_[i1];
            const Vec2& w2 = uvBuffer_[i2];
            const Vec2& w3 = uvBuffer_[i3];

            float x1 = v2.x - v1.x;
            float x2 = v3.x - v1.x;
            float y1 = v2.y - v1.y;
            float y2 = v3.y - v1.y;
            float z1 = v2.z - v1.z;
            float z2 = v3.z - v1.z;

            float s1 = w2.x - w1.x;
            float s2 = w3.x - w1.x;
            float t1 = w2.y - w1.y;
            float t2 = w3.y - w1.y;

            float r = 1.0f / (s1 * t2 - s2 * t1);
            Vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
            Vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

            tan1[i1] += sdir;
            tan1[i2] += sdir;
            tan1[i3] += sdir;

            tan2[i1] += tdir;
            tan2[i2] += tdir;
            tan2[i3] += tdir;
        }

        for (unsigned i = 0; i < tangentBuffer_.size(); i++)
        {
            const Vec3& n = normalBuffer_[i];
            const Vec3& t = tan1[i];
            Vec3 xyz;
            float w;

            // Gram-Schmidt orthogonalize
            xyz = (t - n * n.Dot(t)).Normalized();

            // Calculate handedness
            w = n.Cross(t).Dot(tan2[i]) < 0.0f ? -1.0f : 1.0f;

            Vec4& tangent = tangentBuffer_[i];
            tangent = Vec4(xyz, w);
        }

        // Allocate the number of required tangent vectors
        //tangentBuffer_.clear();
        //tangentBuffer_.resize(positionBuffer_.size());
        //
        //for (unsigned int i = 0; i < indexBuffer_.size(); i += 3) 
        //{
        //    const unsigned i0 = indexBuffer_[i];
        //    const unsigned i1 = indexBuffer_[i + 1];
        //    const unsigned i2 = indexBuffer_[i + 2];
        //
        //    Vec3& v0 = positionBuffer_[i0];
        //    Vec3& v1 = positionBuffer_[i1];
        //    Vec3& v2 = positionBuffer_[i2];
        //
        //    Vec3 edge1 = v1 - v0;
        //    Vec3 edge2 = v2 - v0;
        //
        //    Vec2& t0 = uvBuffer_[i0];
        //    Vec2& t1 = uvBuffer_[i1];
        //    Vec2& t2 = uvBuffer_[i2];
        //
        //    float deltaU1 = t1.x - t0.x;
        //    float deltaV1 = t1.y - t0.y;
        //    float deltaU2 = t2.x - t0.x;
        //    float deltaV2 = t2.y - t0.y;
        //
        //    float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);
        //
        //    Vec3 tangent((deltaV2 * edge1.x - deltaV1 * edge2.x) * f,
        //        (deltaV2 * edge1.y - deltaV1 * edge2.y) * f,
        //        (deltaV2 * edge1.z - deltaV1 * edge2.z) * f);
        //
        //    //Vec3 bitangent((-deltaU2 * edge1.x - deltaU1 * edge2.x) * f,
        //    //    (-deltaU2 * edge1.y - deltaU1 * edge2.y) * f,
        //    //    (-deltaU2 * edge1.z - deltaU1 * edge2.z) * f);
        //
        //    tangentBuffer_[i0] += tangent;
        //    tangentBuffer_[i1] += tangent;
        //    tangentBuffer_[i2] += tangent;
        //}
        //
        //for (unsigned int i = 0; i < tangentBuffer_.size(); ++i)
        //    tangentBuffer_[i].Normalize();
    }

    void MeshData::NormalizeBoneWeights()
    {
        for (unsigned int i = 0; i < boneWeights_.size(); ++i)
            boneWeights_[i].Normalize();
    }

    float MeshData::IntersectRay(const Ray& ray, bool ignoreBackFaces, Vec3* norm, Vec3* bary, unsigned* bestStartIdx) const
    {
        float nearest = std::numeric_limits<float>::max();
        unsigned nearestIndex = -1;

        for (unsigned i = 0; i < indexBuffer_.size(); i += 3)
        {
            Vec3 v0 = positionBuffer_[indexBuffer_[i]];
            Vec3 v1 = positionBuffer_[indexBuffer_[i + 1]];
            Vec3 v2 = positionBuffer_[indexBuffer_[i + 2]];

            math::Triangle tri(v0, v1, v2);
            float distance = 0.0f;
            vec pt;
            if (tri.Intersects(ray, &distance, &pt))
            {
                if (distance >= 0.0f && distance < nearest)
                {
                    if (norm)
                        *norm = tri.NormalCCW();/*normal;*/
                    if (bary)
                        *bary = tri.BarycentricUVW(pt);
                        //*bary = Vec3(1 - (u / det) - (v / det), u / det, v / det);
                    if (bestStartIdx)
                        *bestStartIdx = i;
                    nearest = distance;
                }
            }

            //Vec3 e1(v1 - v0);
            //Vec3 e2(v2 - v0);
            //Vec3 normal = e1.Cross(e2);
            //if (normal.Normalized().Dot(ray.dir) > 0.0f)
            //    continue;
            //
            //Vec3 p(ray.dir.Cross(e2));
            //float det = e1.Dot(p);
            //if (det < EPSILON)
            //    continue;
            //
            //Vec3 t(ray.pos - v0);
            //float u = t.Dot(p);
            //if (u < 0.0f || u > det)
            //    continue;
            //
            //Vec3 q(t.Cross(e1));
            //float v = ray.dir.Dot(q);
            //if (v < 0.0f || u + v > det)
            //    continue;
            //
            //float distance = e2.Dot(q) / det;
            //if (distance >= 0.0f && distance < nearest)
            //{
            //    if (norm)
            //        *norm = normal;
            //    if (bary)
            //        *bary = Vec3(1 - (u / det) - (v / det), u / det, v / det);
            //    if (bestStartIdx)
            //        *bestStartIdx = i;
            //    nearest = distance;
            //}
        }
        return nearest;
    }

    void MeshData::SweepRayForWinding(const Ray& ray, unsigned& winding) const
    {
        float nearest = std::numeric_limits<float>::max();
        unsigned nearestIndex = -1;

        for (unsigned i = 0; i < indexBuffer_.size(); i += 3)
        {
            Vec3 v0 = positionBuffer_[indexBuffer_[i]];
            Vec3 v1 = positionBuffer_[indexBuffer_[i + 1]];
            Vec3 v2 = positionBuffer_[indexBuffer_[i + 2]];
            math::Triangle t(v0, v1, v2);
            if (t.Intersects(ray))
            {
                if (t.NormalCCW().Dot(ray.dir) >= 0.0f)
                    ++winding;
                else
                    --winding;
            }
        }
    }

    bool MeshData::IntersectSphere(const Sphere& sphere, bool ignoreBackfaces, Vec3* norm, Vec3* bary) const
    {
        float nearest = std::numeric_limits<float>::max();
        unsigned* bestIndex;

        float rad2 = sphere.r * sphere.r;
        Vec3 testPoints[3];
        for (unsigned i = 0; i < indexBuffer_.size(); i += 3)
        {
            testPoints[0] = positionBuffer_[indexBuffer_[i]];
            testPoints[1] = positionBuffer_[indexBuffer_[i + 1]];
            testPoints[2] = positionBuffer_[indexBuffer_[i + 2]];

            Vec3 e1(testPoints[1] - testPoints[0]);
            Vec3 e2(testPoints[2] - testPoints[0]);
            Vec3 normal = e1.Cross(e2).Normalized();

            Plane p(testPoints[0], testPoints[1], testPoints[2]);

            // Project onto plane
            const Vec3 ptOnPlane = p.ClosestPoint(sphere.pos);
            Vec3 dirToPlane = (ptOnPlane - sphere.pos);

            // Can't possibly collide if further than radius from the plane
            float distance = dirToPlane.Length();
            dirToPlane.Normalize();
            if (distance > sphere.r || (ignoreBackfaces && dirToPlane.Dot(normal) >= 0.0f))
                continue;

            // Determine the point in relation to the triangle
            const Vec3 bary = GetBarycentricFactors(testPoints, ptOnPlane);
            const Vec3 ptOnTriangle(
                testPoints[0].x * bary.x + testPoints[1].x * bary.y + testPoints[2].x * bary.z,
                testPoints[0].y * bary.x + testPoints[1].y * bary.y + testPoints[2].y * bary.z,
                testPoints[0].z * bary.x + testPoints[1].z * bary.y + testPoints[2].z * bary.z);

            distance = (ptOnTriangle - sphere.pos).Length();
            if (distance <= sphere.r && distance < nearest)
            {
                nearest = distance;
                if (bestIndex)
                    *bestIndex = i;
                if (norm)
                    *norm = normal;
                return true;
            }
        }
        return false;
    }

    Vec3 MeshData::Closest(const Vec3& toPoint, Vec3* writePos, Vec2* writeUV, Vec3* writeNormals) const
    {
        std::vector<float> nearest;
        nearest.resize(indexBuffer_.size() / 3);
        for (unsigned i = 0; i < indexBuffer_.size(); i += 3)
        {
            Triangle tri(positionBuffer_[indexBuffer_[i]], positionBuffer_[indexBuffer_[i + 1]], positionBuffer_[indexBuffer_[i + 2]]);
            nearest[indexBuffer_[i]] = (tri.ClosestPoint(toPoint) - toPoint).LengthSq();
        }

        unsigned closestIdx = std::distance(std::min_element(nearest.begin(), nearest.end()), nearest.begin());
        
        Triangle tri(positionBuffer_[closestIdx*3], positionBuffer_[closestIdx * 3 + 1], positionBuffer_[closestIdx * 3 + 2]);
        if (writePos != 0x0)
            memcpy(writePos, tri.data(), sizeof(Vec3) * 3);
        if (writeUV != 0x0)
            memcpy(writeUV, &uvBuffer_[closestIdx * 3], sizeof(Vec2) * 3);
        if (writeNormals)
            memcpy(writeNormals, &normalBuffer_[closestIdx * 3], sizeof(Vec3) * 3);
        return tri.ClosestPoint(toPoint);
    }

    const BoundingBox& MeshData::CalculateBounds() 
    {
        for (unsigned i = 0; i < positionBuffer_.size(); ++i)
        {
            if (i == 0)
                bounds_.minPoint = bounds_.maxPoint = positionBuffer_[i];
            else
                bounds_.Enclose(positionBuffer_[i]);
        }
        return GetBounds();
    }

    BoundingBox MeshData::CalculateBounds() const
    {
        BoundingBox bounds;
        for (unsigned i = 0; i < positionBuffer_.size(); ++i)
        {
            if (i == 0)
                bounds.minPoint = bounds.maxPoint = positionBuffer_[i];
            else
                bounds.Enclose(positionBuffer_[i]);
        }
        return bounds;
    }

    std::shared_ptr<MeshData> MeshData::Clone() const
    {
        std::shared_ptr<MeshData> ret(new MeshData());

        ret->indexBuffer_ = indexBuffer_;
        ret->positionBuffer_ = positionBuffer_;
        ret->normalBuffer_ = normalBuffer_;
        ret->uvBuffer_ = uvBuffer_;
        ret->tangentBuffer_ = tangentBuffer_;
        ret->colorBuffer_ = colorBuffer_;

        ret->doNotVoxelize_ = doNotVoxelize_;
        ret->doNotWarp_ = doNotWarp_;

        ret->bounds_ = bounds_;

        return ret;
    }

    MeshData* MeshData::CloneRaw() const
    {
        MeshData* ret = new MeshData();

        ret->meshName_ = meshName_;
        ret->indexBuffer_ = indexBuffer_;
        ret->positionBuffer_ = positionBuffer_;
        ret->normalBuffer_ = normalBuffer_;
        ret->uvBuffer_ = uvBuffer_;
        ret->tangentBuffer_ = tangentBuffer_;
        ret->colorBuffer_ = colorBuffer_;

        ret->doNotVoxelize_ = doNotVoxelize_;
        ret->doNotWarp_ = doNotWarp_;

        ret->bounds_ = bounds_;

        return ret;
    }

    template<class T>
    void Extract(T* dest, const std::vector<T>& buffer, unsigned a, unsigned b, unsigned c)
    {
        dest[0] = buffer[a];
        dest[1] = buffer[b];
        dest[2] = buffer[c];
    }

    template<class T>
    void InterpolateTriangle(const T* src, T* dest)
    {
        dest[0] = (src[0] + src[1]) / 2.0f;
        dest[1] = (src[1] + src[2]) / 2.0f;
        dest[2] = (src[0] + src[2]) / 2.0f;
    }

    template<class T>
    void InterpolateVertexData(const std::vector<T>& src, std::vector<T>& dest, unsigned a, unsigned b, unsigned c)
    {
        if (src.size() == 0)
            return;
        T srcValue[3];
        Extract<T>(srcValue, src, a, b, c);
        T destValue[3];
        InterpolateTriangle<T>(srcValue, destValue);
        dest.push_back(destValue[0]);
        dest.push_back(destValue[1]);
        dest.push_back(destValue[2]);
    }

    template<class T>
    void InterpolateVertexDataNormal(const std::vector<T>& src, std::vector<T>& dest, unsigned a, unsigned b, unsigned c)
    {
        if (src.size() == 0)
            return;
        T srcValue[3];
        Extract<T>(srcValue, src, a, b, c);
        T destValue[3];
        InterpolateTriangle<T>(srcValue, destValue);
        dest.push_back(destValue[0].Normalized());
        dest.push_back(destValue[1].Normalized());
        dest.push_back(destValue[2].Normalized());
    }

    std::shared_ptr<MeshData> MeshData::Subdivide(bool smooth) const
    {
        std::shared_ptr<MeshData> ret = Clone();

        for (unsigned i = 0; i < indexBuffer_.size(); i += 3)
        {
            const unsigned a = indexBuffer_[i];
            const unsigned b = indexBuffer_[i + 1];
            const unsigned c = indexBuffer_[i + 2];

            const unsigned startNewIndices = ret->positionBuffer_.size();

            // insert vertices at each edge's midpoints
            InterpolateVertexData<Vec3>(positionBuffer_, ret->positionBuffer_, a, b, c);
            InterpolateVertexDataNormal<Vec3>(normalBuffer_, ret->normalBuffer_, a, b, c);
            InterpolateVertexDataNormal<Vec4>(tangentBuffer_, ret->tangentBuffer_, a, b, c);
            InterpolateVertexData<Vec2>(uvBuffer_, ret->uvBuffer_, a, b, c);
            InterpolateVertexData<RGBA>(colorBuffer_, ret->colorBuffer_, a, b, c);
            InterpolateVertexData<Vec4>(boneWeights_, ret->boneWeights_, a, b, c);

            // Left bottom, replace current
            ret->indexBuffer_[i] = a;
            ret->indexBuffer_[i + 1] = startNewIndices;
            ret->indexBuffer_[i + 2] = startNewIndices + 2;

            // Center
            ret->indexBuffer_.push_back(startNewIndices);
            ret->indexBuffer_.push_back(startNewIndices + 1);
            ret->indexBuffer_.push_back(startNewIndices + 2);

            // top
            ret->indexBuffer_.push_back(startNewIndices);
            ret->indexBuffer_.push_back(b);
            ret->indexBuffer_.push_back(startNewIndices + 1);

            // bottom right
            ret->indexBuffer_.push_back(startNewIndices + 2);
            ret->indexBuffer_.push_back(startNewIndices + 1);
            ret->indexBuffer_.push_back(c);
        }

        if (smooth)
            ret->Smooth();

        return ret;
    }

    std::shared_ptr<MeshData> MeshData::Subdivide(int levels, bool smooth) const
    {
        std::shared_ptr<MeshData> ret;
        ret = Subdivide(smooth);
        while (--levels > 0)
            ret = ret->Subdivide(smooth);
        return ret;
    }

    void MeshData::Smooth(float strength)
    {
        Smooth(std::set<unsigned>(), strength);
    }

    void MeshData::Smooth(std::set<unsigned> indices, float strength)
    {
        std::vector<Vec3> cache = positionBuffer_;
        if (nv::HalfEdge::Mesh* mesh = BuildHalfEdgeMesh())
        {
            auto vertIt = mesh->vertices();
            while (!vertIt.isDone())
            {
                auto vertex = vertIt.current();
                const unsigned index = vertex->id;

                // If we have a list to filter by then only smooth vertices in the list
                if (!indices.empty() && indices.find(index) == indices.end())
                    continue;

                auto edgeIt = vertex->edges();
                Vec3 averagePos;
                int neighborCount = 0;
                while (!edgeIt.isDone())
                {
                    ++neighborCount;
                    averagePos += edgeIt.current()->to()->pos;
                    edgeIt.advance();
                }

                if (neighborCount > 0)
                    averagePos /= neighborCount;

                cache[index] = SprueLerp(positionBuffer_[index], averagePos, strength);

                vertIt.advance();
            }
            positionBuffer_ = cache;

            delete mesh;
        }
    }

    std::shared_ptr<MeshData> MeshData::Decimate(float intensity) const
    {
        std::shared_ptr<MeshData> ret = Clone();

        if (indexBuffer_.size() < 3)
            return ret;

        Simplify::vertices.clear();
        Simplify::triangles.clear();
        Simplify::refs.clear();

        Simplify::vertices.resize(positionBuffer_.size());
        Simplify::triangles.resize(indexBuffer_.size() / 3);
        for (unsigned i = 0; i < positionBuffer_.size(); ++i)
        {
            Simplify::vertices[i].p = positionBuffer_[i];
            if (uvBuffer_.size() != 0)
                Simplify::vertices[i].uv = uvBuffer_[i];
        }

        for (unsigned i = 0; i < indexBuffer_.size(); i += 3)
        {
            Simplify::triangles[i].v[0] = i;
            Simplify::triangles[i].v[1] = i + 1;
            Simplify::triangles[i].v[2] = i + 2;

            Vec3 e0 = positionBuffer_[i + 1] - positionBuffer_[i];
            Vec3 e2 = positionBuffer_[i + 2] - positionBuffer_[i];
            Simplify::triangles[i].n = e0.Cross(e2).Normalized();
        }

        Simplify::simplify_mesh(positionBuffer_.size() * intensity);

        // ?? WTF do we do now?
        ret->positionBuffer_.resize(Simplify::vertices.size());
        ret->indexBuffer_.resize(Simplify::triangles.size() * 3);

        for (unsigned i = 0; i < Simplify::vertices.size(); ++i)
            ret->positionBuffer_[i] = Simplify::vertices[i].p;

        for (unsigned i = 0; i < Simplify::triangles.size(); ++i)
        {
            ret->indexBuffer_[i * 3] = Simplify::triangles[i].v[0];
            ret->indexBuffer_[i * 3 + 1] = Simplify::triangles[i].v[0];
            ret->indexBuffer_[i * 3 + 2] = Simplify::triangles[i].v[0];
        }

        return ret;
    }

    void MeshData::TransformUV(const Vec2& displace, const Vec2& scale)
    {
        for (unsigned i = 0; i < uvBuffer_.size(); ++i)
            uvBuffer_[i] = (uvBuffer_[i] + displace) * scale;
    }

    nv::HalfEdge::Mesh* MeshData::BuildHalfEdgeMesh() const
    {
        nv::HalfEdge::Mesh* mesh = new nv::HalfEdge::Mesh();

        //std::vector<uint32_t> canonicalMap;
        //canonicalMap.reserve(positionBuffer_.size());

        for (int i = 0; i < positionBuffer_.size(); i++) {
            nv::HalfEdge::Vertex* vertex = mesh->addVertex(positionBuffer_[i]);
            if (uvBuffer_.size())
                vertex->tex = uvBuffer_[i];
            if (colorBuffer_.size())
                vertex->col = colorBuffer_[i];
            if (normalBuffer_.size())
                vertex->nor = normalBuffer_[i];

            //canonicalMap.push_back(input_vertex.first_colocal);
        }

        mesh->linkColocals();
        //mesh->linkColocalsWithCanonicalMap(canonicalMap);

        int non_manifold_faces = 0;
        for (int i = 0; i < indexBuffer_.size(); i += 3) {
            int v0 = indexBuffer_[i];
            int v1 = indexBuffer_[i+1];
            int v2 = indexBuffer_[i+2];

            nv::HalfEdge::Face * face = mesh->addFace(v0, v1, v2);
            if (face != NULL) 
            {
                //face->material = input_face.material_index;
            }
            else 
            {
                non_manifold_faces++;
            }
        }

        mesh->linkBoundary();

        return mesh;
    }

    void MeshData::Clear()
    {
        positionBuffer_.clear();
        normalBuffer_.clear();
        tangentBuffer_.clear();
        colorBuffer_.clear();
        uvBuffer_.clear();
        boneIndices_.clear();
        boneWeights_.clear();
        indexBuffer_.clear();
    }
}