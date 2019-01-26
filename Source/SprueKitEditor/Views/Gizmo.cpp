#include "Gizmo.h"

#include "../UrhoHelpers.h"

#include <EditorLib/Selectron.h>
#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Math/Matrix3x4.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/StaticModel.h>

#include <SprueEngine/StringHash.h>

using namespace Urho3D;

#define POINT_ON_SPHERE(sphere, theta, phi) Vector3( \
        sphere.center_.x_ + sphere.radius_ * sinf((float)(theta)  * DEG_TO_RAD) * sinf((float)(phi) * DEG_TO_RAD), \
        sphere.center_.y_ + sphere.radius_ * cosf((float)(phi) * DEG_TO_RAD), \
        sphere.center_.z_ + sphere.radius_ * cosf((float)(theta) * DEG_TO_RAD) * sinf((float)(phi) * DEG_TO_RAD))

static int SignWeight(float val) {
    return (0 < val) - (val < 0);
}

namespace SprueEditor
{

#define GIZMO_RADIUS 0.25f

Gizmo::Gizmo() :
    gizmoCapabilities_(0)
{
    manipulator_ = new ofxManipulator();
    manipulator_->setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
}

Gizmo::~Gizmo()
{
}

void Gizmo::Construct()
{
    SetLocal(Settings::GetInstance()->GetPage("Viewport")->GetValue("Local transform").toBool());
    SetForNone();
    manipulator_->setMatrix(GetTransform());
}

void Gizmo::NotifyDataChanged(void* src, Selectron* sel)
{
    sel->NotifyDataChanged(src, GetEditObject(), SprueEngine::StringHash("Position"));
    sel->NotifyDataChanged(src, GetEditObject(), SprueEngine::StringHash("Rotation"));
    sel->NotifyDataChanged(src, GetEditObject(), SprueEngine::StringHash("Scale"));
}

bool Gizmo::CheckMouse(int x, int y) const
{
    return manipulator_->mousePressed(x, y, 0);
}

#define APPLY_TRANSFORM(GIZMO) Urho3D::Matrix3x4 transform = editMatrix_.Transpose(); ApplyTransform(transform)

void Gizmo::MouseMove(int x, int y)
{
    manipulator_->mouseMoved(x, y);
}

void Gizmo::MouseDrag(int x, int y, float deltaX, float deltaY)
{
    manipulator_->mouseDragged(x, y, deltaX, deltaY, 0);
}

void Gizmo::MouseUp(int x, int y)
{
    manipulator_->mouseReleased(x, y, 0);
}

void Gizmo::RefreshValue()
{
    SetLocal(local_);
    manipulator_->setMatrix(GetTransform());
}

void Gizmo::SetForNone()
{
    manipulator_->setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
    rotate_ = false;
    translate_ = false;
    scale_ = false;
}

void Gizmo::SetForTranslation()
{
    manipulator_->setManipulatorType(ofxManipulator::MANIPULATOR_TRANSLATION);
    rotate_ = false;
    translate_ = true;
    scale_ = false;
}

void Gizmo::SetForRotation()
{
    manipulator_->setManipulatorType(ofxManipulator::MANIPULATOR_ROTATION);
    rotate_ = true;
    translate_ = false;
    scale_ = false;
}

void Gizmo::SetForScale()
{
    manipulator_->setManipulatorType(ofxManipulator::MANIPULATOR_SCALE);
    scale_ = true;
    translate_ = false;
    rotate_ = false;
}

void Gizmo::SetForAxial()
{
    manipulator_->setManipulatorType(ofxManipulator::MANIPULATOR_AXIAL);
}

void Gizmo::SetLocal(bool state)
{
    local_ = state;
    manipulator_->setLocalSpace(local_);
}

Matrix3x4 Gizmo::PrepareTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
    return Matrix3x4(position, rotation, scale);
}

bool Gizmo::ApplySnapping(Urho3D::Matrix3x4& transform) const
{
    if (auto page = Settings::GetInstance()->GetPage("Viewport"))
    {
        if (manipulator_->getManipulatorType() == ofxManipulator::MANIPULATOR_TRANSLATION)
        {
            if (auto setting = page->Get("Snap position"))
            {
                if (setting->value_.toBool())
                {
                    auto value = page->Get("Snap position spacing");
                    const float spacing = value->value_.toFloat();
                    Urho3D::Vector3 trans, scl;
                    Urho3D::Quaternion rot;
                    transform.Decompose(trans, rot, scl);

                    if (manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_X || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_XY || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_XZ || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_XYZ)
                        trans.x_ = Floor(trans.x_ / spacing) * spacing;
                    if (manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_Y || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_XY || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_YZ || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_XYZ)
                        trans.y_ = Floor(trans.y_ / spacing) * spacing;
                    if (manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_Z || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_YZ || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_XZ || manipulator_->m_currTranslation == ofxManipulator::TRANSLATION_XYZ)
                        trans.z_ = Floor(trans.z_ / spacing) * spacing;

                    transform.SetTranslation(trans);

                    wasTranslationSnapped_ = true;
                    return true;
                }
            }
        }
        else if (manipulator_->getManipulatorType() == ofxManipulator::MANIPULATOR_ROTATION)
        {
            if (auto setting = page->Get("Snap rotation"))
            {
                if (setting->value_.toBool())
                {
                    auto value = page->Get("Snap rotation degrees");
                    const float degreeSnap = value->value_.toFloat();
                    Urho3D::Vector3 trans, scl;
                    Urho3D::Quaternion rot;
                    transform.Decompose(trans, rot, scl);

                    Urho3D::Vector3 angles = rot.EulerAngles();
                    if (manipulator_->m_currRotation == ofxManipulator::ROTATION_X || manipulator_->m_currRotation == ofxManipulator::ROTATION_SCREEN)
                        angles.x_ = floorf(angles.x_ / degreeSnap) * degreeSnap;
                    if (manipulator_->m_currRotation == ofxManipulator::ROTATION_Y || manipulator_->m_currRotation == ofxManipulator::ROTATION_SCREEN)
                        angles.y_ = floorf(angles.y_ / degreeSnap) * degreeSnap;
                    if (manipulator_->m_currRotation == ofxManipulator::ROTATION_Z || manipulator_->m_currRotation == ofxManipulator::ROTATION_SCREEN)
                        angles.z_ = floorf(angles.z_ / degreeSnap) * degreeSnap;
                    rot.FromEulerAngles(angles.x_, angles.y_, angles.z_);
                    
                    transform = Urho3D::Matrix3x4(trans, rot, scl);

                    wasRotationSnapped_ = true;
                    return true;
                }
            }
        }
    }
    return true;
}

void Gizmo::SynchronizeTranslation()
{
    auto trans = GetTransform();
    manipulator_->setTranslation(trans.Translation());
}

void Gizmo::SynchronizeRotation()
{
    auto trans = GetTransform();
    manipulator_->setRotation(trans.Rotation());
}

void Gizmo::PrePaint()
{
    // Only perform this settings check if possibly neeeded
    if (wasTranslationSnapped_)
    {
        if (auto page = Settings::GetInstance()->GetPage("Viewport"))
        {
            if (auto setting = page->Get("Snap position"))
            {
                if (!setting->value_.toBool())
                {
                    SynchronizeTranslation();
                    wasTranslationSnapped_ = false;
                }
            }
        }
    }

    // Only perform this settings check if possibly neeeded
    if (wasRotationSnapped_)
    {
        if (auto page = Settings::GetInstance()->GetPage("Viewport"))
        {
            if (auto setting = page->Get("Snap rosition"))
            {
                if (!setting->value_.toBool())
                {
                    SynchronizeRotation();
                    wasRotationSnapped_ = false;
                }
            }
        }
    }
}

void Gizmo::PaintGizmo(Urho3D::Camera* camera, Urho3D::DebugRenderer* debugRender)
{    
    PrePaint();
    ofxManipulator::camera_ = camera;
    ofxManipulator::debugRenderer_ = debugRender;
    
    manipulator_->draw(camera);
}

Urho3D::Matrix3x4 Gizmo::GetEditTransform()
{
    return manipulator_->getMatrix();
}

}