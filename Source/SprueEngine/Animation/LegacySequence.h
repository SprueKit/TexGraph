#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{

    /// Keyframe in a legacy style animation. Comes from files supporting animation (FBX).
    struct SPRUE LegacyKeyframe
    {
        Vec3 position_;
        Quat rotation_;
        Vec3 scale_;

        bool IsCloseEnough(const LegacyKeyframe& rhs, float positionTolerance, float rotationTolerance, float scaleTolerance);
    };

    /// A container for LegacyKeyframes. Contains the animation data for a specific joint.
    struct SPRUE LegacyTimeline
    {
        int jointIndex_;
        std::vector<LegacyKeyframe> frames_;
    };

    /// An explicit animation sequence. Legacy animations are loaded from FBX files and their animations can be used in animation generation.
    /// A container for LegacyTimelines.
    class SPRUE LegacySequence
    {
    public:
        LegacySequence();
        virtual ~LegacySequence();

        std::vector<LegacyTimeline>& GetTimelines() { return timelines_; }
        const std::vector<LegacyTimeline>& GetTimelines() const { return timelines_; }

        std::string GetName() const { return name_; }
        void SetName(const std::string& name) { name_ = name; }

    protected:
        std::string name_;
        std::vector<LegacyTimeline> timelines_;
    };

}