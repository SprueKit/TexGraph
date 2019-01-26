#pragma once

#include "TimelineModelBase.h"

#include <Urho3D/Scene/Animatable.h>
#include <Urho3D/Scene/ObjectAnimation.h>
#include <Urho3D/Scene/ValueAnimation.h>
#include <Urho3D/Scene/ValueAnimationInfo.h>

namespace UrhoEditor
{
    class AnimatableTimelineTrackModel;

    /// Keyframe for an Value animation, Keys are ephemeral
    class AnimatableTimelineKey : public SprueEditor::TimelineKeyModel
    {
    public:
        AnimatableTimelineKey(AnimatableTimelineTrackModel* owner, Urho3D::VAnimKeyFrame* keyframe);

        virtual float GetKeyTime() const override;
        virtual void SetKeyTime(float time) override;

        Urho3D::VAnimKeyFrame* keyFrame_;
        AnimatableTimelineTrackModel* owner_;
        /// Timelines restricted to a single sensible type
        Urho3D::VariantType dataType_;
    };

    class AnimatableEventTimelineKey : public SprueEditor::TimelineKeyModel
    {
    public:
        AnimatableEventTimelineKey(AnimatableTimelineTrackModel* owner, Urho3D::VAnimEventFrame* keyframe);

        virtual float GetKeyTime() const override;
        virtual void SetKeyTime(float time) override;

        Urho3D::VAnimEventFrame* keyFrame_;
        AnimatableTimelineTrackModel* owner_;
    };

    /// Track (Value animation) for an object animation, tracks are not ephemeral, track is responsible for its keyframes
    class AnimatableTimelineTrackModel : public SprueEditor::TimelineTrackModel
    {
    public:
        AnimatableTimelineTrackModel(Urho3D::SharedPtr<Urho3D::ValueAnimation> valueAnim);

        virtual std::string GetName() override;

        void RefreshKeyFrames();

        void InvalidateSpline();

        Urho3D::SharedPtr<Urho3D::ValueAnimation> valueAnimation_;
    };

    class AnimatableTimelineEventTrackModel : public SprueEditor::TimelineTrackModel
    {
    public:
        AnimatableTimelineEventTrackModel();

        void RefreshKeyFrames();

        virtual std::string GetName() { return "Events"; }
    };

    /// Object/Value animation model, Responsible for track construction
    class AnimatableTimelineModel : public SprueEditor::TimelineModelBase
    {
    public:
        AnimatableTimelineModel(Urho3D::SharedPtr<Urho3D::ObjectAnimation> objectAnim);
        AnimatableTimelineModel(Urho3D::SharedPtr<Urho3D::ValueAnimation> valueAnim);
        AnimatableTimelineModel(Urho3D::SharedPtr<Urho3D::Animatable> object);

        virtual unsigned TrackCount() const;
        virtual unsigned KeyframeCount() const;
        virtual float GetLength() const;

    private:
        Urho3D::SharedPtr<Urho3D::ObjectAnimation> objectAnimation_;
        Urho3D::SharedPtr<Urho3D::ValueAnimation> valueAnimation_;
        Urho3D::SharedPtr<Urho3D::Animatable> animated_;
    };
}