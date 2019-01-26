#pragma once

#include <qapplication.h>

#include <map>

#include <Urho3D/Math/Vector2.h>

namespace SprueEditor
{

/// Tracks key inputs to keep states of up/down
class InputRecorder
{
public:
    void SetWheel(int delta) { mouseWheel_ = delta; }
    int GetWheel() const { return mouseWheel_; }

    void SetKeyDown(int code) { SetKey(code, true); }
    void SetKeyUp(int code) { SetKey(code, false); }
    void SetKey(int code, bool down);

    void SetMouseDown(int button) { SetMouse(button, true); }
    void SetMouseUp(int button) { SetMouse(button, false); }
    void SetMouse(int button, bool down);

    /// Test only whether a key is currently down
    bool IsKeyDown(int code) const;

    bool IsMousePressed(int button) const;

    bool WasMousePressed(int button) const;

    /// Test whether a key was pressed during the frame
    bool IsKeyPressed(int code) const;
    bool IsKeyNotPressed(int code) const;

    // Clear the down history
    void UpdateFrame();

    void SetMousePos(Urho3D::IntVector2 pos, bool isNew, bool noCalcDelta = false);

    Urho3D::IntVector2 GetMouseDelta() const { return mouseDelta_; }

    Urho3D::IntVector2 GetMousePos() const { return lastMousePos_; }

    bool IsShiftDown() const;
    bool IsControlDown() const;
    bool IsAltDown() const;
    bool IsArrowDown(int arrow) const;

    bool IsKeySequenceActive(const QKeySequence& sequence) const;

    void SetMouseLocked(bool mouseLocked) { mouseLocked_ = mouseLocked; }
    bool IsMouseLocked() const { return mouseLocked_; }

private:
    /// Need to remap Qt codes to OS codes for key state checking.
    int RemapKey(int key) const;

    std::map<int, bool> keyDown_;
    std::map<int, bool> previousMouseDown_;
    std::map<int, bool> keyPressed_;
    std::map<int, bool> mousePressed_;
    int mouseWheel_;
    bool mouseLocked_ = true;
    Urho3D::IntVector2 mouseDelta_;
    Urho3D::IntVector2 lastMousePos_;
};

}