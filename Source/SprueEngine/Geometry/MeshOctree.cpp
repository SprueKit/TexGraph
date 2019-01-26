#include "MeshOctree.h"

#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/Math/Triangle.h>
#include <SprueEngine/MathGeoLib/Geometry/LineSegment.h>

namespace SprueEngine
{

#define BOTTOM_LEFT_FRONT 0
#define BOTTOM_LEFT_BACK 1
#define BOTTOM_RIGHT_FRONT 2
#define BOTTOM_RIGHT_BACK 3
#define TOP_LEFT_FRONT 4
#define TOP_LEFT_BACK 5
#define TOP_RIGHT_FRONT 6
#define TOP_RIGHT_BACK 7

MeshOctree::MeshOctree(MeshOctreeConstructionData& meshData, unsigned char maxDepth) : 
    constructionData_(&meshData), 
    depth_(0),
    maxDepth_(maxDepth)
{
    for (unsigned i = 0; i < meshData.vertexCt; ++i)
    {
        if (i != 0)
            bounds_.Enclose(meshData.vertices[i]);
        else
            bounds_.SetFromCenterAndSize(meshData.vertices[i], Vec3(0, 0, 0));
    }

    for (unsigned i = 0; i < 8; ++i)
    {
        distances_[i] = FLT_MAX;
        unsignedDistances_[i] = FLT_MAX;
    }
    if (meshData.indices)
    {
        triangleMode_ = true;
        for (unsigned i = 0; i < meshData.indexCt; i += 3)
            Insert(std::make_pair(i, Vec3()));
    }
    else
    {
        triangleMode_ = false;
        for (unsigned i = 0; i < meshData.vertexCt; ++i)
            Insert(std::make_pair(i, meshData.normals[i]));
    }
}

void MeshOctree::Split()
{
    if (children_)
        return;

    const Vec3 halfBounds = bounds_.HalfSize();
    const Vec3 boundsMin = bounds_.minPoint;
    const Vec3 center = bounds_.Centroid();

    children_ = new MeshOctree[8];
    for (unsigned i = 0; i < 8; ++i)
    {
        BoundingBox childBounds(bounds_.minPoint, bounds_.maxPoint);
        if (i & 1)
            childBounds.minPoint.x = center.x;
        else
            childBounds.maxPoint.x = center.x;

        if (i & 2)
            childBounds.minPoint.y = center.y;
        else
            childBounds.maxPoint.y = center.y;

        if (i & 4)
            childBounds.minPoint.z = center.z;
        else
            childBounds.maxPoint.z = center.z;
            
        children_[i].bounds_ = childBounds;
        children_[i].depth_ = depth_ + 1;
        children_[i].triangleMode_ = triangleMode_;
        children_[i].constructionData_ = constructionData_;
        children_[i].maxDepth_ = maxDepth_;
    }
}

void MeshOctree::Collapse()
{
    if (children_ == 0x0)
        return;

    /// Move all child contents up to us
    for (unsigned i = 0; i < 8; ++i)
    {
        for (auto pair : children_[i].contents_)
        {
            /// Collapse the child if necessary
            children_[i].Collapse();
            contents_.push_back(pair);
        }
    }

    delete[] children_;
    children_ = 0x0;
}

bool MeshOctree::Insert(std::pair<unsigned, Vec3>& item)
{
    if (triangleMode_)
    {
        // Triangle require a bounding box for selection
        Triangle tri(
            constructionData_->vertices[constructionData_->indices[item.first]],
            constructionData_->vertices[constructionData_->indices[item.first + 1]],
            constructionData_->vertices[constructionData_->indices[item.first + 2]]);

        item.second = tri.GetNormal();
        return Insert(item, tri, tri.GetBounds());
    }
    else
    {
        // Vertices just get inserted at the lowest possible leaf
        if (bounds_.Contains(constructionData_->vertices[item.first]))
        {
            if (!children_ && depth_ < maxDepth_)
            {
                Split();
                for (unsigned i = 0; i < 8; ++i)
                    if (children_[i].Insert(item))
                        return true;
            }
            contents_.push_back(item);
            return true;
        }
        return false;
    }
}

bool MeshOctree::Insert(std::pair<unsigned, Vec3>& item, const Triangle& tri, const BoundingBox& bounds)
{
    if (bounds_.Contains(bounds))
    {
        // Check distances
        for (unsigned i = 0; i < 8; ++i)
            CheckVertex(i, tri);

        if (ShouldSplit(bounds))
        {
            if (!children_)
                Split();
            for (unsigned i = 0; i < 8; ++i)
            {
                if (children_->Insert(item, tri, bounds))
                    return true;
            }
        }
        contents_.push_back(item);
        return true;
    }
    return false;
}

void MeshOctree::CollectRaycast(const Ray& ray, std::vector<unsigned>& results, Vec3* withNormal, float minNor, float maxNor) const
{
    if (ray.Intersects(bounds_))
    {
        Collect(results, withNormal, minNor, maxNor);
        if (children_)
            for (unsigned i = 0; i < 8; ++i)
                children_[i].CollectRaycast(ray, results, withNormal, minNor, maxNor);
    }
}

void MeshOctree::CollectSphere(const Sphere& sphere, std::vector<unsigned>& results, Vec3* withNormal, float minNor, float maxNor) const
{
    if (sphere.Intersects(bounds_))
    {
        Collect(results, withNormal, minNor, maxNor);
        if (children_)
            for (unsigned i = 0; i < 8; ++i)
                children_[i].CollectSphere(sphere, results, withNormal, minNor, maxNor);
    }
}

void MeshOctree::CollectBox(const BoundingBox& bounds, std::vector<unsigned>& results, Vec3* withNormal, float minNor, float maxNor) const
{
    if (bounds_.Intersects(bounds))
    {
        Collect(results, withNormal, minNor, maxNor);
        if (children_)
            for (unsigned i = 0; i < 8; ++i)
                children_[i].CollectBox(bounds, results, withNormal, minNor, maxNor);
    }
}

void MeshOctree::CollectPoint(const Vec3& point, std::vector<unsigned>& results, Vec3* withNormal, float minNormalTolerance, float maxNormalTolerance) const
{
    if (bounds_.Contains(point))
    {
        Collect(results, withNormal, minNormalTolerance, maxNormalTolerance);
        if (children_)
            for (unsigned i = 0; i < 8; ++i)
                children_[i].CollectPoint(point, results, withNormal, minNormalTolerance, maxNormalTolerance);
    }
}

void MeshOctree::CollectLine(const Vec3& from, const Vec3& to, std::vector<unsigned>& results, unsigned* hitVertex) const
{
    LineSegment seg(from, to);
    if (bounds_.Intersects(seg))
    {
        Collect(results);
        if (children_)
            for (unsigned i = 0; i < 8; ++i)
                children_[i].CollectLine(from, to, results, hitVertex);
    }
}

bool MeshOctree::TraceLine(const Vec3& from, const Vec3& to, unsigned ignoreIndex, Vec3* hitPos)
{
    LineSegment seg(from, to);
    if (bounds_.Intersects(seg))
    {
        const Ray segRay = seg.ToRay();
        const float segLen = seg.Length();
        for (auto item : contents_)
        {
            unsigned indices[] = {
                constructionData_->indices[item.first],
                constructionData_->indices[item.first + 1],
                constructionData_->indices[item.first + 2]
            };

            if (indices[0] == ignoreIndex || indices[1] == ignoreIndex || indices[2] == ignoreIndex)
                continue;

            Triangle tri(
                constructionData_->vertices[indices[0]],
                constructionData_->vertices[indices[1]],
                constructionData_->vertices[indices[2]]);

            float dist = 0.0f;
            Vec3 baryJunk;
            if (tri.IntersectRayEitherSide(segRay, &dist, hitPos, &baryJunk))
            {
                if (dist < segLen)
                    return false;
            }
        }


        if (children_)
            for (unsigned i = 0; i < 8; ++i)
                if (!children_[i].TraceLine(from, to, ignoreIndex, hitPos))
                    return false;
    }

    return true;
}

void MeshOctree::Collect(std::vector<unsigned>& results, Vec3* withNormal, float minNor, float maxNor) const
{
    if (!withNormal)
    {
        results.reserve(results.size() + contents_.size());
        for (auto result : contents_)
            results.push_back(result.first);
    }
    else
    {
        for (auto result : contents_)
        {
            const float dot = withNormal->Dot(result.second);
            if (SprueBetweenInclusive(dot, minNor, maxNor))
                results.push_back(result.first);
        }
    }
}

bool MeshOctree::ShouldSplit(const BoundingBox& bounds) const
{
    if (!triangleMode_)
        return depth_ < maxDepth_;
    else if (depth_ < 5)
        return true;

    Vec3 size = bounds.Size();
    const Vec3 half = bounds_.HalfSize();
    if (depth_ >= maxDepth_)
        return false;
    if (!bounds_.Contains(bounds))
        return false;
    if (size.x >= half.x || size.y >= half.y || size.z >= half.z)
        return false;

    /*if (bounds.minPoint.x <= bounds_.Centroid().x - 0.5f * half.x ||
        bounds.maxPoint.x >= bounds_.Centroid().x + 0.5f * half.x ||
        bounds.minPoint.y <= bounds_.Centroid().y - 0.5f * half.y ||
        bounds.maxPoint.y >= bounds_.Centroid().y + 0.5f * half.y ||
        bounds.minPoint.z <= bounds_.Centroid().z - 0.5f * half.z ||
        bounds.maxPoint.z >= bounds_.Centroid().z + 0.5f * half.z)
        return false;*/

    return true;
}

float MeshOctree::SignedDistance(const Vec3& point, Vec3* foundNormal) const
{
    std::vector<unsigned> results;
    CollectPoint(point, results);
    
    // If we fail to collect any geometry then take the nearest lerp distance
    //if (results.empty())
    //{
    //    float value = FLT_MAX;
    //    NearestLerpedDistance(point, value);
    //    return value;
    //}

    float bestDistance = FLT_MAX;
    float nearestTriDistance = FLT_MAX; // nearest Sq Distance
    for (unsigned i = 0; i < results.size(); ++i)
    {
        Triangle tri(
            constructionData_->vertices[constructionData_->indices[i]],
            constructionData_->vertices[constructionData_->indices[i + 1]],
            constructionData_->vertices[constructionData_->indices[i + 2]]
        );

        // Don't want a signed distance right away, that'll prioritize distance faces we're on the backside
        // Want the nearest triangle to the point
        const float dist = tri.Distance2(point); //sqr dist, defer that sq root as long as possible
        if (dist < nearestTriDistance)
        {
            // Now that we're closer than anyone else sign the distance
            bestDistance = tri.SignDistance(point, sqrtf(dist));
            nearestTriDistance = dist;
            if (foundNormal)
                *foundNormal = tri.GetNormal();
        }
    }
    return bestDistance;
}

bool MeshOctree::NearestLerpedDistance(const Vec3& point, float& holder) const
{
    if (bounds_.Contains(point))
    {
        if (children_)
        {
            for (unsigned i = 0; i < 8; ++i)
            {
                if (children_[i].NearestLerpedDistance(point, holder))
                    return true;
            }
        }
        holder = LerpDistances(point);
        return true;
    }
    return false;
}

float MeshOctree::CalculateDensity(const Vec3& position) const
{
    return SignedDistance(position);
}

void MeshOctree::CheckVertex(unsigned vertIndex, const Triangle& tri)
{
    const Vec3 extents = bounds_.Size();
    Vec3 vert = bounds_.minPoint;

//  6 ___________ 7
//   /|         /|
// 2/_|________/3| 
// |  |        | |
// | 4|________|_| 5
// | /         | /
// |/__________|/
// 0           1

    if (vertIndex & 1)
        vert.x = extents.x;
    if (vertIndex & 2)
        vert.y = extents.y;
    if (vertIndex & 4)
        vert.z = extents.z;
    
    const float unsignedDist = tri.Distance2(vert);
    if (unsignedDist < unsignedDistances_[vertIndex])
    {
        distances_[vertIndex] = tri.SignDistance(vert, sqrtf(unsignedDist));
        unsignedDistances_[vertIndex] = unsignedDist;
    }
}

float MeshOctree::LerpDistances(const Vec3& point) const
{
    // differential coordinates
    const Vec3 nPos = NORMALIZE(point, bounds_.minPoint, bounds_.maxPoint);
    
    // front bottom edge
    const float c00 = distances_[0] * (1 - nPos.x) + distances_[1] * nPos.x;
    // back bottom edge
    const float c01 = distances_[4] * (1 - nPos.x) + distances_[5] * nPos.x;
    // front top edge
    const float c10 = distances_[2] * (1 - nPos.x) + distances_[3] * nPos.x;
    // back top edge
    const float c11 = distances_[6] * (1 - nPos.x) + distances_[7] * nPos.x;

    // both front edges
    const float c0 = c00 * (1.0f - nPos.y) + c10 * nPos.y;
    // both back edges
    const float c1 = c01 * (1.0f - nPos.y) + c11 * nPos.y;

    // between front and back
    return c0 * (1 - nPos.z) + c1 * nPos.z;
}

bool MeshOctree::TestLine(const Vec3& from, const Vec3& to, const std::vector<unsigned>& results, unsigned ignoreIndex, Vec3* hitPos)
{
    LineSegment seg(from, to);
    const float segLength = seg.Length();
    Ray testRay(seg);

    for (unsigned i = 0; i < results.size(); ++i)
    {
        unsigned indices[] = {
            constructionData_->indices[results[i]],
            constructionData_->indices[results[i]+1],
            constructionData_->indices[results[i]+2]
        };

        if (indices[0] == ignoreIndex || indices[1] == ignoreIndex || indices[2] == ignoreIndex)
            continue;

        Triangle tri(
            constructionData_->vertices[indices[0]], 
            constructionData_->vertices[indices[1]], 
            constructionData_->vertices[indices[2]]);

        float dist = 0.0f;
        Vec3 baryJunk;
        if (tri.IntersectRayEitherSide(testRay, &dist, hitPos, &baryJunk))
        {
            if (dist < segLength)
                return false;
        }
    }

    return true;
}

}