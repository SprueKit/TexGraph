#include "TexturePreviewView.h"

#include "../../Data/UrhoDataSources.h"
#include "../../Data/SprueDataSources.h"

#include "TextureDocument.h"

#include "../../Views/Gizmo.h"
#include "../../Views/RenderWidget.h"
#include "../../Panels/SceneTreePanel.h"
#include "../../SprueKitEditor.h"
#include "../../Views/Urho/UrhoGizmo.h"
#include "../../UrhoHelpers.h"
#include "../../Views/ViewUtility.h"

#include "../../Views/Controllers/FlyController.h"
#include "../../Views/Controllers/GizmoController.h"
#include "../../Views/Controllers/UrhoRaySelectController.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/Selectron.h>
#include <EditorLib/TaskProcessor.h>

#include <SprueEngine/Core/SceneObject.h>
#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/FString.h>

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

#include <memory>
#include <set>

using namespace Urho3D;

namespace SprueEditor
{
    TexturePreviewView::TexturePreviewView(TextureDocument* doc, Urho3D::Context* context) :
        PerspectiveView(context, doc)
    {
        ResourceCache* cache = GetContext()->GetSubsystem<ResourceCache>();
        Zone* zone = scene_->GetOrCreateComponent<Zone>();
        zone->SetBoundingBox(Urho3D::BoundingBox(Urho3D::Vector3(-1000, -1000, -1000), Urho3D::Vector3(1000, 1000, 1000)));
        zone->SetZoneTexture(cache->GetResource<TextureCube>("Textures/Skybox.xml"));

        Node* skyBoxNode = scene_->CreateChild("SKYBOX");
        skyBoxNode->SetScale(500.0f);
        Skybox* skybox = skyBoxNode->GetOrCreateComponent<Skybox>();
        skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(cache->GetResource<Material>("IBL/Day/Material.xml"));

        previewNode_ = scene_->CreateChild("PREVIEW_MESH");
        StaticModel* model = previewNode_->GetOrCreateComponent<StaticModel>();
        model->SetModel(cache->GetResource<Model>("Models/Sphere.mdl"));
        model->SetMaterial(doc->GetMaterial());

        Node* dirLightNode = scene_->CreateChild("DIR_LIGHT");
        dirLightNode->SetPosition(Urho3D::Vector3(-10, 10, -10));
        dirLightNode->LookAt(Urho3D::Vector3(0, 0, 0));
        Light* dirLight = dirLightNode->GetOrCreateComponent<Light>();
        dirLight->SetLightType(Urho3D::LIGHT_DIRECTIONAL);

        Node* pointLightNode = scene_->CreateChild("POINT_LIGHT");
        pointLightNode->SetPosition(Urho3D::Vector3(5, 5, 5));
        Light* pointLight = pointLightNode->GetOrCreateComponent<Light>();
        pointLight->SetLightType(Urho3D::LIGHT_POINT);
    }

    TexturePreviewView::~TexturePreviewView()
    {
        
    }

    void TexturePreviewView::UpdateMaterial()
    {
        if (previewNode_)
            previewNode_->GetOrCreateComponent<StaticModel>()->SetMaterial(((TextureDocument*)document_)->GetMaterial());
    }

    void TexturePreviewView::Activate()
    {
        PerspectiveView::Activate();
        if (frameStats_ == 0x0)
            frameStats_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();
        if (camPos_ == 0x0)
            camPos_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();
        if (triStats_ == 0x0)
            triStats_ = context_->GetSubsystem<UI>()->GetRoot()->CreateChild<Text>();

        frameStats_->SetVisible(true);
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
            viewControllers_.push_back(std::shared_ptr<ViewController>(new FlyController(cameraNode_, GetViewManager(), this, GetViewManager()->GetInput())));
            viewControllers_.push_back(std::shared_ptr<ViewController>(new UrhoRaySelectController(GetViewManager(), this, GetViewManager()->GetInput(), scene_)));
        }

        ofxManipulator::camera_ = cameraNode_->GetComponent<Camera>();
        ofxManipulator::debugRenderer_ = scene_->GetComponent<Urho3D::DebugRenderer>();
    }

    void TexturePreviewView::Deactivate()
    {
        PerspectiveView::Deactivate();
        viewControllers_.clear();
        frameStats_->SetVisible(false);
        camPos_->SetVisible(false);
    }

    void TexturePreviewView::SelectionChanged(Selectron* sel)
    {
        gizmos_.clear();
        viewControllers_.erase(viewControllers_.begin() + 2, viewControllers_.end());

        for (unsigned i = 0; i < sel->GetSelectedCount(); ++i)
        {
            if (auto object = sel->GetSelected<UrhoEditor::SerializableDataSource>(i))
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
                    //giz->GetNode()->SetEnabled(true);
                    gizmos_.push_back(giz);
                    viewControllers_.push_back(std::shared_ptr<SprueEditor::ViewController>(new GizmoController(giz, manager_, this, manager_->GetInput())));
                    giz->SetLocal(GetViewManager()->GetGizmoLocal());
                }
            }
        }

        if (!gizmos_.empty())
            gizmos_.front()->SetActive(true);
    }

    void TexturePreviewView::VisuallyChanged(Selectron* object)
    {

    }

    void TexturePreviewView::RenderUpdate()
    {
        PerspectiveView::RenderUpdate();

        Urho3D::DebugRenderer* debug = scene_->GetComponent<Urho3D::DebugRenderer>();
        Selectron* sel = GetViewManager()->GetRenderWidget()->GetSelectron();
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

        frameStats_->SetText(FString("FPS: %1", (1.0f / context_->GetSubsystem<Renderer>()->GetFrameInfo().timeStep_)).c_str());
        camPos_->SetText(FString("X: %1 Y: %2 Z: %3", cameraNode_->GetWorldPosition().x_, cameraNode_->GetWorldPosition().z_, cameraNode_->GetWorldPosition().y_).c_str());
        triStats_->SetText(FString("Triangles: %1", context_->GetSubsystem<Renderer>()->GetNumPrimitives()).c_str());

        for (auto giz : gizmos_)
        {
            if (giz->IsActive())
                giz->PaintGizmo(cameraNode_->GetComponent<Camera>(), debug);
        }
    }

    void TexturePreviewView::GizmoUpdated(std::shared_ptr<Gizmo> giz)
    {
        // Refresh the values of any other gizmos
        for (auto g : gizmos_)
        {
            if (g != giz)
                g->RefreshValue();
        }
    }

}