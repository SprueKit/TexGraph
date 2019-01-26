#pragma once

#include <SprueEngine/Animation/SpaceRemapping.h>
#include <SprueEngine/Selector.h>

#include <vector>

namespace SprueEngine
{
    class FABRIKRig;
    class LegacyAnimation;
    class SprueModel;

    struct SPRUE RetargetedKeyframe
    {
        SpacialPosition generalizedSpace_;
        float time_;
    };

    struct SPRUE RetargetedTimeline
    {
        Selector selector_;
        std::vector<RetargetedKeyframe> frames_;

        bool Overlaps(const RetargetedTimeline& rhs) const;
        /// Returns true if the two timelines overlap.
        bool TimeOverlaps(const RetargetedTimeline& rhs) const;

        bool SelectorsOverlap(const RetargetedTimeline& rhs) const;
    };

    class SPRUE RetargetedSequence
    {
    public:
        RetargetedSequence();
        virtual ~RetargetedSequence();

        std::vector<RetargetedTimeline>& GetTimelines() { return timelines_; }
        const std::vector<RetargetedTimeline>& GetTimelines() const { return timelines_; }

        std::string GetName() const { return name_; }
        void SetName(const std::string& name) { name_ = name; }

        /// Regenerates this animation for the target object.
        LegacyAnimation* RetargetAnimation();

        /// Generates an appropriate fabrik rib.
        FABRIKRig* ConstructRig(SprueModel* model);
        /// Sets up the given 
        void SetupRig(FABRIKRig* rig, float time, bool isNormalizedTime = false);
        /// Returns the length in time of the animation.
        float GetLength() const;

        bool ValidFor(SprueModel* rig, int* permutations = 0x0);

    private:
        std::vector<RetargetedTimeline> timelines_;
        std::string name_;
    };
}