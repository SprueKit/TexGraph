#pragma once

#include <SprueKitEditor/Views/Handles/Handle.h>

#include <vector>

/// A HandleSystem is a collection of handles for manipulating an object.
/// HandleSystem simplifies the construction and handling of arbitrary manipulators
/// A "combo-gizmo" displaying Translate + Rotate + Scale is equally as viable
///     as a Axis-Angle gizmo
struct HandleSystem
{
    /// Construct empty.
    HandleSystem();
    /// Construct with a set of handles.
    HandleSystem(const std::vector<Handle*>& handles);
    /// Releases the contained handles.
    ~HandleSystem();

    /// Respond to mouse movement, use to test for rays such as highlight.
    void MouseMoved(int x, int y);
    /// Check for mouse interactivity, return true if capture is desired.
    bool MousePressed(int x, int y, int button);
    /// Respond to mouse movement while actively capturing the mouse.
    void MouseDragged(int x, int y, float deltaX, float deltaY, int button);
    /// Terminate capture
    void MouseReleased(int x, int y, int button);
    /// Render the handles
    void Paint(Urho3D::DebugRenderer* renderer);


    std::vector<Handle*>& GetHandles() { return handles_; }
    const std::vector<Handle*>& GetHandles() const { return handles_; }

private:
    std::vector<Handle*> handles_;
};