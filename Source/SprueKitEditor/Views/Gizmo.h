#pragma once

#include "ofxManipulator.h"

#include <EditorLib/Commands/SmartUndoStack.h>
#include <EditorLib/DataSource.h>

#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Math/Matrix3x4.h>
#include <Urho3D/Math/Matrix4.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Math/Vector3.h>

#include <memory>

class Selectron;

namespace Urho3D
{
    class Node;
    class DebugRenderer;
}

class DocumentBase;

namespace SprueEditor
{

enum GizmoAxis
{
    GA_None = 0,
    GA_X = 1,
    GA_Y = 2,
    GA_Z = 4
};

enum GizmoMode
{
    GM_None = 0,
    GM_Translate = 1,
    GM_Rotate = (1 << 1),
    GM_Scale = (1 << 2),
    GM_ReferencePoint = (1 << 3),
    GM_DeformedPoint = (1 << 4),
    GM_Axial = (1 << 5)
};

class Gizmo
{
public:
    Gizmo();
    virtual ~Gizmo();

    virtual void Construct();

    bool CheckMouse(int x, int y) const;
    void MouseMove(int x, int y);
    void MouseDrag(int x, int y, float deltaX, float deltaY);
    void MouseUp(int x, int y);

    virtual std::shared_ptr<DataSource> GetEditObject() = 0;
    virtual unsigned GetCapabilities() const { return gizmoCapabilities_; }
    virtual bool CanTranslate() const { return gizmoCapabilities_ & GM_Translate; }
    virtual bool CanScale() const { return gizmoCapabilities_ & GM_Scale; }
    virtual bool CanRotate() const { return gizmoCapabilities_ & GM_Rotate; }

    virtual bool IsControlPoint() const { return gizmoCapabilities_ & (GM_ReferencePoint | GM_DeformedPoint); }
    virtual bool IsInitialControlPoint() const { return gizmoCapabilities_ & GM_ReferencePoint; }
    virtual bool IsDeformedControlPoint() const { return gizmoCapabilities_ & GM_DeformedPoint; }

    virtual void NotifyDataChanged(void* src, Selectron* selectron);

    virtual void RefreshValue();
    virtual bool Equal(Gizmo* rhs) const = 0;
    virtual bool ExecuteCmd(unsigned cmd) { return false; }

    void SetForNone();
    void SetForTranslation();
    void SetForRotation();
    void SetForScale();
    void SetForAxial();
    void SetLocal(bool state);

    /// Gets the transform of the manipulator.
    Urho3D::Matrix3x4 GetEditTransform();
    /// Gets the transform of the object being edited.
    virtual Urho3D::Matrix3x4 GetTransform() = 0;
    /// Prepares disparate data for use as a transform.
    virtual Urho3D::Matrix3x4 PrepareTransform(const Urho3D::Vector3& position, const Urho3D::Quaternion& rotation, const Urho3D::Vector3& scale);
    /// Applies the manipulator's transform to the object.
    virtual void ApplyTransform(const Urho3D::Matrix3x4& transform) = 0;
    /// Applies snapping settings, should be called before ApplyTransform.
    virtual bool ApplySnapping(Urho3D::Matrix3x4& transform) const;

    /// Resets the manipulator's translation to match that of the object being edited. Called when position snapping is turned off.
    virtual void SynchronizeTranslation();
    /// Resets the manipulator's rotation to match that of the object being edited. Called when rotation snapping is turned off.
    virtual void SynchronizeRotation();

    virtual void PrePaint();
    virtual void PaintGizmo(Urho3D::Camera* camera, Urho3D::DebugRenderer* debugRender);

    void SetDrawScale(float scale) { drawScale_ = scale; }
    ofxManipulator* GetManipulator() { return manipulator_; }

    bool IsActive() const { return active_; }
    void SetActive(bool active) { active_ = active; }

    virtual void RecordTransforms() { }
    virtual void PushUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix) = 0;
    virtual SmartCommand* CreateUndo(DocumentBase* document, const Urho3D::Matrix3x4& oldMatrix) = 0;

protected:
    float drawScale_ = 1.0f;
    bool local_ = false;
    bool translate_ = true;
    bool rotate_ = false;
    bool scale_ = false;
    bool axisAngle_ = false;
    bool active_ = false;
    bool wasChanged_ = false;
    mutable bool wasRotationSnapped_ = false;
    mutable bool wasTranslationSnapped_ = false;
    unsigned gizmoCapabilities_ = 0;

    ofxManipulator* manipulator_ = 0x0;
};

}