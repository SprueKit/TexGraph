#pragma once

#include <memory>
#include <string>
#include <vector>

namespace SprueEditor
{

class TimelineTrackModel;

class TimelineKeyModel
{
public:
    virtual ~TimelineKeyModel() { }

    virtual float GetKeyTime() const = 0;
    virtual void SetKeyTime(float time) = 0;

    // Functions for "spans", a keyframe occurs at precise point in time, a "span" has a start and an end
    virtual bool IsSpan() const { return false; }
    virtual float GetKeySpan() const { return 0.0f; }
    virtual void SetSpan(float length) { }
};

/// Represents a singular track in the timeline
class TimelineTrackModel
{
public:
    virtual ~TimelineTrackModel() { }
    virtual std::string GetName() = 0;

    /// Implement for drag and drop support between tracks
    virtual bool AcceptsKey(TimelineKeyModel* key) const { return false; }

    bool Expanded;
    /// List of expandable child tracks, if necessary
    std::vector< std::shared_ptr<TimelineTrackModel> > ChildTracks;
    /// List of keyframes in this track
    std::vector<std::shared_ptr<TimelineKeyModel> > KeyFrames;
};

/// Baseclass for implementing the model to interact with the timeline controls
class TimelineModelBase
{
public:
    virtual ~TimelineModelBase() { }

    virtual unsigned TrackCount() const = 0;
    virtual unsigned KeyframeCount() const = 0;
    virtual float GetLength() const = 0;
    /// If the retuned value is true than precise (time instead of xied frame) keying is used.
    virtual bool UsesPreciseKeying() const { return false; }

    std::vector< std::shared_ptr<TimelineTrackModel> > Tracks;
};

}