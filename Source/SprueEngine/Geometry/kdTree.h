#pragma once

#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/MathGeoLib/Geometry/LineSegment.h>

#include <vector>

namespace SprueEngine
{

    struct kdTreeConstructionData
    {
        Vec3* positionBuffer_;
        unsigned positionBufferLength_;
        uint32_t* indexBuffer_;
        unsigned indexBufferLength_;
        std::vector<uint32_t> shortForm_;

        void Pack() {
            shortForm_.reserve(indexBufferLength_ / 3);
            for (int i = 0; i < indexBufferLength_ / 3; ++i)
                shortForm_.push_back(indexBuffer_[i/3]);
        }
    };

    struct kdTree
    {
        BoundingBox bounds_;
        std::vector<uint32_t> contents_;
        bool isRoot_ = false;
        kdTree* left_ = 0x0;
        kdTree* right_ = 0x0;
        kdTreeConstructionData& data_;

        kdTree(kdTreeConstructionData& data);
        kdTree(kdTreeConstructionData& data, const BoundingBox& bounds, std::vector<uint32_t> selectedIndices, int depth);
        ~kdTree();

        void Build(kdTreeConstructionData& data, const BoundingBox& bounds, std::vector<uint32_t> shortForm, int depth);

        inline bool kdTree::Hit(const LineSegment& ray, uint32_t testIdex = -1) const 
        {
            return Hit(this, ray, testIdex);
        }

        inline int kdTree::Winding(const Ray& ray) const
        {
            return Winding(this, ray, 0);
        }

        bool Hit(const kdTree* tree, const LineSegment& ray, uint32_t testIndex = -1) const;
        int Winding(const kdTree* tree, const Ray& ray, int currentWinding) const;
    };

}