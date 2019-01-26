#pragma once

#include <EditorLib/editorlib_global.h>

#include <vector>

/// Base-type for a UIEvent to dispatch.
struct EDITORLIB_EXPORT UIEvent
{
    unsigned eventID_;
    void* source_ = 0x0;

    /// Construct and specify the ID.
    UIEvent(unsigned id, void* src = 0x0) : eventID_(id), source_(0x0) { }

    unsigned GetID() { return eventID_; }
};

/// For event receivers/static-board data.
/// ALL UIEventBoard instances will be have Process() called for all events.
/// Purpose:
///     Although QT supports user events through QEvent the use of QEvent still
/// requires a tight coupling as it's necessary to know who to send the QEvent
/// for it to "bubble up" from.
///     "Trickle down" dispatch could be brute-forced but would not be ideal, 
/// as it would require a full traversal of the Widget tree.
///     Instead UIEventBoard maintains a list of those objects/widgets that need
/// to know about both extraordinary situations and things for which any written
/// coupling is not tenable (in that coupling combinations would explode).
///     Being brain-dead is deliberate.
class EDITORLIB_EXPORT UIEventBoard
{
    static std::vector<UIEventBoard*> receivers_;
    static std::vector<UIEvent*> events_;
protected:
    /// Construct.
    UIEventBoard();
    /// Destruct.
    virtual ~UIEventBoard();

public:
    /// Call to execute the processing of all events.
    static void ProcessEvents();

    /// Push an event to the queue for processing.
    static void PushEvent(UIEvent* event);

    /// Will be called on the instance to check for processing an event.
    virtual void ProcessEvent(UIEvent* event) { }
};