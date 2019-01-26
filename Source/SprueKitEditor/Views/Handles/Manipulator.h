#pragma once

#include <Urho3D/Math/Color.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Matrix3.h>
#include <Urho3D/Math/Matrix3x4.h>
#include <Urho3D/Math/Matrix4.h>
#include <Urho3D/Math/Ray.h>

namespace ImManip
{

    enum GizmoStyle {
        GIZ_Custom = 0,
        GIZ_Translate,
        GIZ_Rotate,
        GIZ_Scale,
        GIZ_Axial
    };

    struct ManipulatorContext {
        bool wasMouseDown[3];
        bool mouseButtonsDown[3];

        bool localSpace;
        float xDelta;
        float yDelta;
        float xPos;
        float yPos;

        Urho3D::Ray mouseRay_;
        Urho3D::Matrix3x4 transform_;
        Urho3D::Matrix3x4 parentTransform_;
        Urho3D::Matrix3x4 oldTransform_;

        int previousHandleID;
        int nextID;

    public:
        void Init();
        void Begin();
        void End();
    };

    ManipulatorContext* CreateContext();

    /// Produces a common transform manipulator (using ManipulatorContext::transform_)
    bool Transform(bool scaled);
    // Axis arrow that tracks along the direction.
    bool AxisTrack(Urho3D::Vector3 origin, const Urho3D::Vector3 dir, bool scaled);

    bool AxisBox(Urho3D::Vector3 origin, const Urho3D::Vector3 dir, bool scaled);

    // Axis arrow that rotates instead of tracks along the arrow.
    bool AxisPoint(Urho3D::Vector3 origin, const Urho3D::Vector3 dir, bool scaled);
    // Return false.
    bool AxisSheet(Urho3D::Vector3 origin, int axis, float offset);
    //
    bool Box(Urho3D::Vector3 origin, float size, bool scaled);

    bool Camembert(Urho3D::Vector3 origin, Urho3D::Vector3 dir, bool scaled);


    void DrawAxis(Urho3D::Vector3 origin, Urho3D::Vector3 dir, float scale, Urho3D::Color color);
    void Drawbox(Urho3D::Vector3 origin, float size, float scale, Urho3D::Color color);
    void DrawSheet(Urho3D::Vector3 pts[4], float scale, Urho3D::Color color);
    void DrawCamembert(Urho3D::Vector3 origin, Urho3D::Vector3 dir, float scale, Urho3D::Color);
    void DrawcamembertPie(Urho3D::Vector3 origin, Urho3D::Vector3 dir, float startAng, float endAng, float scale, Urho3D::Color);

    // Handles the mouse hit behaviour of an axis
    bool AxisBehaviour(Urho3D::Vector3 origin, const Urho3D::Vector3 dir, bool scaled);
    bool HandleSheetBehaviour(Urho3D::Vector3 origin, const Urho3D::Vector3 dir, bool scaled);

}