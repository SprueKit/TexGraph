#include "UrhoGizmo.h"

#include "../../Commands/Urho/UrhoPropertyCmd.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace UrhoEditor
{

UrhoGizmo::UrhoGizmo(Urho3D::Node* transformNode) :
    SprueEditor::Gizmo(),
    transformingNode_(transformNode),
    source_(new UrhoEditor::NodeDataSource(transformNode))
{
    gizmoCapabilities_ = SprueEditor::GM_Translate | SprueEditor::GM_Rotate | SprueEditor::GM_Scale;
}

UrhoGizmo::~UrhoGizmo()
{

}

void UrhoGizmo::Construct()
{
    Gizmo::Construct();
    if (Settings::GetInstance()->GetPage("Viewport")->GetValue("Translate mode").toBool())
        SetForTranslation();
    else if (Settings::GetInstance()->GetPage("Viewport")->GetValue("Rotate mode").toBool())
        SetForRotation();
    else if (Settings::GetInstance()->GetPage("Viewport")->GetValue("Scale mode").toBool())
        SetForScale();
    else
        SetForNone();
}

Urho3D::Matrix3x4 UrhoGizmo::GetTransform()
{
    return transformingNode_->GetWorldTransform();
}

void UrhoGizmo::ApplyTransform(const Matrix3x4& transform)
{
    Urho3D::Vector3 trans, scale;
    Urho3D::Quaternion rot;
    transform.Decompose(trans, rot, scale);
    transformingNode_->SetWorldTransform(trans, rot, scale);
    RefreshValue();
}

void UrhoGizmo::RefreshValue()
{
    SetLocal(local_);
}

void UrhoGizmo::PushUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix)
{
    if (auto undo = CreateUndo(document, oldMatrix))
        document->GetUndoStack()->Push(undo);
}

SmartCommand* UrhoGizmo::CreateUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix)
{
    return new UrhoEditor::UrhoGizmoCmd(source_, oldMatrix, GetTransform());
}

bool UrhoGizmo::Equal(Gizmo* rhs) const
{
    if (auto other = dynamic_cast<UrhoGizmo*>(rhs))
        return other->source_ == source_;
    return false;
}

}