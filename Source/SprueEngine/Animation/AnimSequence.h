#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/IEditable.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{
    class Keyframe
    {
        NOCOPYDEF(Keyframe);
    public:
        /// Interpolation is decided on a "first opt-out" basis, if either side of a transition is "Hard" then the transition is hard
        bool HardEntry; // The previous frame is not allowed to interpolate into this one
        bool HardExit;  // Is not allowed to interpolate into the next frame
    };

    class TransformKeyframe : Keyframe
    {
        NOCOPYDEF(TransformKeyframe);
        BASECLASSDEF(TransformKeyframe, Keyframe);
    public:
        Vec3 Position;
        Quat Rotation;
        Vec3 Scale;
    };

    class IKKeyframe : Keyframe
    {
        NOCOPYDEF(IKKeyframe);
        BASECLASSDEF(IKKeyframe, Keyframe);
    public:
        /// Name of the effector
        std::string Effector;
        /// Name of the chain
        std::string Chain;
        /// Position of the effector
        Vec3 Position;
        /// Plane on which the IK chains should be clamped
        Plane ClampPlane;
    };

    class AnimTrack
    {
        NOCOPYDEF(AnimTrack);
    public:

        std::vector<Keyframe*>& getKeyframes() { return keyframes_; }
        const std::vector<Keyframe*>& getKeyframes() const { return keyframes_; }

    private:
        std::vector<Keyframe*> keyframes_;
    };

    class IKTrack : AnimTrack
    {
        NOCOPYDEF(IKTrack);
        BASECLASSDEF(IKTrack, AnimTrack);
    public:

    };

    class AnimSequence : public IEditable
    {
        SPRUE_EDITABLE(AnimSequence);
    public:
        AnimSequence();
        virtual ~AnimSequence();

        static void Register(Context* context);

        std::string GetName() const { return name_; }
        void SetName(const std::string& name) { name_ = name; }

        unsigned GetFlags() const { return flags_; }
        void SetFlags(unsigned flags) { flags_ = flags; }

        std::vector<AnimTrack*>& GetTracks() { return tracks_; }
        const std::vector<AnimTrack*>& GetTracks() const { return tracks_; }

        float GetDuration() const { return duration_; }
        void SetDuration(float value) { duration_ = value; }

    private:
        std::vector<AnimTrack*> tracks_;
        std::string name_;
        unsigned flags_ = 0;
        float duration_ = 0.0f;
        bool loops_ = false;
    };

}