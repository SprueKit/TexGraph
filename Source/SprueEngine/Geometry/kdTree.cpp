#include "kdTree.h"

#include <SprueEngine/Math/Triangle.h>

namespace SprueEngine
{

    kdTree::kdTree(kdTreeConstructionData& data) :
        isRoot_(true),
        data_(data)
    {
        BoundingBox bb;
        for (unsigned i = 0; i < data.positionBufferLength_; ++i)
            if (i != 0)
                bb.Enclose(data.positionBuffer_[i]);
            else
                bb.minPoint = bb.maxPoint = data.positionBuffer_[i];
        Build(data, bb, data.shortForm_, 0);
    }

    kdTree::kdTree(kdTreeConstructionData& data, const BoundingBox& bounds, std::vector<uint32_t> selectedIndices, int depth) :
        data_(data),
        bounds_(bounds),
        isRoot_(false)
    {
        Build(data, bounds, selectedIndices, depth);
    }

    kdTree::~kdTree()
    {
        if (left_)
            delete left_;
        if (right_)
            delete right_;
        left_ = right_ = 0x0;
    }

    void kdTree::Build(kdTreeConstructionData& data, const BoundingBox& bounds, std::vector<uint32_t> shortForm, int depth)
    {
        bounds_ = bounds;
        contents_ = shortForm;

        if (contents_.size() == 0)
            return;

        if (contents_.size() == 1)
            return;

        const int maxAxis = bounds_.Size().MaxElementIndex();
        BoundingBox leftBounds = bounds;
        BoundingBox rightBounds = bounds;
        switch (maxAxis)
        {
        case 0:
            leftBounds.maxPoint.x = bounds.CenterPoint().x;
            rightBounds.minPoint.x = bounds.CenterPoint().x;
            break;
        case 1:
            leftBounds.maxPoint.y = bounds.CenterPoint().y;
            rightBounds.minPoint.y = bounds.CenterPoint().y;
            break;
        case 2:
            leftBounds.maxPoint.z = bounds.CenterPoint().z;
            rightBounds.minPoint.z = bounds.CenterPoint().z;
            break;
        }

        std::vector<uint32_t> leftList, rightList;
        for (unsigned i = 0; i < shortForm.size(); ++i)
        {
            const int indices[] = {
                data_.indexBuffer_[shortForm[i]],
                data_.indexBuffer_[shortForm[i] + 1],
                data_.indexBuffer_[shortForm[i] + 2],
            };
            auto tri = math::Triangle(data_.positionBuffer_[indices[0]], data_.positionBuffer_[indices[1]], data_.positionBuffer_[indices[2]]);
            if (leftBounds.Contains(tri))
                leftList.push_back(i);
            if (rightBounds.Contains(tri))
                rightList.push_back(i);
        }

        int matches = 0;
        for (int i = 0; i < leftList.size(); ++i)
        {
            for (int j = 0; j < rightList.size(); ++j)
            {
                if (leftList[i] == rightList[j])
                    matches++;
            }
        }

        if (depth < 8 || ((float)matches / leftList.size() < 0.5f && (float)matches / rightList.size() < 0.5f))
        {
            left_ = new kdTree(data, leftBounds, leftList, depth + 1);
            right_ = new kdTree(data, rightBounds, rightList, depth + 1);
        }
    }

    bool kdTree::Hit(const kdTree* tree, const LineSegment& ray, uint32_t testIndex) const
    {
        if (bounds_.Intersects(ray))
        {
            if ((left_ != 0x0 && right_ != 0x0) && (left_->contents_.size() || right_->contents_.size()))
            {
                bool hitLeft = left_->Hit(ray, testIndex);
                bool hitRight = right_->Hit(ray, testIndex);
                return hitLeft || hitRight;
            }
            if (!contents_.empty())
            {
                auto asRay = ray.ToRay();
                for (const auto& index : contents_)
                {
                    const int indices[] = {
                        data_.indexBuffer_[index],
                        data_.indexBuffer_[index + 1],
                        data_.indexBuffer_[index + 2],
                    };

                    if (testIndex != -1 && (indices[0] == testIndex || indices[1] == testIndex || indices[2] == testIndex))
                        return false;

                    float dist = FLT_MAX;
                    if (SprueEngine::Triangle(data_.positionBuffer_[indices[0]], data_.positionBuffer_[indices[1]], data_.positionBuffer_[indices[2]]).IntersectRayEitherSide(asRay, &dist))
                    {
                        if (dist < ray.Length())
                            return true;
                    }
                }
            }
        }
        return false;
    }

    int kdTree::Winding(const kdTree* tree, const Ray& ray, int currentWinding) const
    {
        int winding = currentWinding;
        if (bounds_.Intersects(ray))
        {
            if ((left_ != 0x0 && right_ != 0x0) && (left_->contents_.size() || right_->contents_.size()))
            {
                winding = left_->Winding(left_, ray, winding);
                winding = right_->Winding(right_, ray, winding);
            }
            if (!contents_.empty())
            {
                for (const auto& index : contents_)
                {
                    const int indices[] = {
                        data_.indexBuffer_[index],
                        data_.indexBuffer_[index + 1],
                        data_.indexBuffer_[index + 2],
                    };

                    float dist = FLT_MAX;
                    math::Triangle t(data_.positionBuffer_[indices[0]], data_.positionBuffer_[indices[1]], data_.positionBuffer_[indices[2]]);
                    if (t.Intersects(ray))
                    {
                        if (t.NormalCCW().Dot(ray.dir) > 0.0f)
                            ++winding;
                        else
                            --winding;
                    }
                }
            }
        }
        return winding;
    }
}