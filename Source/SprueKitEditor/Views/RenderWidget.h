#pragma once

#include <EditorLib/Selectron.h>

#include <QWidget>
#include <QLabel>

#include "InputRecorder.h"

namespace Urho3D
{
    class Context;
}

namespace SprueEditor
{
    class ViewManager;

    class RenderWidget : public QWidget, public SelectronLinked
    {
        Q_OBJECT
    public:
        RenderWidget();

        /// Returns the internal view manager
        ViewManager* GetViewManager() { return viewManager_; }
        /// Returns the Urho3D::Context held in the ViewManager
        Urho3D::Context* GetUrhoContext();

        virtual void keyPressEvent(QKeyEvent* event) override;
        virtual void keyReleaseEvent(QKeyEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void wheelEvent(QWheelEvent* event) override;
        virtual void enterEvent(QEvent* event) override;
        virtual void paintEvent(QPaintEvent*) override {  }

        InputRecorder* GetInput() { return &inputTracking_; }

        /// Connects notification handlers for this object's responses to selection changes
        virtual void Link(Selectron* sel) override;

        virtual QPaintEngine* paintEngine() { return 0x0; }
        void repaintLbl() { lbl_->repaint(); }

    private:
        QLabel* lbl_;
        ViewManager* viewManager_;
        InputRecorder inputTracking_;
        QPoint mousePos_;
        int mouseGrabStack_;
        int keyGrabStack_;
        bool suppressNextMouse_;
    };

}