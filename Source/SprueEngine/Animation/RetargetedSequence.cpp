#include "RetargetedSequence.h"

#include <SprueEngine/Animation/LegacySequence.h>

#include <SprueEngine/Core/SprueModel.h>
#include <SprueEngine/Core/Bone.h>

namespace SprueEngine
{

    bool RetargetedTimeline::Overlaps(const RetargetedTimeline& rhs) const
    {
        if (SelectorsOverlap(rhs))
            return TimeOverlaps(rhs);
        return false;
    }

    bool RetargetedTimeline::TimeOverlaps(const RetargetedTimeline& rhs) const
    {
        if (!frames_.empty() && !rhs.frames_.empty())
        {
            std::pair<float, float> left = std::pair<float,float>(frames_.front().time_, frames_.back().time_);
            std::pair<float, float> right = std::pair<float, float>(rhs.frames_.front().time_, rhs.frames_.back().time_);

            if (SprueOverlap(left.first, left.second, right.first, right.second))
                return true;
        }
        return false;
    }

    bool RetargetedTimeline::SelectorsOverlap(const RetargetedTimeline& rhs) const
    {
        int failCts = 0;
        failCts += selector_.MorphologySelection & rhs.selector_.MorphologySelection;
        failCts += selector_.AllowedRegions & rhs.selector_.AllowedRegions;
        failCts += selector_.ExcludeRegions & rhs.selector_.ExcludeRegions;
        failCts += selector_.RequireRegions & rhs.selector_.RequireRegions;

        unsigned partCaps = selector_.PartCaps & rhs.selector_.PartCaps;
        unsigned partFlags = selector_.PartFlags & rhs.selector_.PartFlags;
        if (failCts > 0)
            return partCaps || partFlags;
        return false;
    }

    LegacyAnimation* RetargetedSequence::RetargetAnimation()
    {
        return 0x0;
    }

    FABRIKRig* RetargetedSequence::ConstructRig(SprueModel* model)
    {
        return 0x0;
    }
    
    void RetargetedSequence::SetupRig(FABRIKRig* rig, float time, bool isNormalizedTime)
    {

    }
    
    float RetargetedSequence::GetLength() const
    {
        return 0.0f;
    }

    bool RetargetedSequence::ValidFor(SprueModel* model, int* permutations)
    {
        bool valid = true;
        int permutationCt = 1;
        
        std::vector<Bone*> bones = model->GetChildrenOfType<Bone>(true);
        for (auto& timeline : timelines_)
        {
            // Only care about valid timelines.
            if (timeline.frames_.empty())
                continue;

            int validCt = 0;
            for (auto bone : bones)
                if (timeline.selector_.IsSelectorValid(model, bone, 0x0))
                    ++validCt;
            valid &= validCt ? true : false;

            // Accumulate the permutation count, this may not be precise.
            if (validCt)
                permutationCt = permutationCt * validCt;
        }
        if (permutations)
            *permutations = permutationCt;

        return valid;
    }
}