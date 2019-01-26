#include "ofxManipulator.h"

#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Plane.h>

#include <QApplication>

#include <vector>
#include <algorithm>

#define PI 3.141596f

Urho3D::DebugRenderer* ofxManipulator::debugRenderer_ = 0x0;
Urho3D::Camera* ofxManipulator::camera_ = 0x0;

static int SignWeight(float val) {
    return (0 < val) - (val < 0);
}

ofxManipulator::ofxManipulator()
    :m_currScale(SCALE_NONE)
    , m_currScalePredict(SCALE_NONE)
    , m_currRotation(ROTATION_NONE)
    , m_currRotationPredict(ROTATION_NONE)
    , m_currTranslation(TRANSLATION_NONE)
    , m_currTranslationPredict(TRANSLATION_NONE)
    , m_currAxial(AXIAL_NONE)
    , m_currAxialPredict(AXIAL_NONE)
    , m_currManipulator(MANIPULATOR_NONE)
    , m_x_color(1.0f, 0.25f, 0.2f)
    , m_y_color(0.18f, 0.8f, 0.25f)
    , m_z_color(0.0f, 0.45f, 0.85f)
    , m_w_color(1.0f, 0.25f, 1.0f)
    , m_select_color(1.0f, 0.86f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_manipulatorScale(1.0f)
    , m_screenFactor(0.0f)
    , m_angleRad(0.0f)
{
}

ofxManipulator::~ofxManipulator()
{
    
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void ofxManipulator::draw(Urho3D::Camera* cam)
{
    m_view = cam->GetView();
    m_proj = cam->GetProjection();
    m_viewInverse = m_view.Inverse();
    m_screenFactor = computeScreenFactor();

    Urho3D::Vector3 origin = m_translation;
    float screenf = m_screenFactor;
    float screenf_half = screenf * 0.5f;

    Urho3D::Matrix3x4 m = Urho3D::Matrix3x4::IDENTITY;
    m.SetRotation(m_rotation.RotationMatrix());
    Urho3D::Vector3 axe_x, axe_y, axe_z;
    getAxes(axe_x, axe_y, axe_z);

    Urho3D::Color unit_color_x = m_x_color;
    Urho3D::Color unit_color_y = m_y_color;
    Urho3D::Color unit_color_z = m_z_color;
    Urho3D::Color unit_color_w = m_w_color;
    Urho3D::Color select_color = m_select_color;

    switch (m_currManipulator)
    {
    case (MANIPULATOR_SCALE) :
    {
        SCALE_TYPE curr = m_currScalePredict;

        if (m_axisFlags & (AXE_X | AXE_Z))
            drawTriangle(origin, screenf_half, ((curr == SCALE_XZ) || (curr == SCALE_XYZ)), axe_x, axe_z, unit_color_y);
        if (m_axisFlags & (AXE_X | AXE_Y))
            drawTriangle(origin, screenf_half, ((curr == SCALE_XY) || (curr == SCALE_XYZ)), axe_x, axe_y, unit_color_z);
        if (m_axisFlags & (AXE_Y | AXE_Z))
            drawTriangle(origin, screenf_half, ((curr == SCALE_YZ) || (curr == SCALE_XYZ)), axe_y, axe_z, unit_color_x);

        axe_x *= screenf;
        axe_y *= screenf;
        axe_z *= screenf;

        if (m_axisFlags & AXE_X)
            drawScaleAxis(origin, axe_x, axe_y, ((curr == SCALE_X) || (curr == SCALE_XYZ)) ? select_color : unit_color_x);
        if (m_axisFlags & AXE_Y)
            drawScaleAxis(origin, axe_y, axe_x, ((curr == SCALE_Y) || (curr == SCALE_XYZ)) ? select_color : unit_color_y);
        if (m_axisFlags & AXE_Z)
            drawScaleAxis(origin, axe_z, axe_x, ((curr == SCALE_Z) || (curr == SCALE_XYZ)) ? select_color : unit_color_z);
    } break;

    case (MANIPULATOR_ROTATION) :
    {
        ROTATION_TYPE type = m_currRotation;
        ROTATION_TYPE curr = m_currRotationPredict;

        Urho3D::Vector3 right;
        Urho3D::Vector3 front;
        Urho3D::Vector3 dir;
        Urho3D::Vector3 up;

        // So that the orientation of the object is always easily understood the local axes are drawn
        Urho3D::Vector3 localAxe_x, localAxe_y, localAxe_z;
        getAxes(localAxe_x, localAxe_y, localAxe_z, true);
        debugRenderer_->AddLine(origin, origin + localAxe_x * screenf, Urho3D::Color::RED, false);
        debugRenderer_->AddLine(origin, origin + localAxe_y * screenf, Urho3D::Color::GREEN, false);
        debugRenderer_->AddLine(origin, origin + localAxe_z * screenf, Urho3D::Color::BLUE, false);

        dir = origin - camera_->GetNode()->GetWorldPosition();// .Translation();
        dir.Normalize();

        right = dir;
        right = right.CrossProduct(axe_y);
        right.Normalize();

        up = dir;
        up = up.CrossProduct(right);
        up.Normalize();

        right = dir;
        right = right.CrossProduct(up);
        right.Normalize();

        if (m_axisFlags == (AXE_X | AXE_Y | AXE_Z))
        {
            Urho3D::Vector3 vtx = 1.2f * screenf * up;
            Urho3D::Vector3 vty = 1.2f * screenf * right;
            drawCircle(origin, vtx, vty, curr == ROTATION_SCREEN ? select_color : unit_color_w);
        }

        if (m_axisFlags & AXE_X)
        {
            right = dir;
            right = right.CrossProduct(axe_x);
            right.Normalize();
            front = right;
            front = front.CrossProduct(axe_x);
            front.Normalize();
            Urho3D::Vector3 vtx = screenf * right;
            Urho3D::Vector3 vty = screenf * front;
            drawCircle(origin, vtx, vty, curr == ROTATION_X ? select_color : unit_color_x);
        }

        if (m_axisFlags & AXE_Y)
        {
            right = dir;
            right = right.CrossProduct(axe_y);
            right.Normalize();
            front = right;
            front = front.CrossProduct(axe_y);
            front.Normalize();
            Urho3D::Vector3 vtx = screenf * right;
            Urho3D::Vector3 vty = screenf * front;
            drawCircle(origin, vtx, vty, curr == ROTATION_Y ? select_color : unit_color_y);
        }

        if (m_axisFlags & AXE_Z)
        {
            right = dir;
            right = right.CrossProduct(axe_z);
            right.Normalize();
            front = right;
            front = front.CrossProduct(axe_z);
            front.Normalize();
            Urho3D::Vector3 vtx = screenf * right;
            Urho3D::Vector3 vty = screenf * front;
            drawCircle(origin, vtx, vty, curr == ROTATION_Z ? select_color : unit_color_z);
        }

      if (type != ROTATION_NONE)
      {
          Urho3D::Vector3 x = m_rotate_x;
          Urho3D::Vector3 y = m_rotate_y;
          float angle = m_angleRad;
          x *= screenf;
          y *= screenf;
          switch (curr)
          {
          case (ROTATION_X) :
              drawCamembert(origin, x, y, -angle, unit_color_x);
              break;
          case (ROTATION_Y) :
              drawCamembert(origin, x, y, -angle, unit_color_y);
              break;
          case (ROTATION_Z) :
              drawCamembert(origin, x, y, -angle, unit_color_z);
              break;
          case (ROTATION_SCREEN) :
              drawCamembert(origin, x, y, -angle, unit_color_w);
              break;
          }
      }
    } break;

    case (MANIPULATOR_TRANSLATION) :
    {
        TRANSLATION_TYPE curr = m_currTranslationPredict;

        if (m_axisFlags & (AXE_X | AXE_Z))
            drawQuad(origin, screenf_half, ((curr == TRANSLATION_XZ) || (curr == TRANSLATION_XYZ)), axe_x, axe_z, unit_color_y);
        if (m_axisFlags & (AXE_X | AXE_Y))
            drawQuad(origin, screenf_half, ((curr == TRANSLATION_XY) || (curr == TRANSLATION_XYZ)), axe_x, axe_y, unit_color_z);
        if (m_axisFlags & (AXE_Y | AXE_Z))
            drawQuad(origin, screenf_half, ((curr == TRANSLATION_YZ) || (curr == TRANSLATION_XYZ)), axe_y, axe_z, unit_color_x);

        axe_x *= screenf;
        axe_y *= screenf;
        axe_z *= screenf;

        if (m_axisFlags & AXE_X)
            drawAxis(origin, axe_x, axe_y, axe_z, 0.05f, 0.83f, ((curr == TRANSLATION_X) || (curr == TRANSLATION_XYZ)) ? select_color : unit_color_x);
        if (m_axisFlags & AXE_Y)
            drawAxis(origin, axe_y, axe_x, axe_z, 0.05f, 0.83f, ((curr == TRANSLATION_Y) || (curr == TRANSLATION_XYZ)) ? select_color : unit_color_y);
        if (m_axisFlags & AXE_Z)
            drawAxis(origin, axe_z, axe_x, axe_y, 0.05f, 0.83f, ((curr == TRANSLATION_Z) || (curr == TRANSLATION_XYZ)) ? select_color : unit_color_z);
    } break;

    case MANIPULATOR_AXIAL:
    {
        AXIAL_TYPE curr = m_currAxialPredict;

        drawAxis(origin, axe_y * screenf, axe_x * screenf, axe_z * screenf, 0.05f, 0.83f, curr == AXIAL_DIRECTION ? select_color : unit_color_z);

        Urho3D::Vector3 right;
        Urho3D::Vector3 front;
        Urho3D::Vector3 dir;
        Urho3D::Vector3 up;

        dir = origin - camera_->GetNode()->GetWorldPosition();// .Translation();
        dir.Normalize();

        right = dir;
        right = right.CrossProduct(axe_y);
        right.Normalize();

        up = dir;
        up = up.CrossProduct(right);
        up.Normalize();

        right = dir;
        right = right.CrossProduct(up);
        right.Normalize();

        if (m_axisFlags & AXE_Y)
        {
            right = dir;
            right = right.CrossProduct(axe_y);
            right.Normalize();
            front = right;
            front = front.CrossProduct(axe_y);
            front.Normalize();
            Urho3D::Vector3 vtx = screenf * right;
            Urho3D::Vector3 vty = screenf * front;
            drawCircle(origin, vtx, vty, curr == ROTATION_Y ? select_color : unit_color_y);
        }

    } break;
    }
}

Urho3D::Matrix3x4 ofxManipulator::getMatrix()
{
    return Urho3D::Matrix3x4(m_translation, m_rotation, m_scale);
}

void ofxManipulator::setMatrix(Urho3D::Matrix3x4 mat)
{
    mat.Decompose(m_translation, m_rotation, m_scale);
}

Urho3D::Vector3 ofxManipulator::getScale()
{
    return m_scale;
}

Urho3D::Quaternion ofxManipulator::getRotation()
{
    return m_rotation;
}

Urho3D::Vector3 ofxManipulator::getTranslation()
{
    return m_translation;
}

void ofxManipulator::setScale(Urho3D::Vector3 scale)
{
    m_scale = scale;
}

void ofxManipulator::setRotation(Urho3D::Quaternion rotation)
{
    m_rotation = rotation;
}

void ofxManipulator::setTranslation(Urho3D::Vector3 translation)
{
    m_translation = translation;
}

void ofxManipulator::scale(Urho3D::Vector3 scale)
{
    m_scale *= scale;
}

void ofxManipulator::rotate(Urho3D::Quaternion rotation)
{
    m_rotation = m_rotation * rotation;
}

void ofxManipulator::translate(Urho3D::Vector3 translation)
{
    m_translation += translation;
}

void ofxManipulator::toggleScale()
{
    m_currManipulator = (m_currManipulator == MANIPULATOR_SCALE) ? MANIPULATOR_NONE : MANIPULATOR_SCALE;
}

void ofxManipulator::toggleRotation()
{
    m_currManipulator = (m_currManipulator == MANIPULATOR_ROTATION) ? MANIPULATOR_NONE : MANIPULATOR_ROTATION;
}

void ofxManipulator::toggleTranslation()
{
    m_currManipulator = (m_currManipulator == MANIPULATOR_TRANSLATION) ? MANIPULATOR_NONE : MANIPULATOR_TRANSLATION;
}

float ofxManipulator::getManipulatorScale()
{
    return m_manipulatorScale;
}

void ofxManipulator::setManipulatorScale(float scale)
{
    m_manipulatorScale = scale;
}

ofxManipulator::MANIPULATOR_TYPE ofxManipulator::getManipulatorType()
{
    return m_currManipulator;
}

void ofxManipulator::setManipulatorType(MANIPULATOR_TYPE type)
{
    m_currManipulator = type;
}

void ofxManipulator::setManipulatorColors(Urho3D::Color x, Urho3D::Color y, Urho3D::Color z, Urho3D::Color w, Urho3D::Color select)
{
    m_x_color = x;
    m_y_color = y;
    m_z_color = z;
    m_w_color = w;
    m_select_color = select;
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void ofxManipulator::mouseMoved(int x, int y)
{
    switch (m_currManipulator)
    {
    case (MANIPULATOR_SCALE) :
        getCurrScale(m_currScalePredict, x, y);
        break;
    case (MANIPULATOR_ROTATION) :
        getCurrRotation(m_currRotationPredict, x, y);
        break;
    case (MANIPULATOR_TRANSLATION) :
        getCurrTranslation(m_currTranslationPredict, x, y);
        break;
    case MANIPULATOR_AXIAL:
        getCurrAxial(m_currAxialPredict, x, y);
        break;
    }
}

static Urho3D::Vector3 trueLockPosition;

bool ofxManipulator::mousePressed(int x, int y, int button)
{
    if (button != 0)
        return false;

    switch (m_currManipulator)
    {
    case (MANIPULATOR_SCALE) :
        getCurrScale(m_currScale, x, y);
        return m_currScale != 0;
    case (MANIPULATOR_ROTATION) :
        getCurrRotation(m_currRotation, x, y);
        return m_currRotation != 0;
    case (MANIPULATOR_TRANSLATION) :
        getCurrTranslation(m_currTranslation, x, y);
        return m_currTranslation != 0;
    case MANIPULATOR_AXIAL:
        getCurrAxial(m_currAxial, x, y);
        return m_currAxial != 0;
    }
    return false;
}

void ofxManipulator::mouseDragged(int x, int y, float deltaX, float deltaY, int button)
{
    // If there's no mouse motion then do nothing.
    if (deltaX == 0 && deltaY == 0)
        return;

    if (button != 0)
        return;

    const bool shiftHeld = QApplication::keyboardModifiers() & Qt::KeyboardModifier::ShiftModifier;
    const float shiftMultiplier = shiftHeld ? 3.0f : 1.0f;

    switch (m_currManipulator)
    {
    case (MANIPULATOR_SCALE) :
    {
        if (m_currScale == SCALE_NONE) {
            getCurrScale(m_currScalePredict, x, y);
            break;
        }

        Urho3D::Vector3 intersection;

        Urho3D::Ray ray = createRay(x, y);
        intersectRay(m_plane, intersection, ray);

        Urho3D::Matrix3x4 m(m_translation, m_rotation, 2);
        Urho3D::Vector3 axe_x, axe_y, axe_z;
        getAxes(axe_x, axe_y, axe_z);

        Urho3D::Vector3 df;

        if (m_currScale == SCALE_XYZ)
        {
            float scale = deltaX;//x - m_lockedCursor_x;
            df = Urho3D::Vector3(scale, scale, scale);
        }
        else {
            df = intersection - m_lockedPoint;
            df.Normalize();
            switch (m_currScale)
            {
            case SCALE_X:  df = Urho3D::Vector3(df.DotProduct(axe_x), 0, 0); break;
            case SCALE_Y:  df = Urho3D::Vector3(0, df.DotProduct(axe_y), 0); break;
            case SCALE_Z:  df = Urho3D::Vector3(0, 0, df.DotProduct(axe_z)); break;
            case SCALE_XY: df = Urho3D::Vector3(df.DotProduct(axe_x + axe_y), df.DotProduct(axe_x + axe_y), 0); break;
            case SCALE_XZ: df = Urho3D::Vector3(df.DotProduct(axe_x + axe_z), 0, df.DotProduct(axe_x + axe_z)); break;
            case SCALE_YZ: df = Urho3D::Vector3(0, df.DotProduct(axe_y + axe_z), df.DotProduct(axe_y + axe_z)); break;
            }
            df *= 10.0f;
        }

        float len = ((sqrtf(df.x_ * df.x_ + df.y_ * df.y_ + df.z_ * df.z_)) / 100.0f) * Settings::GetInstance()->GetValue("Viewport/Scaling speed")->value_.toDouble() * shiftMultiplier;

        m_scale += (df.Normalized() * len);
    } break;

    case (MANIPULATOR_ROTATION) :
    {
        if (m_currRotation == ROTATION_NONE) {
            getCurrRotation(m_currRotationPredict, x, y);
            break;
        }

        Urho3D::Vector3 intersection;

        Urho3D::Ray ray = createRay(x, y);
        intersectRay(m_plane, intersection, ray);

        Urho3D::Vector3 df = intersection - m_translation;
        df.Normalize();

        float acos_angle = df.DotProduct(m_lockedPoint);
        float adjustBy = (acos_angle < -0.99999f) || (acos_angle > 0.99999f) ? 0.0f : acos(acos_angle);

        if (df.DotProduct(m_rotate_y) > 0)
            adjustBy = -adjustBy;

        if (adjustBy != 0.0f)
            m_angleRad += adjustBy;
        else
            break;

        if (m_currRotation == ROTATION_X || m_currRotation == ROTATION_Y || m_currRotation == ROTATION_Z)
        {
            Urho3D::Vector3 axe_x, axe_y, axe_z;
            getAxes(axe_x, axe_y, axe_z);

            float xAxis = m_currRotation == ROTATION_X ? adjustBy * Urho3D::M_RADTODEG * Settings::GetInstance()->GetValue("Viewport/Rotate speed")->value_.toDouble() * shiftMultiplier : 0.0f;
            float yAxis = m_currRotation == ROTATION_Y ? adjustBy * Urho3D::M_RADTODEG * Settings::GetInstance()->GetValue("Viewport/Rotate speed")->value_.toDouble() * shiftMultiplier : 0.0f;
            float zAxis = m_currRotation == ROTATION_Z ? adjustBy * Urho3D::M_RADTODEG * Settings::GetInstance()->GetValue("Viewport/Rotate speed")->value_.toDouble() * shiftMultiplier : 0.0f;
            
            Urho3D::Quaternion q;
            q.FromEulerAngles(xAxis, yAxis, zAxis);
            m_rotation = m_rotation * q;
        }
        else
        {
            Urho3D::Matrix3x4 m = Urho3D::Matrix3x4::IDENTITY;
            m.SetRotation(m_rotation.RotationMatrix());
        
            Urho3D::Quaternion quat(adjustBy * Urho3D::M_RADTODEG, m_plane.normal_);
            Urho3D::Matrix3x4 rotMat = Urho3D::Matrix3x4::IDENTITY;
            rotMat.SetRotation(quat.RotationMatrix());
            m = m * rotMat;

            m_rotation = m.Rotation();
        }
    } break;

    case (MANIPULATOR_TRANSLATION) :
    {
        if (m_currTranslation == TRANSLATION_NONE) {
            getCurrTranslation(m_currTranslationPredict, x, y);
            break;
        }

        Urho3D::Vector3 intersection;

        Urho3D::Ray ray = createRay(x, y);
        intersectRay(m_plane, intersection, ray);
        if (intersection.IsNaN())
            break;

        Urho3D::Matrix3x4 m(m_translation, m_localSpace ? m_rotation : Urho3D::Quaternion::IDENTITY, 1);
        Urho3D::Vector3 axe_x, axe_y, axe_z;
        getAxes(axe_x, axe_y, axe_z);

        if (m_currTranslation == TRANSLATION_XYZ)
        {
            Urho3D::Vector3 df = intersection - m_lockedPoint;
            Urho3D::Vector3 camRight = camera_->GetNode()->GetWorldRight();
            Urho3D::Vector3 camUp = camera_->GetNode()->GetWorldUp();            
            m_translation += (camRight * deltaX * 0.01f) + (camUp * -deltaY * 0.01f);
        }
        else 
        {
            Urho3D::Vector3 df = intersection - m_lockedPoint;
            df.Normalize();
            switch (m_currTranslation)
            {
            case TRANSLATION_X:  df = Urho3D::Vector3(SignWeight(df.DotProduct(axe_x)), 0, 0); break;
            case TRANSLATION_Y:  df = Urho3D::Vector3(0, SignWeight(df.DotProduct(axe_y)), 0); break;
            case TRANSLATION_Z:  df = Urho3D::Vector3(0, 0, SignWeight(df.DotProduct(axe_z))); break;
            case TRANSLATION_XY: df = Urho3D::Vector3(df.DotProduct(axe_x) * 0.75f, df.DotProduct(axe_y) * 0.75f, 0); break;
            case TRANSLATION_XZ: df = Urho3D::Vector3(df.DotProduct(axe_x) * 0.75f, 0, df.DotProduct(axe_z) * 0.75f); break;
            case TRANSLATION_YZ: df = Urho3D::Vector3(0, df.DotProduct(axe_y) * 0.75f, df.DotProduct(axe_z) * 0.75f); break;
            }
            Urho3D::Vector3 vec = (df.x_ * axe_x) + (df.y_ * axe_y) + (df.z_ * axe_z);

            //m_translation = m_translationSaved + vec;
            m_translation += vec * 0.25f * Settings::GetInstance()->GetValue("Viewport/Translate speed")->value_.toDouble() * shiftMultiplier;
        }
    } break;
    case MANIPULATOR_AXIAL:
    {
        if (m_currAxial == AXIAL_NONE)
        {
            getCurrAxial(m_currAxialPredict, x, y);
            return;
        }



    } break;

    }
}

void ofxManipulator::mouseReleased(int x, int y, int button)
{
    if (button != 0)
        return;

    m_currScale = SCALE_NONE;
    m_currRotation = ROTATION_NONE;
    m_currTranslation = TRANSLATION_NONE;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

float ofxManipulator::computeScreenFactor()
{
    Urho3D::Matrix4 viewproj = camera_->GetProjection() * camera_->GetView();
    Urho3D::Vector4 transform = Urho3D::Vector4(m_translation.x_, m_translation.y_, m_translation.z_, 1.0f);
    transform = viewproj * transform;
    float scale = transform.w_ * 0.2f * m_manipulatorScale;
    if (scale <= 0.0f)
        return 1.0f;
    return scale;
}

void ofxManipulator::getCurrScale(SCALE_TYPE &type, unsigned int x, unsigned int y)
{
    m_lockedCursor_x = x;
    m_lockedCursor_y = y;
    m_scaleSaved = m_scale;


    Urho3D::Ray ray = createRay(x, y);

    Urho3D::Matrix3x4 m(m_translation, m_rotation, 1);
    Urho3D::Vector3 axe_x, axe_y, axe_z;
    getAxes(axe_x, axe_y, axe_z);

    Urho3D::Vector3 df;

    df = m_axisFlags & AXE_Y ? raytrace(ray, axe_y, m) : Urho3D::Vector3(100,100,100);

    if ((df.x_ < 0.1f) && (df.z_ < 0.1f) && (df.x_ > -0.1f) && (df.z_ > -0.1f)) { type = SCALE_XYZ; return; }
    else
    {
        df = m_axisFlags & AXE_Z ? raytrace(ray, axe_z, m) : Urho3D::Vector3(100, 100, 100);

        if ((df.x_ < 0.1f) && (df.y_ < 0.1f) && (df.x_ > -0.1f) && (df.y_ > -0.1f)) { type = SCALE_XYZ; return; }
        else
        {
            df = m_axisFlags & AXE_X ? raytrace(ray, axe_x, m) : Urho3D::Vector3(100, 100, 100);

            if ((df.y_ < 0.1f) && (df.z_ < 0.1f) && (df.y_ > -0.1f) && (df.z_ > -0.1f)) { type = SCALE_XYZ; return; }
        }
    }

    df = m_axisFlags & AXE_Y ? raytrace(ray, axe_y, m) : Urho3D::Vector3(100, 100, 100);

    if ((df.x_ >= 0.f) && (df.x_ <= 1.f) && (fabs(df.z_) < 0.1f))          { type = SCALE_X;  return; }
    else if ((df.z_ >= 0.f) && (df.z_ <= 1.f) && (fabs(df.x_) < 0.1f))          { type = SCALE_Z;  return; }
    else if ((df.x_ < 0.5f) && (df.z_ < 0.5f) && (df.x_ > 0.f) && (df.z_ > 0.f)) { type = SCALE_XZ; return; }
    else
    {
        df = m_axisFlags & AXE_Z ? raytrace(ray, axe_z, m) : Urho3D::Vector3(100, 100, 100);

        if ((df.x_ >= 0.f) && (df.x_ <= 1.f) && (fabs(df.y_) < 0.1f))          { type = SCALE_X;  return; }
        else if ((df.y_ >= 0.f) && (df.y_ <= 1.f) && (fabs(df.x_) < 0.1f))          { type = SCALE_Y;  return; }
        else if ((df.x_ < 0.5f) && (df.y_ < 0.5f) && (df.x_ > 0.f) && (df.y_ > 0.f)) { type = SCALE_XY; return; }
        else
        {
            df = m_axisFlags & AXE_X ? raytrace(ray, axe_x, m) : Urho3D::Vector3(100, 100, 100);

            if ((df.y_ >= 0.f) && (df.y_ <= 1.f) && (fabs(df.z_) < 0.1f))          { type = SCALE_Y;  return; }
            else if ((df.z_ >= 0.f) && (df.z_ <= 1.f) && (fabs(df.y_) < 0.1f))          { type = SCALE_Z;  return; }
            else if ((df.y_ < 0.5f) && (df.z_ < 0.5f) && (df.y_ > 0.f) && (df.z_ > 0.f)) { type = SCALE_YZ; return; }
        }
    }

    type = SCALE_NONE;
}

void ofxManipulator::getCurrRotation(ROTATION_TYPE &type, unsigned int x, unsigned int y)
{
    Urho3D::Ray ray = createRay(x, y);

    Urho3D::Matrix3x4 m(m_translation, m_rotation, 1.0f);
    Urho3D::Vector3 axe_x, axe_y, axe_z;
    getAxes(axe_x, axe_y, axe_z);

    if (m_axisFlags & AXE_X && checkRotationPlane(axe_x, 1.0f, ray)) { type = ROTATION_X; return; }
    if (m_axisFlags & AXE_Y && checkRotationPlane(axe_y, 1.0f, ray)) { type = ROTATION_Y; return; }
    if (m_axisFlags & AXE_Z && checkRotationPlane(axe_z, 1.0f, ray)) { type = ROTATION_Z; return; }

    Urho3D::Vector3 direction = m_translation - camera_->GetNode()->GetWorldPosition();
    direction.Normalize();

    if (m_axisFlags == (AXE_X | AXE_Y | AXE_Z))
        if (checkRotationPlane(direction, 1.2f, ray)) { type = ROTATION_SCREEN; return; }

    type = ROTATION_NONE;
}

void ofxManipulator::getCurrTranslation(TRANSLATION_TYPE &type, unsigned int x, unsigned int y)
{
    Urho3D::Vector3 mat_screenCoord = camera_->WorldToScreenPoint(m_translationSaved);
    m_lockedCursor_x = x;
    m_lockedCursor_y = y;
    m_translationSaved = m_translation;

    float screenf = m_screenFactor;
    float screenf_half = screenf * 0.5f;

    Urho3D::Ray ray = createRay(x, y);

    Urho3D::Matrix3x4 m(m_translation, m_localSpace ? m_rotation : Urho3D::Quaternion::IDENTITY, 1.0f);
    Urho3D::Vector3 axe_x, axe_y, axe_z;
    getAxes(axe_x, axe_y, axe_z);

    ray.direction_.Normalize();

    Urho3D::Vector3 df;
    df = m_axisFlags & AXE_Y ? raytrace(ray, axe_y, m) : Urho3D::Vector3(100, 100, 100);

    if ((df.x_ < 0.1f) && (df.z_ < 0.1f) && (df.x_ > -0.1f) && (df.z_ > -0.1f)) { 
        type = TRANSLATION_XYZ; 
        m_plane = createPlane(m_translation, -camera_->GetNode()->GetWorldDirection());
        return; 
    }
    else
    {
        df = m_axisFlags & AXE_Z ? raytrace(ray, axe_z, m) : Urho3D::Vector3(100, 100, 100);

        if ((df.x_ < 0.1f) && (df.y_ < 0.1f) && (df.x_ > -0.1f) && (df.y_ > -0.1f)) { 
            type = TRANSLATION_XYZ; 
            m_plane = createPlane(m_translation, -camera_->GetNode()->GetWorldDirection());
            return; 
        }
        else
        {
            df = m_axisFlags & AXE_X ? raytrace(ray, axe_x, m) : Urho3D::Vector3(100, 100, 100);

            if ((df.y_ < 0.1f) && (df.z_ < 0.1f) && (df.y_ > -0.1f) && (df.z_ > -0.1f)) { 
                type = TRANSLATION_XYZ; 
                m_plane = createPlane(m_translation, -camera_->GetNode()->GetWorldDirection());
                return; 
            }
        }
    }

    df = m_axisFlags & AXE_Y ? raytrace(ray, axe_y, m) : Urho3D::Vector3(100, 100, 100);

    if ((df.x_ >= 0.f) && (df.x_ <= 1.f) && (fabs(df.z_) < 0.1f)) { 
        type = TRANSLATION_X;  return; 
    }
    else if ((df.z_ >= 0.f) && (df.z_ <= 1.f) && (fabs(df.x_) < 0.1f)) { 
        type = TRANSLATION_Z;  return; 
    }
    else if ((df.x_ < 0.5f) && (df.z_ < 0.5f) && (df.x_ > 0.f) && (df.z_ > 0.f)) { 
        type = TRANSLATION_XZ; return; }
    else
    {
        df = m_axisFlags & AXE_Z ? raytrace(ray, axe_z, m) : Urho3D::Vector3(100, 100, 100);

        if ((df.x_ >= 0.f) && (df.x_ <= 1.f) && (fabs(df.y_) < 0.1f)) { 
            type = TRANSLATION_X;  return; 
        }
        else if ((df.y_ >= 0.f) && (df.y_ <= 1.f) && (fabs(df.x_) < 0.1f)) { 
            type = TRANSLATION_Y;  return; 
        }
        else if ((df.x_ < 0.5f) && (df.y_ < 0.5f) && (df.x_ > 0.f) && (df.y_ > 0.f)) { 
            type = TRANSLATION_XY; return; 
        }
        else
        {
            df = m_axisFlags & AXE_X ? raytrace(ray, axe_x, m) : Urho3D::Vector3(100, 100, 100);

            if ((df.y_ >= 0.f) && (df.y_ <= 1.f) && (fabs(df.z_) < 0.1f))          { 
                type = TRANSLATION_Y;   return; }
            else if ((df.z_ >= 0.f) && (df.z_ <= 1.f) && (fabs(df.y_) < 0.1f))          { 
                type = TRANSLATION_Z;   return; }
            else if ((df.y_ < 0.5f) && (df.z_ < 0.5f) && (df.y_ > 0.f) && (df.z_ > 0.f)) { 
                type = TRANSLATION_YZ;  return; }
        }
    }

    type = TRANSLATION_NONE;
}

void ofxManipulator::getCurrAxial(AXIAL_TYPE& type, unsigned int x, unsigned int y)
{
    type = AXIAL_NONE;

    // Axial is always in local space mode
    m_lockedCursor_x = x;
    m_lockedCursor_y = y;
    m_translationSaved = m_translation;

    float screenf = m_screenFactor;
    float screenf_half = screenf * 0.5f;

    Urho3D::Ray ray = createRay(x, y);

    Urho3D::Matrix3x4 m(m_translation, m_rotation, 1.0f);
    Urho3D::Vector3 axe_x, axe_y, axe_z;
    getAxes(axe_x, axe_y, axe_z);

    ray.direction_.Normalize();

    Urho3D::Vector3 df;
    df = raytrace(ray, axe_x, m);
    if ((df.y_ >= 0.f) && (df.y_ <= 1.f) && (fabs(df.z_) < 0.1f)) {
        type = AXIAL_DIRECTION;
        return;
    } 
    else
    {
        df = raytrace(ray, axe_z, m);
        if ((df.y_ >= 0.f) && (df.y_ <= 1.f) && (fabs(df.x_) < 0.1f)) {
            type = AXIAL_DIRECTION;
            return;
        }
    }

    if (m_axisFlags & AXE_Y && checkRotationPlane(axe_y, 1.0f, ray)) { type = AXIAL_ANGLE; return; }
}

Urho3D::Ray ofxManipulator::createRay(float x, float y)
{
    auto graphics = camera_->GetSubsystem<Urho3D::Graphics>();
    return camera_->GetScreenRay(x / ((float)graphics->GetWidth()), y / ((float)graphics->GetHeight()));
}

void ofxManipulator::intersectRay(Urho3D::Plane &m_plane, Urho3D::Vector3 &inter_point, Urho3D::Ray ray)
{
    // TODO: double check the Vector4 conversions
    //Urho3D::Plane plane(Urho3D::Vector3(m_plane.x_, m_plane.y_, m_plane.z_), m_translation);
    inter_point = ray.origin_ + ray.direction_ * ray.HitDistance(m_plane);
    //float den = m_plane.DotProduct(Urho3D::Vector4(ray.direction_, 0.0f));
    //Urho3D::Vector4 tmp = (m_plane * m_plane.w_) - Urho3D::Vector4(ray.origin_, 1.0f);
    //inter_point = ray.origin_ + (m_plane.DotProduct(tmp) / den) * ray.direction_;
}

Urho3D::Vector3 ofxManipulator::raytrace(Urho3D::Ray ray, Urho3D::Vector3 normal, Urho3D::Matrix3x4 matrix)
{
    Urho3D::Vector3 df;
    Urho3D::Vector3 intersection(Urho3D::M_INFINITY, Urho3D::M_INFINITY, Urho3D::M_INFINITY);

    Urho3D::Matrix3x4 invMat = matrix.Inverse();

    m_plane = createPlane(m_translation, normal);
    //Urho3D::Plane plane(normal, m_translation);
    float hit = ray.HitDistance(m_plane);
    if (hit != Urho3D::M_INFINITY)
        intersection = ray.origin_ + ray.direction_ * hit;
    //intersectRay(m_plane, intersection, ray);// ray_origin, ray_dir);
    df = invMat * intersection;
    m_lockedPoint = intersection;
    df /= m_screenFactor;

    return df;
}

Urho3D::Plane ofxManipulator::createPlane(Urho3D::Vector3 p, Urho3D::Vector3 n)
{
    //Urho3D::Vector4 m_plane;
    //Urho3D::Vector3 normal;
    //
    //float length = 1.0f / sqrtf((n.x_ * n.x_) + (n.y_ * n.y_) + (n.z_ * n.z_));
    //
    //normal = n * length;
    //
    //m_plane.x_ = normal.x_;
    //m_plane.y_ = normal.y_;
    //m_plane.z_ = normal.z_;
    //m_plane.w_ = normal.DotProduct(p);
    //
    //return m_plane;
    return Urho3D::Plane(n, p);
}

bool ofxManipulator::checkRotationPlane(Urho3D::Vector3 normal, float factor, Urho3D::Ray ray)
{
    Urho3D::Vector3 df;
    Urho3D::Vector3 intersection;

    m_plane = createPlane(m_translation, normal);
    intersectRay(m_plane, intersection, ray);
    df = intersection - m_translation;
    df /= m_screenFactor;

    float dflen = (df.Length() / factor);
    if ((dflen > 0.9f) && (dflen < 1.1f))
    {
        m_rotationSaved = m_rotation;
        m_lockedPoint = df;
        m_lockedPoint.Normalize();
        m_rotate_x = m_lockedPoint;
        m_rotate_y = m_lockedPoint;
        m_rotate_y = m_rotate_y.CrossProduct(normal);
        m_rotate_y.Normalize();
        m_rotate_x *= factor;
        m_rotate_y *= factor;
        m_angleRad = 0.0f;
        m_axis = normal;
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
// DRAW FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void ofxManipulator::drawCircle(Urho3D::Vector3 origin, Urho3D::Vector3 vtx, Urho3D::Vector3 vty, Urho3D::Color color)
{
    const int size = 50;
    std::vector<Urho3D::Vector3> vertices;
    vertices.resize(size);
    for (int i = 0; i < size; i++) {
        Urho3D::Vector3 vt;
        vt = vtx * cos((2 * PI / size) * i);
        vt += vty * sin((2 * PI / size) * i);
        vt += origin;
        vertices[i] = vt;
    }
    
    // Line loop
    for (int i = 0; i < size - 1; ++i)
        debugRenderer_->AddLine(vertices[i], vertices[i + 1], color, false);
    debugRenderer_->AddLine(vertices.back(), vertices.front(), color, false);
}

void ofxManipulator::drawAxis(Urho3D::Vector3 origin, Urho3D::Vector3 m_axis, Urho3D::Vector3 vtx, Urho3D::Vector3 vty, float fct, float fct2, Urho3D::Color color)
{
    std::vector<Urho3D::Vector3> cone_mesh;
    cone_mesh.resize(93);
    for (int i = 0, j = 0; i <= 30; i++) {
        Urho3D::Vector3 pt;
        pt = vtx * cos(((2 * PI) / 30.0f) * i) * fct;
        pt += vty * sin(((2 * PI) / 30.0f) * i) * fct;
        pt += m_axis * fct2;
        pt += origin;
        cone_mesh[j++] = pt;
        pt = vtx * cos(((2 * PI) / 30.0f) * (i + 1)) * fct;
        pt += vty * sin(((2 * PI) / 30.0f) * (i + 1)) * fct;
        pt += m_axis * fct2;
        pt += origin;
        cone_mesh[j++] = pt;
        cone_mesh[j++] = origin + m_axis;
    }

    // Line
    debugRenderer_->AddLine(origin, origin + m_axis, color, false);
    // Triangle fan cone
    for (int i = 2; i < cone_mesh.size(); ++i)
        debugRenderer_->AddTriangle(cone_mesh.front(), cone_mesh[i - 1], cone_mesh[i], color, false);
}

static const Urho3D::Vector3 BoxVertices[] = {
    // Top
    Urho3D::Vector3(-0.2f, 0.2f, 0.2f),     // front left
    Urho3D::Vector3(-0.2f, 0.2f, -0.2f),    // back left
    Urho3D::Vector3(0.2f, 0.2f, 0.2f),      // front right
    Urho3D::Vector3(0.2f, 0.2f, -0.2f),     // back right
    // Bottom
    Urho3D::Vector3(-0.2f, -0.2f, 0.2f),    // front left
    Urho3D::Vector3(-0.2f, -0.2f, -0.2f),   // back left
    Urho3D::Vector3(0.2f, -0.2f, 0.2f),     // front right
    Urho3D::Vector3(0.2f, -0.2f, -0.2f),    // back right
};

#define QUAD(A, B, C, D, COLOR) debugRenderer_->AddTriangle((A) * box_size, (B) * box_size, (C) * box_size, COLOR, false); debugRenderer_->AddTriangle((C) * box_size, (B) * box_size, (D) * box_size, COLOR, false);

void ofxManipulator::drawScaleAxis(Urho3D::Vector3 origin, Urho3D::Vector3 m_axis, Urho3D::Vector3 vtx, Urho3D::Color color)
{
    debugRenderer_->AddLine(origin, origin + m_axis, color, false);

    float box_size = 0.1f * m_screenFactor;

    Urho3D::Vector3 box_pos = origin;
    box_pos += m_axis.Normalized() * (m_axis.Length() - (box_size * 0.5f));

    box_size = 1.0f;
    // Top
    QUAD(box_pos + BoxVertices[0], box_pos + BoxVertices[1], box_pos + BoxVertices[2], box_pos + BoxVertices[3], color);
    // Bottom
    QUAD(box_pos + BoxVertices[4], box_pos + BoxVertices[5], box_pos + BoxVertices[6], box_pos + BoxVertices[7], color);
    // Left
    QUAD(box_pos + BoxVertices[0], box_pos + BoxVertices[1], box_pos + BoxVertices[4], box_pos + BoxVertices[5], color);
    // Right
    QUAD(box_pos + BoxVertices[2], box_pos + BoxVertices[3], box_pos + BoxVertices[6], box_pos + BoxVertices[7], color);
    // Front
    QUAD(box_pos + BoxVertices[0], box_pos + BoxVertices[2], box_pos + BoxVertices[4], box_pos + BoxVertices[6], color);
    // Back
    QUAD(box_pos + BoxVertices[1], box_pos + BoxVertices[3], box_pos + BoxVertices[5], box_pos + BoxVertices[7], color);

    //TODO: draw a solid box ofNode box_node;
    //TODO: draw a solid box box_node.lookAt(m_axis, vtx);
    //TODO: draw a solid box box_node.setPosition(box_pos);
    //TODO: draw a solid box ofPushMatrix();
    //TODO: draw a solid box ofMultMatrix(box_node.getGlobalTransformMatrix());
    //TODO: draw a solid box ofDrawBox(box_size);
    //TODO: draw a solid box ofPopMatrix();
}

void ofxManipulator::drawCamembert(Urho3D::Vector3 origin, Urho3D::Vector3 vtx, Urho3D::Vector3 vty, float ng, Urho3D::Color color)
{
    std::vector<Urho3D::Vector3> vertices;
    vertices.resize(52);
    int j = 0;
    vertices[j++] = origin;
    for (int i = 0; i <= 50; i++) {
        Urho3D::Vector3 vt;
        vt = vtx * cos((ng / 50) * i);
        vt += vty * sin((ng / 50) * i);
        vt += origin;
        vertices[j++] = vt;
    }

    color.a_ = 0.5f;

    // Triangle fan
    for (int i = 2; i < vertices.size(); ++i)
        debugRenderer_->AddTriangle(vertices.front(), vertices[i - 1], vertices[i], color, false);

    // Line loop?
    //ofMesh(OF_PRIMITIVE_LINE_LOOP, vertices).draw();
}

void ofxManipulator::drawQuad(Urho3D::Vector3 origin, float size, bool is_selected, Urho3D::Vector3 m_axis_u, Urho3D::Vector3 m_axis_v, Urho3D::Color color)
{
    origin += ((m_axis_u + m_axis_v) * size) * 0.2f;
    m_axis_u *= 0.8f;
    m_axis_v *= 0.8f;

    std::vector<Urho3D::Vector3> pts;
    pts.resize(4);
    pts[0] = origin;
    pts[1] = origin + (m_axis_u * size);
    pts[2] = origin + (m_axis_u + m_axis_v) * size;
    pts[3] = origin + (m_axis_v * size);

    color.a_ = is_selected ? 0.8f : 0.4f;
    
    // Triangle fan
    for (int i = 2; i < pts.size(); ++i)
        debugRenderer_->AddTriangle(pts.front(), pts[i - 1], pts[i], color, false);
    
    // Line loop border
    color.a_ = (is_selected ? 1.0f : 0.8f);
    debugRenderer_->AddLine(pts[0], pts[1], color, false);
    debugRenderer_->AddLine(pts[1], pts[2], color, false);
    debugRenderer_->AddLine(pts[2], pts[3], color, false);
    debugRenderer_->AddLine(pts[3], pts[0], color, false);
}

void ofxManipulator::drawTriangle(Urho3D::Vector3 origin, float size, bool is_selected, Urho3D::Vector3 m_axis_u, Urho3D::Vector3 m_axis_v, Urho3D::Color color)
{
    origin += ((m_axis_u + m_axis_v) * size) * 0.2f;
    m_axis_u *= 0.8f;
    m_axis_v *= 0.8f;

    std::vector<Urho3D::Vector3> pts;
    pts.resize(3);
    pts[0] = origin;
    pts[1] = (m_axis_u * size) + origin;
    pts[2] = (m_axis_v * size) + origin;

    // Triangles
    color.a_ = is_selected ? 0.8f : 0.4f;
    for (int i = 0; i < pts.size(); i += 3)
        debugRenderer_->AddTriangle(pts[i], pts[i + 1], pts[i + 2], color, false);
    
    // Line loop
    color.a_ = (is_selected ? 1.0f : 0.8f);
    debugRenderer_->AddLine(pts[0], pts[1], color, false);
    debugRenderer_->AddLine(pts[1], pts[2], color, false);
    debugRenderer_->AddLine(pts[0], pts[2], color, false);
}

bool ofxManipulator::rayQuad(Urho3D::Ray ray, Urho3D::Vector3 origin, float size, Urho3D::Vector3 axis_u, Urho3D::Vector3 axis_v)
{
    origin += ((axis_u + axis_v) * size) * 0.2f;
    axis_u *= 0.8f;
    axis_v *= 0.8f;

    std::vector<Urho3D::Vector3> pts;
    pts.resize(4);
    pts[0] = origin;
    pts[1] = origin + (axis_u * size);
    pts[2] = origin + (axis_u + axis_v) * size;
    pts[3] = origin + (axis_v * size);

    float hitDistance = std::min(ray.HitDistance(pts[0], pts[1], pts[2]), ray.HitDistance(pts[0], pts[2], pts[3]));
    if (hitDistance == Urho3D::M_INFINITY)
        return false;
    return true;
}

bool ofxManipulator::rayTriangle(Urho3D::Ray ray, Urho3D::Vector3 origin, float size, Urho3D::Vector3 axis_u, Urho3D::Vector3 axis_v)
{
    origin += ((axis_u + axis_v) * size) * 0.2f;
    axis_u *= 0.8f;
    axis_v *= 0.8f;

    std::vector<Urho3D::Vector3> pts;
    pts.resize(3);
    pts[0] = origin;
    pts[1] = (axis_u * size) + origin;
    pts[2] = (axis_v * size) + origin;

    float hit = ray.HitDistance(pts[0], pts[1], pts[2]);
    if (hit == Urho3D::M_INFINITY)
        return false;
    return true;
}

void ofxManipulator::getAxes(Urho3D::Vector3& x, Urho3D::Vector3& y, Urho3D::Vector3& z, bool forceLocal)
{
    if (m_localSpace || m_currManipulator == MANIPULATOR_AXIAL || forceLocal)
    {
        Urho3D::Matrix3x4 m(Urho3D::Vector3(), m_rotation, 1.0f);
        x = m * Urho3D::Vector3::RIGHT;
        y = m * Urho3D::Vector3::UP;
        z = m * Urho3D::Vector3::FORWARD;
    }
    else
    {
        x = Urho3D::Vector3::RIGHT;
        y = Urho3D::Vector3::UP;
        z = Urho3D::Vector3::FORWARD;
    }
}