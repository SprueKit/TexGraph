#include "InputRecorder.h"

#include <QKeySequence>

#ifdef WIN32
    #include <Windows.h>
#endif

namespace SprueEditor
{

void InputRecorder::SetKey(int code, bool down)
{
    keyDown_[code] = down;
    if (down)
        keyPressed_[code] = true;
}

void InputRecorder::SetMouse(int button, bool down)
{
    mousePressed_[button] = down;
}

/// Test only whether a key is currently down
bool InputRecorder::IsKeyDown(int code) const 
{
    auto found = keyDown_.find(code);
    if (found != keyDown_.end())
        return found->second && ::GetAsyncKeyState(code) < 0;
    return false;
}

bool InputRecorder::IsMousePressed(int button) const 
{
    auto found = mousePressed_.find(button);
    if (found != mousePressed_.end() && found->second)
        return true;
    return false;
}

bool InputRecorder::WasMousePressed(int button) const 
{
    // Button was previously down?
    auto foundPrev = previousMouseDown_.find(button);
    if (foundPrev != previousMouseDown_.end() && foundPrev->second)
    {
        // Button is no longer down?
        auto current = mousePressed_.find(button);
        if (current == mousePressed_.end() || !current->second)
            return true;
    }
    return false;
}

bool InputRecorder::IsKeyPressed(int code) const 
{
    auto found = keyDown_.find(code);
    // Need to ask the OS, as mouse dragging results in Qt Key events being incredibly rare
    if (found != keyDown_.end() && found->second && ::GetAsyncKeyState(RemapKey(code)) < 0)
        return true;
    //found = keyPressed_.find(code);
    //if (found != keyPressed_.end() && found->second)
    //    return true;
    return false;
}
bool InputRecorder::IsKeyNotPressed(int code) const 
{
    auto foundKey = keyDown_.find(code);
    auto foundPressed = keyPressed_.find(code);
    if (foundKey == keyDown_.end() && foundPressed == keyPressed_.end())
        return true;
    return false;
}

// Clear the down history
void InputRecorder::UpdateFrame()
{
    keyPressed_.clear();
    previousMouseDown_ = mousePressed_;
    mouseWheel_ = 0;
    mouseDelta_ = Urho3D::IntVector2(0, 0);
}

void InputRecorder::SetMousePos(Urho3D::IntVector2 pos, bool isNew, bool noCalcDelta) 
{
    if (isNew)
    {
        mouseDelta_ = Urho3D::IntVector2(0, 0);
        lastMousePos_ = pos;
    }
    else
    {
        if (!noCalcDelta)
            mouseDelta_ = pos - lastMousePos_;
        lastMousePos_ = pos;
    }
}

bool InputRecorder::IsShiftDown() const 
{
    //if (QApplication::keyboardModifiers() & Qt::KeyboardModifier::ShiftModifier)
    //    return true;
#ifdef WIN32
    if (GetAsyncKeyState(VK_SHIFT))
        return true;
#endif
    return false;
}

bool InputRecorder::IsControlDown() const 
{
    //if (QApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier)
    //    return true;
#ifdef WIN32
    if (GetAsyncKeyState(VK_CONTROL))
        return true;
#endif
    return false;
}

bool InputRecorder::IsAltDown() const
{
    //return QApplication::keyboardModifiers() & Qt::KeyboardModifier::AltModifier; 
#ifdef WIN32
    if (GetAsyncKeyState(VK_MENU))
        return true;
#endif
    return false;
}

bool InputRecorder::IsArrowDown(int arrow) const
{
    //return QApplication::keyboardModifiers() & Qt::KeyboardModifier::AltModifier; 
#ifdef WIN32
    if (arrow == Qt::Key_Left)
        return GetAsyncKeyState(VK_LEFT);
    else if (arrow == Qt::Key_Right)
        return GetAsyncKeyState(VK_RIGHT);
    else if (arrow == Qt::Key_Down)
        return GetAsyncKeyState(VK_DOWN);
    else if (arrow == Qt::Key_Up)
        return GetAsyncKeyState(VK_UP);
#endif
    return false;
}

int InputRecorder::RemapKey(int key) const
{
    if (key == Qt::Key_Home)
        return VK_HOME;
    else if (key == Qt::Key_End)
        return VK_END;
    else if (key == Qt::Key_Up)
        return VK_UP;
    else if (key == Qt::Key_Left)
        return VK_LEFT;
    else if (key == Qt::Key_Right)
        return VK_RIGHT;
    else if (key == Qt::Key_Down)
        return VK_DOWN;
    return (key);
}

bool InputRecorder::IsKeySequenceActive(const QKeySequence& sequence) const
{
    for (int i = 0; i < sequence.count(); ++i)
    {
        uint code = sequence[i];
        if (code == Qt::Key::Key_Control && !IsControlDown())
            return false;
        if (code == Qt::Key::Key_Shift && !IsShiftDown())
            return false;
        if (code == Qt::Key::Key_Alt && !IsAltDown())
            return false;
        if (code == Qt::Key::Key_Down || code == Qt::Key::Key_Up || code == Qt::Key::Key_Right || code == Qt::Key::Key_Left)
            return IsArrowDown(code);
        if (!IsKeyPressed(code))
            return false;
    }
    return sequence.count() > 0;
}

}