#pragma once

#include "FrameControl.h"

#include <QWidget>

/// Implements a Quick action toolbar containing a set of commands. Basically just a widget with some toolbuttons.
/// Unlike the MS ribbon UI this toolbar is fixed and not customizable (will likely only contain save, undo, and redo buttons in practice).
/// Presently unused: Windows frame painting interferred with drag-and-drop events.
class QuickAccessToolbar : public QWidget, public FrameControl
{
public:
    /// Actions to place into the quick action toolbar.
    QuickAccessToolbar(const std::vector<QAction*>& actions);

    /// This widget never needs to move.
    virtual void WindowResized(const QSize& newWindowSize) override;
};