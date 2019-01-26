#include "UrhoAnimatableTimelineModel.h"



namespace UrhoEditor
{

    AnimatableTimelineKey::AnimatableTimelineKey(AnimatableTimelineTrackModel* owner, Urho3D::VAnimKeyFrame* keyframe) :
        owner_(owner),
        keyFrame_(keyframe)
    {

    }

    float AnimatableTimelineKey::GetKeyTime() const
    {
        if (keyFrame_)
            return keyFrame_->time_;
        return 0;
    }

    void AnimatableTimelineKey::SetKeyTime(float time)
    {
        if (keyFrame_)
        {
            keyFrame_->time_ = time;
            owner_->InvalidateSpline();
        }
    }

    AnimatableEventTimelineKey::AnimatableEventTimelineKey(AnimatableTimelineTrackModel* owner, Urho3D::VAnimEventFrame* keyframe) :
        keyFrame_(keyframe),
        owner_(owner)
    {

    }

    float AnimatableEventTimelineKey::GetKeyTime() const
    {
        if (keyFrame_->time_)
            return keyFrame_->time_;
        return 0.0f;
    }

    void AnimatableEventTimelineKey::SetKeyTime(float time)
    {
        if (keyFrame_)
            keyFrame_->time_ = time;
    }

    AnimatableTimelineTrackModel::AnimatableTimelineTrackModel(Urho3D::SharedPtr<Urho3D::ValueAnimation> valueAnim) :
        valueAnimation_(valueAnim)
    {
        RefreshKeyFrames();
    }

    std::string AnimatableTimelineTrackModel::GetName()
    {
        return valueAnimation_->GetName().CString();
    }

    void AnimatableTimelineTrackModel::RefreshKeyFrames()
    {
        KeyFrames.clear();
        if (valueAnimation_)
        {
            auto frames = valueAnimation_->GetKeyFrames();
            for (auto key = frames.Begin(); key!= frames.End(); ++key)
                KeyFrames.push_back(std::shared_ptr<SprueEditor::TimelineKeyModel>(new AnimatableTimelineKey(this, &(*key))));
        }
    }

    void AnimatableTimelineTrackModel::InvalidateSpline()
    {
        // Note: as 10/20/2016, Setting the spline tension to its current value forces the spline to be marked dirty for rebuilding.
        // Actual rebuild will be done when an interpolated value is requested
        if (valueAnimation_)
            valueAnimation_->SetSplineTension(valueAnimation_->GetSplineTension());
    }

    AnimatableTimelineEventTrackModel::AnimatableTimelineEventTrackModel()
    {

    }

    void AnimatableTimelineEventTrackModel::RefreshKeyFrames()
    {

    }

    AnimatableTimelineModel::AnimatableTimelineModel(Urho3D::SharedPtr<Urho3D::ObjectAnimation> objectAnim) :
        objectAnimation_(objectAnim)
    {
        auto infos = objectAnimation_->GetAttributeAnimationInfos();
        for (auto info : infos)
            Tracks.push_back(std::shared_ptr<SprueEditor::TimelineTrackModel>(new AnimatableTimelineTrackModel(Urho3D::SharedPtr<Urho3D::ValueAnimation>(info.second_->GetAnimation()))));
    }

    AnimatableTimelineModel::AnimatableTimelineModel(Urho3D::SharedPtr<Urho3D::ValueAnimation> valueAnim) :
        valueAnimation_(valueAnim)
    {
    }

    AnimatableTimelineModel::AnimatableTimelineModel(Urho3D::SharedPtr<Urho3D::Animatable> object) :
        animated_(object)
    {

    }

    unsigned AnimatableTimelineModel::TrackCount() const
    {
        if (objectAnimation_)
            return objectAnimation_->GetAttributeAnimationInfos().Size();
        else if (valueAnimation_)
            return 1;
        return 0;
    }

    unsigned AnimatableTimelineModel::KeyframeCount() const
    {
        // Return negative one to tell the timeline that we're an arbitrary layout, not a fixed grid
        return -1;
    }

    float AnimatableTimelineModel::GetLength() const
    {
        float maxVal = 0.0f;
        if (objectAnimation_)
        {
            // Find the maximum end time of any animation
            auto valueAnims = objectAnimation_->GetAttributeAnimationInfos();
            for (auto valueAnim : valueAnims)
                maxVal = fmaxf(maxVal, valueAnim.second_->GetAnimation()->GetEndTime());
        }
        else if (valueAnimation_)
            return valueAnimation_->GetEndTime();
        return maxVal;
    }
}