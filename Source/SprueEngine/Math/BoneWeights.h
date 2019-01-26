#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Math/IntVec.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <memory>
#include <stdint.h>

namespace SprueEngine
{
    /// Handles the accumulation of bone weights. Stronger weights override the weaker weights.
    struct SPRUE BoneWeights
    {
        IntVec4 indices_;
        math::float4 weights_;

        /// Adds a weighted bone to the set. The lowest weight is overwritten if it is less than the incoming weight.
        bool AddBoneWeight(int index, float weight)
        {
            int lowestIndex = weights_.MinElementIndex();
            if (weights_[lowestIndex] < weight)
            {
                weights_[lowestIndex] = weight;
                indices_[lowestIndex] = index;
                return true;
            }
            return false;
        }

        void Normalize()
        {
            weights_.Normalize();
        }
    };
}