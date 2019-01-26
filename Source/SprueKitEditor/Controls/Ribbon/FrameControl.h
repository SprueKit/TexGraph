#pragma once

#include <QSize>

/// Abstract base class for controls that need to live in the non-client area so they respond to window resizing in order to position themselves correctly.
class FrameControl
{
public:
    virtual void WindowResized(const QSize& newWindowSize) = 0;
};