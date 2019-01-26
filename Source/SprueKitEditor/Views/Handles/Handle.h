#pragma once

namespace Urho3D
{
    class DebugRenderer;
}

struct Handle
{
    bool HasFocus() const { return hasFocus_; }
    /// Return true if we have highlight or focus.
    bool HasHighlight() const { return hasHighlight_ || hasFocus_; }
    unsigned GetBitmask() const { return bitMask_; }
    float GetScaleFactor() const { return scaleFactor_; }
    void SetScaleFactor(float value) { scaleFactor_ = value; }

    /// Responsible for painting the handle to an Urho3D debug renderer
    virtual void PaintHandle(Urho3D::DebugRenderer* renderer, bool forceHighlight = false) = 0;

    /// Return bit flags for other handles that we require to have highlighted if we're "highlighted" (used for XYZ combo handles)
    virtual unsigned HighlightMask() { return 0; }
    /// Respond to mouse movement, use to test for rays such as highlight.
    virtual void MouseMoved(int x, int y) = 0;
    /// Check for mouse interactivity, return true if capture is desired.
    virtual bool MousePressed(int x, int y, int button) = 0;
    /// Respond to mouse movement while actively capturing the mouse.
    virtual void MouseDragged(int x, int y, float deltaX, float deltaY, int button) = 0;
    /// Terminate capture
    virtual void MouseReleased(int x, int y, int button) = 0;

protected:
    /// Tweaks the scaling of the rendered elements
    float scaleFactor_ = 1.0f;
    /// Handle may be marked with a bitmask so that other handles can trigger a highlight of it
    unsigned bitMask_ = 0;
    /// True if we have taken focus during MousePressed
    bool hasFocus_ = false;
    /// True if we have achieved highlight during MouseMoved
    bool hasHighlight_ = false;
};