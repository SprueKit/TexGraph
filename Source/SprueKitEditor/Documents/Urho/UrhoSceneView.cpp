#include "UrhoSceneView.h"

#include "../../GlobalAccess.h"
#include "../../Views/ViewController.h"
#include "../../Views/ViewManager.h"
#include "../../Views/Controllers/FlyController.h"
#include "../../Views/Controllers/GizmoController.h"
#include "../../Views/Controllers/UrhoRaySelectController.h"

#include "../../Views/Urho/UrhoGizmo.h"

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Zone.h>

using namespace Urho3D;

namespace UrhoEditor
{

    UrhoSceneView::UrhoSceneView(Urho3D::Context* context, BaseUrhoDocument* document) :
        SprueEditor::PerspectiveView(context, document),
        document_(document)
    {

    }

    UrhoSceneView::~UrhoSceneView()
    {

    }

    void UrhoSceneView::Activate()
    {
        PerspectiveView::Activate();
        if (frameStats_ == 0x0)
            frameStats_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();
        if (camPos_ == 0x0)
            camPos_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();
        if (triStats_ == 0x0)
            triStats_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();

        frameStats_->SetVisible(true);
        frameStats_->SetText("TEST");

        frameStats_->SetFont(context_->GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
        frameStats_->SetColor(Color(1, 1, 0));
        frameStats_->SetPosition(5, 5);

        camPos_->SetFont(context_->GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
        camPos_->SetPosition(5, 0);
        camPos_->SetColor(Color(1, 1, 0));
        camPos_->SetAlignment(Urho3D::HorizontalAlignment::HA_LEFT, Urho3D::VerticalAlignment::VA_BOTTOM);

        triStats_->SetAlignment(Urho3D::HorizontalAlignment::HA_RIGHT, Urho3D::VerticalAlignment::VA_TOP);
        triStats_->SetFont(context_->GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
        triStats_->SetColor(Color(1, 1, 0));

        if (viewControllers_.empty())
        {
            viewControllers_.push_back(std::shared_ptr<SprueEditor::ViewController>(new SprueEditor::FlyController(cameraNode_, GetViewManager(), this, GetViewManager()->GetInput())));
            viewControllers_.push_back(std::shared_ptr<SprueEditor::ViewController>(new SprueEditor::UrhoRaySelectController(GetViewManager(), this, GetViewManager()->GetInput(), scene_)));
        }

        ofxManipulator::camera_ = cameraNode_->GetComponent<Camera>();
        ofxManipulator::debugRenderer_ = scene_->GetComponent<Urho3D::DebugRenderer>();
    }

    void UrhoSceneView::Deactivate()
    {
        PerspectiveView::Deactivate();
        viewControllers_.clear();
        frameStats_->SetVisible(false);
        camPos_->SetVisible(false);
    }

    void UrhoSceneView::SelectionChanged(Selectron* newSelection)
    {
        gizmos_.clear();
        viewControllers_.erase(viewControllers_.begin() + 2, viewControllers_.end());

        for (unsigned i = 0; i < newSelection->GetSelectedCount(); ++i)
        {
            if (auto object = newSelection->GetSelected<UrhoEditor::SerializableDataSource>(i))
            {
                Urho3D::Node* selectedNode = 0x0;
                if (Urho3D::Node* node = dynamic_cast<Urho3D::Node*>(object->GetObject().Get()))
                    selectedNode = node;
                else if (Urho3D::Component* comp = dynamic_cast<Urho3D::Component*>(object->GetObject().Get()))
                    selectedNode = comp->GetNode();

                if (selectedNode)
                {
                    std::shared_ptr<UrhoEditor::UrhoGizmo> giz(new UrhoEditor::UrhoGizmo(selectedNode));
                    giz->Construct();
                    gizmos_.push_back(giz);
                    viewControllers_.push_back(std::shared_ptr<SprueEditor::ViewController>(new SprueEditor::GizmoController(giz, manager_, this, manager_->GetInput())));
                    giz->SetLocal(GetViewManager()->GetGizmoLocal());
                }
            }
        }

        if (!gizmos_.empty())
            gizmos_.front()->SetActive(true);
    }

    void UrhoSceneView::VisuallyChanged(Selectron* object)
    {

    }

    void UrhoSceneView::RenderUpdate()
    {
        PerspectiveView::RenderUpdate();

        Urho3D::DebugRenderer* debug = scene_->GetComponent<Urho3D::DebugRenderer>();
        
        // Draw anything the mouse ray is currently over
        if (auto raySelector = ((SprueEditor::UrhoRaySelectController*)viewControllers_[1].get()))
        {
            if (auto node = raySelector->GetRaycast())
            {
                debug->AddNode(node, false);
                auto comps = node->GetComponents();
                for (auto comp : comps)
                    comp->DrawDebugGeometry(debug, false);
            }
        }
        
        Selectron* sel = Global_ObjectSectron();
        for (unsigned i = 0; i < sel->GetSelectedCount(); ++i)
        {
            if (auto selected = sel->GetSelected<UrhoEditor::SerializableDataSource>(i))
            {
                if (Urho3D::Serializable* object = selected->GetObject())
                {
                    if (Urho3D::Node* node = dynamic_cast<Urho3D::Node*>(object))
                    {
                        debug->AddNode(node);
                        for (auto comp : node->GetComponents())
                            comp->DrawDebugGeometry(debug, false);
                    }
                    else if (Urho3D::Component* comp = dynamic_cast<Urho3D::Component*>(object))
                        comp->DrawDebugGeometry(debug, false);
                }
            }
        }

        frameStats_->SetText(QString("FPS: %1").arg(1.0f / context_->GetSubsystem<Renderer>()->GetFrameInfo().timeStep_).toStdString().c_str());
        camPos_->SetText(QString("X: %1 Y: %2 Z: %3").arg(cameraNode_->GetWorldPosition().x_).arg(cameraNode_->GetWorldPosition().y_).arg(cameraNode_->GetWorldPosition().z_).toStdString().c_str());
        triStats_->SetText(QString("Triangles: %1").arg(context_->GetSubsystem<Renderer>()->GetNumPrimitives()).toStdString().c_str());

        for (auto giz : gizmos_)
        {
            if (giz->IsActive())
                giz->PaintGizmo(cameraNode_->GetComponent<Camera>(), debug);
        }
    }

    void UrhoSceneView::GizmoUpdated(std::shared_ptr<SprueEditor::Gizmo> giz)
    {
        // Refresh the values of any other gizmos
        for (auto g : gizmos_)
        {
            if (g != giz)
                g->RefreshValue();
        }
    }
}