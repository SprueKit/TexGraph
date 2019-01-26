#pragma once

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Math/Color.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Math/Matrix3x4.h>
#include <Urho3D/Math/Matrix4.h>
#include <Urho3D/Math/Plane.h>
#include <Urho3D/Math/Quaternion.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Vector4.h>

class ofxManipulator
{
public:
    static Urho3D::DebugRenderer* debugRenderer_;
    static Urho3D::Camera* camera_;

    ofxManipulator();
    ~ofxManipulator();

    enum MANIPULATOR_AXES
    {
        AXE_X = 1,
        AXE_Y = 2,
        AXE_Z = 4
    };

    enum MANIPULATOR_TYPE
    {
        MANIPULATOR_NONE,
        MANIPULATOR_SCALE,
        MANIPULATOR_ROTATION,
        MANIPULATOR_TRANSLATION,
        MANIPULATOR_ROTATION_TRANSLATION, // pairs up the Translation and Rotation gizmos into one super gizmo
        MANIPULATOR_AXIAL,  // Can reorient the axis in screenspace and rotate around the axis, similar to Spore's simplified edit gizmo for anchored objects
    };

    void draw(Urho3D::Camera* cam);

    bool isLocalSpace() { return m_localSpace; }
    void setLocalSpace(bool value) { m_localSpace = value; }

    unsigned GetManipulatorAxes() { return m_axisFlags; }
    void SetManipulatorAxes(unsigned flags) { m_axisFlags = flags; }

    Urho3D::Matrix3x4 getMatrix();
    void setMatrix(Urho3D::Matrix3x4);

    Urho3D::Vector3 getScale();
    Urho3D::Quaternion getRotation();
    Urho3D::Vector3 getTranslation();

    void setScale(Urho3D::Vector3);
    void setRotation(Urho3D::Quaternion);
    void setTranslation(Urho3D::Vector3);

    void scale(Urho3D::Vector3);
    void rotate(Urho3D::Quaternion);
    void translate(Urho3D::Vector3);

    void toggleScale();
    void toggleRotation();
    void toggleTranslation();

    float getManipulatorScale();
    void setManipulatorScale(float scale);
    MANIPULATOR_TYPE getManipulatorType();
    void setManipulatorType(MANIPULATOR_TYPE type);
    void setManipulatorColors(Urho3D::Color x, Urho3D::Color y, Urho3D::Color z, Urho3D::Color w, Urho3D::Color select);

    void mouseMoved(int x, int y);
    bool mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, float deltaX, float deltaY, int button);
    void mouseReleased(int x, int y, int button);

public:
    enum SCALE_TYPE
    {
        SCALE_NONE,
        SCALE_X,
        SCALE_Y,
        SCALE_Z,
        SCALE_XY,
        SCALE_XZ,
        SCALE_YZ,
        SCALE_XYZ
    };

    enum ROTATION_TYPE
    {
        ROTATION_NONE,
        ROTATION_X,
        ROTATION_Y,
        ROTATION_Z,
        ROTATION_SCREEN,
    };

    enum TRANSLATION_TYPE
    {
        TRANSLATION_NONE,
        TRANSLATION_X,
        TRANSLATION_Y,
        TRANSLATION_Z,
        TRANSLATION_XY,
        TRANSLATION_XZ,
        TRANSLATION_YZ,
        TRANSLATION_XYZ
    };

    enum AXIAL_TYPE
    {
        AXIAL_NONE,
        AXIAL_DIRECTION,
        AXIAL_ANGLE
    };

    void getAxes(Urho3D::Vector3& x, Urho3D::Vector3& y, Urho3D::Vector3& z, bool forceLocal = false);

    float computeScreenFactor();

    void getCurrScale(SCALE_TYPE &type, unsigned int x, unsigned int y);
    void getCurrRotation(ROTATION_TYPE &type, unsigned int x, unsigned int y);
    void getCurrTranslation(TRANSLATION_TYPE &type, unsigned int x, unsigned int y);
    void getCurrAxial(AXIAL_TYPE& type, unsigned int x, unsigned int y);

    Urho3D::Ray createRay(float x, float y);
    void intersectRay(Urho3D::Plane &plane, Urho3D::Vector3 &inter_point, Urho3D::Ray);
    Urho3D::Vector3 raytrace(Urho3D::Ray, Urho3D::Vector3 normal, Urho3D::Matrix3x4 matrix);

    Urho3D::Plane createPlane(Urho3D::Vector3 p, Urho3D::Vector3 n);
    bool checkRotationPlane(Urho3D::Vector3 normal, float factor, Urho3D::Ray);

    void drawCircle(Urho3D::Vector3 origin, Urho3D::Vector3 vtx, Urho3D::Vector3 vty, Urho3D::Color color);
    void drawAxis(Urho3D::Vector3 origin, Urho3D::Vector3 axis, Urho3D::Vector3 vtx, Urho3D::Vector3 vty, float fct, float fct2, Urho3D::Color color);
    void drawScaleAxis(Urho3D::Vector3 origin, Urho3D::Vector3 axis, Urho3D::Vector3 vtx, Urho3D::Color color);
    void drawCamembert(Urho3D::Vector3 origin, Urho3D::Vector3 vtx, Urho3D::Vector3 vty, float ng, Urho3D::Color color);
    void drawQuad(Urho3D::Vector3 origin, float size, bool is_selected, Urho3D::Vector3 axis_u, Urho3D::Vector3 axis_v, Urho3D::Color color);
    void drawTriangle(Urho3D::Vector3 origin, float size, bool is_selected, Urho3D::Vector3 axis_u, Urho3D::Vector3 axis_v, Urho3D::Color color);

    bool rayQuad(Urho3D::Ray ray, Urho3D::Vector3 origin, float size, Urho3D::Vector3 axis_u, Urho3D::Vector3 axis_v);
    bool rayTriangle(Urho3D::Ray ray, Urho3D::Vector3 origin, float size, Urho3D::Vector3 axis_u, Urho3D::Vector3 axis_v);

    SCALE_TYPE m_currScale;
    SCALE_TYPE m_currScalePredict;

    ROTATION_TYPE m_currRotation;
    ROTATION_TYPE m_currRotationPredict;

    TRANSLATION_TYPE m_currTranslation;
    TRANSLATION_TYPE m_currTranslationPredict;

    AXIAL_TYPE m_currAxial;
    AXIAL_TYPE m_currAxialPredict;

    MANIPULATOR_TYPE m_currManipulator;

    bool m_localSpace = false;
    float m_angleRad;
    float m_screenFactor;
    float m_manipulatorScale;
    float m_lockedCursor_x;
    float m_lockedCursor_y;
    unsigned m_axisFlags = AXE_X | AXE_Y | AXE_Z;

    Urho3D::Vector3 m_lockedPoint;
    Urho3D::Vector3 m_axis;
    Urho3D::Vector3 m_rotate_x;
    Urho3D::Vector3 m_rotate_y;

    Urho3D::Vector3 m_scale;
    Urho3D::Vector3 m_scaleSaved;
    Urho3D::Vector3 m_translation;
    Urho3D::Vector3 m_translationSaved;
    Urho3D::Quaternion m_rotation;
    Urho3D::Quaternion m_rotationSaved;

    Urho3D::Color m_x_color;
    Urho3D::Color m_y_color;
    Urho3D::Color m_z_color;
    Urho3D::Color m_w_color;
    Urho3D::Color m_select_color;

    Urho3D::Plane m_plane;

    Urho3D::Matrix3x4 m_view;
    Urho3D::Matrix3x4 m_viewInverse;
    Urho3D::Matrix4 m_proj;
};