#pragma once

#include <numeric>

namespace SprueEngine
{

    template<typename INDEX_T>
    struct NearestIndex
    {
        float closestDistance_;
        INDEX_T closest_;

        NearestIndex(INDEX_T defaultValue)
        {
            closest_ = defaultValue;
            closestDistance_ = std::numeric_limits<float>::max();
        }

        bool Check(float dist, INDEX_T index)
        {
            if (dist < closestDistance_)
            {
                closestDistance_ = dist;
                closest_ = index;
                return true;
            }
            return false;
        }
    };

}