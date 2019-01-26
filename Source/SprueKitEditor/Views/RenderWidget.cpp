#include "RenderWidget.h"

#include "ViewManager.h"

#include "../Data/SprueDataSources.h"
#include "../Data/UrhoDataSources.h"

#include <qevent.h>

#include <QApplication>
#include <QLabel>

#include <SprueEngine/Property.h>
#include <SprueEngine/IEditable.h>
#include <SprueEngine/Core/SceneObject.h>

#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Engine/DebugHud.h>

namespace SprueEditor
{

RenderWidget::RenderWidget() :
    mouseGrabStack_(0),
    keyGrabStack_(0),
    suppressNextMouse_(false)
{
    viewManager_ = new ViewManager(this);
    setAttribute(Qt::WA_KeyCompression, false);
    setUpdatesEnabled(false);
    setMouseTracking(true);

    lbl_ = new QLabel("Testing this out", this);
}

void RenderWidget::keyPressEvent(QKeyEvent* event)
{
    inputTracking_.SetKeyDown(event->key());
    //if (keyGrabStack_ == 0)
    //{
    //    grabKeyboard();
    //    ++keyGrabStack_;
    //}
    //event->accept();
    QWidget::keyPressEvent(event);
}

Urho3D::Context* RenderWidget::GetUrhoContext()
{
    return viewManager_->context_;
}

void RenderWidget::keyReleaseEvent(QKeyEvent* event)
{
    inputTracking_.SetKeyUp(event->key());
    //if (--keyGrabStack_ == 0)
    //    releaseKeyboard();
    //event->accept();
    QWidget::keyReleaseEvent(event);
}

void RenderWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
        inputTracking_.SetMouseDown(0);
    if (event->buttons() & Qt::MouseButton::RightButton)
        inputTracking_.SetMouseDown(1);
    if (event->buttons() & Qt::MouseButton::MiddleButton)
        inputTracking_.SetMouseDown(2);
    if (mouseGrabStack_ == 0)
    {
        ++mouseGrabStack_;
        mousePos_ = mapToGlobal(QPoint(event->x(), event->y()));
        grabMouse();
        setCursor(QCursor(Qt::CursorShape::BlankCursor));
    }
    //event->accept();
    QWidget::mousePressEvent(event);
}

void RenderWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if ((event->button() == Qt::MouseButton::LeftButton))
        inputTracking_.SetMouseUp(0);
    if ((event->button() == Qt::MouseButton::RightButton))
        inputTracking_.SetMouseUp(1);
    if ((event->button() == Qt::MouseButton::MiddleButton))
        inputTracking_.SetMouseUp(2);
    if (--mouseGrabStack_ == 0)
    {
        releaseMouse();
        setCursor(QCursor(Qt::CursorShape::ArrowCursor));
    }
    QWidget::mouseReleaseEvent(event);
}

void RenderWidget::wheelEvent(QWheelEvent* event)
{
    inputTracking_.SetWheel(event->angleDelta().y());
    QWidget::wheelEvent(event);
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!suppressNextMouse_)
    {
      QPoint local = mapToGlobal(QPoint(event->x(), event->y()));
      inputTracking_.SetMousePos(Urho3D::IntVector2(local.x(), local.y()), false);    
        if (mouseGrabStack_ > 0 && inputTracking_.IsMouseLocked())
        {
            //Urho3D::IntVector2 newPos(x() + width() / 2, y() + height() / 2);
            //QCursor::setPos(newPos.x_, newPos.y_);
            Urho3D::IntVector2 newPos(mousePos_.x(), mousePos_.y());
            QCursor::setPos(newPos.x_, newPos.y_);
            inputTracking_.SetMousePos(newPos, false, true);
            //inputTracking_.SetMousePos(newPos, false, true);
            suppressNextMouse_ = true;
        }
    }
    else
        suppressNextMouse_ = false;
    //event->accept();
    QWidget::mouseMoveEvent(event);
}

void RenderWidget::enterEvent(QEvent* event)
{
    QPoint pt = QCursor::pos();
    inputTracking_.SetMousePos(Urho3D::IntVector2(pt.x(), pt.y()), true);
    //event->accept();
}

void RenderWidget::Link(Selectron* sel)
{
    connect(sel, &Selectron::SelectionChanged, [=](void* src, Selectron* sel)
    {
        if (src == this)
            return;
        viewManager_->SetSelection(sel);
    });

    connect(sel, &Selectron::DataChanged, [=](void* src, Selectron* sel, unsigned property) {
        if (src == this)
            return;
        if (auto object = sel->GetSelected<IEditableDataSource>())
        {
            if (dynamic_cast<SprueEngine::SceneObject*>(object->GetEditable()))
            {
                bool visualChange = false;
                if (SprueEngine::TypeProperty* prop = object->GetEditable()->FindProperty(property))
                    visualChange = prop->GetFlags() & SprueEngine::PS_VisualConsequence;
                viewManager_->OnDataChanged(sel, visualChange);
            }
            return;
        }
        if (auto object = sel->GetSelected<UrhoEditor::SerializableDataSource>())
        {
            viewManager_->OnDataChanged(sel, false);
            return;
        }
    });
}

}