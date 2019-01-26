#include "ViewBase.h"

#include "InputRecorder.h"
#include "RenderWidget.h"
#include "ViewManager.h"
#include "../UrhoHelpers.h"

#include <EditorLib/Selectron.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <SprueEngine/Core/SceneObject.h>

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/IOEvents.h>
#include <Urho3D/Engine/EngineEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/Zone.h>

using namespace Urho3D;
using namespace SprueEngine;

namespace SprueEditor
{

ViewBase::ViewBase(Urho3D::Context* ctx, DocumentBase* owner) : 
    Urho3D::Object(ctx), document_(owner) 
{ 
}

void ViewBase::Deactivate()
{
    if (activeViewController_)
    {
        activeViewController_->Terminate();
        activeViewController_ = 0x0;
    }
}

bool ViewBase::CheckControllers()
{
    if (!viewControllers_.empty())
    {
        bool vcChanged = false;
        // Index loop allows for modification during iteration by the "PassiveChecks" method
        for (unsigned i = 0; i < viewControllers_.size(); ++i)
        {
            auto vc = viewControllers_[i];
            if (vc->IsEnabled())
            {
                if (vc->CheckActivation(activeViewController_.get()))
                {
                    if (activeViewController_ && activeViewController_ != vc)
                    {
                        if (!vcChanged) // don't call terminate if we never even got to start
                            activeViewController_->Terminate();
                        activeViewController_ = vc;
                        vcChanged = true;
                    }
                    else if (!activeViewController_)
                    {
                        activeViewController_ = vc;
                        vcChanged = true;
                    }
                }
                vc->PassiveChecks(activeViewController_.get());
            }
            else
                vc->DisabledChecked(activeViewController_.get());
        }
        if (activeViewController_)
        {
            if (vcChanged)
                activeViewController_->Prepare();

            activeViewController_->Update();
            if (activeViewController_->CheckTermination())
            {
                activeViewController_->Terminate();
                activeViewController_ = 0x0;
            }
        }

        return activeViewController_.get() != 0x0;
    }

    return false;
}

std::shared_ptr<Gizmo> ViewBase::GetActiveGizmo()
{
    for (auto giz : gizmos_)
    {
        if (giz->IsActive())
            return giz;
    }
    return 0x0;
}

void ViewBase::RedetermineGizmos()
{
    auto activeGizmo = GetActiveGizmo();
    RedetermineGizmosInternal();
    if (activeGizmo)
    {
        std::shared_ptr<SprueEditor::Gizmo> madeActive;
        for (auto giz : gizmos_)
        {
            if (giz->Equal(activeGizmo.get()))
            {
                giz->SetActive(true);
                madeActive = giz;
            }
        }

        if (madeActive)
        {
            for (auto giz : gizmos_)
                if (giz != madeActive)
                    giz->SetActive(false);
        }
    }
    if (GetCamera())
        GetCamera()->SetWorldPosition(GetCamera()->GetWorldPosition());
    GetViewManager()->RunFrame();
}

void ViewBase::RenderUpdate()
{
    for (auto giz : gizmos_)
    {
        if (giz->IsActive())
            giz->PaintGizmo(cameraNode_->GetComponent<Camera>(), scene_->GetComponent<DebugRenderer>());
    }
}

}