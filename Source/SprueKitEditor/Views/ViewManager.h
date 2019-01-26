#pragma once

#include <qobject.h>
#include <qtimer.h>

#include "Gizmo.h"
#include "ViewBase.h"

#include <EditorLib/Selectron.h>

#include <SprueEngine/IHaveGizmos.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/StringHash.h>

#include <memory>
#include <set>
#include <vector>

namespace Urho3D
{
    class Node;
    class Camera;
    class Scene;
    class Viewport;
}

class DocumentBase;

namespace SprueEditor
{

class InputRecorder;
class RenderWidget;
class ViewManager;

/// Enumeration for how views are to be layed out
enum ViewportLayout
{
    VL_Center = 0,  // Single view
    VL_TopBottom,   //2 views, top and below
    VL_LeftRight,   //2 views, let and right
    VL_2Top1Bottom, //3 views, 2 top 1 bottom
    VL_1Top2Bottom, //3 views, 1 top 2 bottom
    VL_2Left1Right, //3 views, 2 left 1 right
    VL_1Left2Right, //3 views, 1 left 2 right
    VL_Quad,        //4 views
};


/// Manages the activity of the different views.
class ViewManager : public QObject, public Urho3D::Object
{
    friend class RenderWidget;

    Q_OBJECT
public:
    ViewManager(QWidget* holder);
    virtual ~ViewManager();

    URHO3D_OBJECT(ViewManager, Object);

    void HandleLogMessage(Urho3D::StringHash event, Urho3D::VariantMap& params);
    void HandleRenderUpdate(Urho3D::StringHash event, Urho3D::VariantMap& params);

    void SetSelection(Selectron* sel) {
        if (activeView_)
            activeView_->SelectionChanged(sel);
    }

    std::shared_ptr<ViewBase> GetActiveView() { return activeView_; }

    InputRecorder* GetInput() const;
    const RenderWidget* GetRenderWidget() const { return renderWidget_; }

    void SetGizmoMode(SprueEditor::GizmoMode mode) { 
        gizmoMode_ = mode;
        if (activeView_) 
            activeView_->SetGizmoMode(gizmoMode_); 
    }

    void SetGizmoLocal(bool local)
    {
        gizmoLocal_ = local;
        if (activeView_)
            activeView_->SetGizmoLocal(gizmoLocal_);
    }

    bool GetGizmoLocal() const { return gizmoLocal_; }

    SprueEditor::GizmoMode GetGizmoMode() const { return gizmoMode_; }

    void RunFrame();

signals:
    void SceneChanged(Urho3D::Scene* scene);
    void ViewChanged(ViewBase* view);

protected:
    void CreateSetup();

private slots:
    void OnTimeout();
    void OnDataChanged(Selectron* object, bool visuallyChanged);
    void DocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc);
    void OnViewChanged(DocumentBase* doc);

private:
    Urho3D::Context* context_;
    Urho3D::Engine* engine_;

    QTimer* timer_;
    RenderWidget* renderWidget_;
    bool movingView_ = false;
    bool gizmoLocal_ = false;
    SprueEditor::GizmoMode gizmoMode_ = SprueEditor::GM_Translate;

    std::shared_ptr<ViewBase> activeView_;
};

}