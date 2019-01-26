#pragma once

#include "Gizmo.h"
#include "ViewController.h"

#include <EditorLib/Selectron.h>

#include <SprueEngine/IHaveGizmos.h>

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Graphics/Viewport.h>

#include <memory>
#include <set>
#include <vector>

namespace Urho3D
{
    class Node;
}

class DocumentBase;

namespace SprueEditor
{
    class InputRecorder;
    class RenderWidget;
    class ViewManager;

    /// Allowed multiple viewport layouts.
    enum ViewPaneLayout
    {
        VPL_SinglePane,             // Just one window
        VPL_DoubleLeftAndRight,     // Left and right tall
        VPL_DoubleTopAndBottom,     // Top and bottom wide
        VPL_TripleTallLeft,         // 1 tall view on the left, and 2 views on the right
        VPL_TripleTallRight,        // 1 tall view on the right, and 2 views on the left
        VPL_TripleWideTop,          // 1 wide view on the top, and 2 views on the bottom
        VPL_TripleWideBottom,       // 1 wide view on the bottom, and 2 views on the top
        VPL_Quad                    // 4 views
    };

    /// Contains the viewing pane data.
    struct ViewPane
    {
        Urho3D::SharedPtr<Urho3D::Viewport> viewport_;
        Urho3D::SharedPtr<Urho3D::Node> cameraNode_;
        Urho3D::SharedPtr<Urho3D::Camera> camera_;
    };

    /// Base class for viewports to use
    class ViewBase : public Urho3D::Object
    {
        URHO3D_OBJECT(ViewBase, Object);
        friend class ViewManager;
    public:
        ViewBase(Urho3D::Context* ctx, DocumentBase* owner);
        virtual ~ViewBase() { }

        /// View has become active
        virtual void Activate() = 0;
        /// View has become inactive
        virtual void Deactivate();

        /// Selections have been changed, must update gizmos and bounds drawing (only non-control point gizmos are used and they are locked to translate only mode)
        virtual void SelectionChanged(Selectron* newSelection) { }
        /// A property was changed that is indicated to have visual consequences.
        virtual void VisuallyChanged(Selectron* fromWho) { }
        /// The transform mode of gizmos has been changed.
        virtual void SetGizmoMode(SprueEditor::GizmoMode mode) { }
        /// The local/global space of gizmos has been changed.
        virtual void SetGizmoLocal(bool local) { }

        /// Check our viewport controllers for commands that need to be executed
        virtual bool CheckControllers();

        /// Called whenever a gizmo has been updated.
        virtual void GizmoUpdated(std::shared_ptr<Gizmo>) { }
        /// Called to perform any custom rendering that needs to be done (ie. DebugRenderer)
        virtual void RenderUpdate();

        /// Returns the Urho3D scene for this view.
        Urho3D::Scene* GetScene() const { return scene_; }
        /// Returns the Urho3D viewport for this view.
        Urho3D::Viewport* GetViewport() const { return viewport_; }
        /// Returns the node that contains the camera for this scene.
        Urho3D::Node* GetCamera() const { return cameraNode_; }
        /// Return the ViewManager that owns this view.
        ViewManager* GetViewManager() const { return manager_; }
        /// Returns the document that created this view.
        DocumentBase* GetDocument() const { return document_; }

        std::shared_ptr<Gizmo> GetActiveGizmo();
        /// Returns all currently contained gizmos. Transient.
        std::vector< std::shared_ptr<Gizmo> >& GetGizmos() { return gizmos_; }
        /// Returns all currently contained gizmos. Transient.
        const std::vector< std::shared_ptr<Gizmo> >& GetGizmos() const { return gizmos_; }
        virtual void RedetermineGizmos();

        std::vector< std::shared_ptr<ViewController> >& GetViewController() { return viewControllers_; }
        const std::vector< std::shared_ptr<ViewController> >& GetViewController() const { return viewControllers_; }
        std::shared_ptr<ViewController> GetActiveViewController() const { return activeViewController_; }

        template<typename T>
        std::shared_ptr<T> GetController() {
            for (unsigned i = 0; i < viewControllers_.size(); ++i)
            {
                if (auto ret = std::dynamic_pointer_cast<T>(viewControllers_[i]))
                    return ret;
            }
            return std::shared_ptr<T>();
        }

    protected:
        virtual void RedetermineGizmosInternal() { }

        /// List of all possibly ViewControllers that responsible for manipulating the view and the contents.
        std::vector< std::shared_ptr<ViewController> > viewControllers_;
        /// The view controller that is currently in control.
        std::shared_ptr<ViewController> activeViewController_;

        /// All available gizmos from the current selection.
        std::vector<std::shared_ptr<Gizmo> > gizmos_;
        /// Tracked state of gizmo transform modes.
        SprueEngine::GizmoMode gizmoMode_ = SprueEngine::GizmoMode::GIZ_Translate;

        /// Scene for the view.
        Urho3D::SharedPtr<Urho3D::Scene> scene_;
        /// Viewport for the view.
        Urho3D::SharedPtr<Urho3D::Viewport> viewport_;
        /// Camera node for the view.
        Urho3D::SharedPtr<Urho3D::Node> cameraNode_;
        /// Owning ViewManager. Prefer access through this as opposed to global access through SprueKitEditor (future-proof).
        ViewManager* manager_;
        /// The document that is responsible for this view
        DocumentBase* document_;
    };

}