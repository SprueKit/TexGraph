#include "Manipulator.h"

#include <Urho3D/Math/Ray.h>
#include <Urho3D/Math/Plane.h>

#include <memory>

using namespace Urho3D;

namespace ImManip
{
    static ManipulatorContext context_;

    static const Urho3D::Color InactiveAxisColors[] = {
        Urho3D::Color(1.0f, 0.25f, 0.2f),
        Urho3D::Color(0.18f, 0.8f, 0.25f),
        Urho3D::Color(0.0f, 0.45f, 0.85f),
        Urho3D::Color(1.0f, 0.25f, 1.0f)
    };
    static Urho3D::Color SelectedColor(1.0f, 0.86f, 0.0f);

    void ManipulatorContext::Init()
    {
        wasMouseDown[0] = wasMouseDown[1] = wasMouseDown[2] = false;
        mouseButtonsDown[0] = mouseButtonsDown[1] = mouseButtonsDown[2] = false;

        localSpace = false;
        xDelta = 0.0f;
        yDelta = 0.0f;
        xPos = 0.0f;
        yPos = 0.0f;

        transform_ = Matrix3x4::IDENTITY;
        parentTransform_ = Matrix3x4::IDENTITY;
        oldTransform_ = Matrix3x4::IDENTITY;

        previousHandleID = -1;
        nextID = 0;
    }

    void ManipulatorContext::Begin()
    {
        
    }

    void ManipulatorContext::End()
    {
        wasMouseDown[0] = mouseButtonsDown[0];
        wasMouseDown[1] = mouseButtonsDown[1];
        wasMouseDown[2] = mouseButtonsDown[2];
    }

    bool Transform(bool scaled)
    {        
        Urho3D::Vector3 trans, scl;
        Urho3D::Quaternion rot;
        context_.transform_.Decompose(trans, rot, scl);

        bool anyChanges = Box(trans, 0.5f, true);

        // X axis
        anyChanges |= AxisTrack(trans, Urho3D::Vector3::RIGHT, true);
        // Y axis
        anyChanges |= AxisTrack(trans, Urho3D::Vector3::UP, true);
        // Z axis
        anyChanges |= AxisTrack(trans, Urho3D::Vector3::FORWARD, true);

        return anyChanges;
    }

    bool AxisTrack(Urho3D::Vector3 origin, const Urho3D::Vector3 dir, bool scaled)
    {
        int id = context_.nextID++;

        Urho3D::Ray r(origin, dir.Normalized());
        auto projectedPt = r.ClosestPoint(context_.mouseRay_);

        Urho3D::Plane plane(dir, origin);

        // on the right side of our plane
        if (plane.Distance(projectedPt) > 0.0f)
        {
            float dist = (context_.mouseRay_.ClosestPoint(r) - projectedPt).Length();
            if (dist < 1.0f && !context_.wasMouseDown[0])
            {
                context_.previousHandleID = id;
                //hit
            }
        }

        return false;
    }
    
    bool AxisPoint(Urho3D::Vector3 origin, const Urho3D::Vector3 dir, bool scaled)
    {
        int id = context_.nextID++;

        return false;
    }
    
    bool AxisSheet(Urho3D::Vector3 origin, int axis, float offset)
    {
        int id = context_.nextID++;

        return false;
    }
    
    bool Box(Urho3D::Vector3 origin, float size, bool scaled)
    {
        int id = context_.nextID++;

        return false;
    }

    bool Camembert(Urho3D::Vector3 origin, Urho3D::Vector3 dir, bool scaled)
    {
        int id = context_.nextID++;

        return false;
    }
}