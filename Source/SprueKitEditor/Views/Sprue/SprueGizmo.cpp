#include "SprueGizmo.h"

#include "DebugRenderer.h"
#include "../../Commands/SprueKit/SprueSceneCmds.h"

#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

namespace SprueEditor
{

    SprueGizmo::SprueGizmo(std::shared_ptr<DataSource> dataSource, std::shared_ptr<SprueEngine::Gizmo> gizmo) :
        Gizmo(),
        gizmo_(gizmo),
        dataSource_(dataSource)
    {
    }

    SprueGizmo::~SprueGizmo()
    {

    }

    void SprueGizmo::Construct()
    {
        Gizmo::Construct();
        SetLocal(Settings::GetInstance()->GetPage("Viewport")->GetValue("Local transform").toBool());
        if (gizmo_)
        {
            gizmoCapabilities_ = gizmo_->flags_;
            if (gizmo_->CanTranslate() && Settings::GetInstance()->GetPage("Viewport")->GetValue("Translate mode").toBool())
                SetForTranslation();
            else if (gizmo_->CanRotate() && Settings::GetInstance()->GetPage("Viewport")->GetValue("Rotate mode").toBool())
                SetForRotation();
            else if (gizmo_->CanScale() && Settings::GetInstance()->GetPage("Viewport")->GetValue("Scale mode").toBool())
                SetForScale();
        }
        else
            SetForNone();
    }

    void SprueGizmo::RefreshValue()
    {
        //if (gizmoNode_)
        {
            if (gizmo_)
                gizmo_->RefreshValue();
            SetLocal(local_);
            //??Gizmo::RefreshValue();
        }
    }

    Urho3D::Matrix3x4 SprueGizmo::GetTransform()
    {
        return Urho3D::Matrix3x4(&gizmo_->transform_.v[0][0]);
    }

    void SprueGizmo::ApplyTransform(const Urho3D::Matrix3x4& transform)
    {
        gizmo_->transform_ = SprueEngine::Mat3x4(&transform.m00_);
        gizmo_->UpdateValue();
        RefreshValue();
    }

    void SprueGizmo::PushUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix)
    {
        if (auto undo = CreateUndo(document, oldMatrix))
            document->GetUndoStack()->Push(undo);
    }

    SmartCommand* SprueGizmo::CreateUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix)
    {
        Urho3D::Matrix3x4 newMat = GetTransform();
        return new SprueEditor::SprueModelSceneCommands::GizmoCmd(std::dynamic_pointer_cast<SprueEditor::SceneObjectDataSource>(dataSource_), SprueEngine::Mat3x4(&oldMatrix.m00_), SprueEngine::Mat3x4(&newMat.m00_));
    }

    void SprueGizmo::PaintGizmo(Urho3D::Camera* camera, Urho3D::DebugRenderer* debugRender)
    {
        Gizmo::PaintGizmo(camera, debugRender);
        if (gizmo_)
        {
            DebugRenderer debugDraw(debugRender);
            gizmo_->DrawDebug(&debugDraw);
        }
    }

    bool SprueGizmo::Equal(Gizmo* rhs) const
    {
        if (auto other = dynamic_cast<SprueGizmo*>(rhs))
            return gizmo_->Equals(other->gizmo_.get());
        return false;
    }
}