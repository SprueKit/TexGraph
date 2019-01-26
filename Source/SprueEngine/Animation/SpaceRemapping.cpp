#include "SpaceRemapping.h"

#include <SprueEngine/Math/MathDef.h>

namespace SprueEngine
{

    Mat3x4 ToGeneralizedSpace(Remapping remap, const Mat3x4& input, const Mat3x4& restPos, const Mat3x4* reference, const BoundingBox& bounds, const float limbLength)
    {
        Vec3 remappedPosition;
        Quat remappedRotation;
        const Vec3 extents = bounds.Size();
        const Vec3 halfDim = bounds.HalfSize();
        
        const Vec3 refPt = reference ? reference->TranslatePart() : Vec3();
        const Vec3 restPt = restPos.TranslatePart();
        const Vec3 localPt = input.TranslatePart();

        const Quat refRot = reference ? reference->RotatePart().ToQuat() : Quat();
        const Quat restRot = restPos.RotatePart().ToQuat();
        const Quat localRot = input.RotatePart().ToQuat();

        if (remap & REMAP_Explicit) // Used as is
        {
            remappedPosition = refPt;
        }
        
        if (remap & REMAP_ForeignRelative) // G = Of - R
        {
            Vec3 direction = localPt - refPt;
            remappedPosition = NORMALIZE(direction, restPt, refPt);
        } 
        else if (remap & REMAP_RestRelative) // G = P-R
        {
            Vec3 direction = localPt - restPt;
            remappedPosition = NORMALIZE(direction, -halfDim, halfDim);
        }
        else if (remap & REMAP_LookAt) // G = Normalized(P - R)
        {
            //??? do nothing?
        }

        if (remap & REMAP_GroundRelative) // Gy = (Py - Ry) / -Ry
            remappedPosition.y = (localPt.y - restPt.y) / -restPt.y;

        if (remap & REMAP_ScaledLength) // G = P / LimbLength
            remappedPosition /= limbLength;
        else if (remap & REMAP_ScaledSize) // G = P / BoundsExtent
            remappedPosition /= extents;

        return Mat3x4::FromTRS(remappedPosition, remappedRotation, Vec3::one);
    }
    
    Mat3x4 ToSpecializedSpace(Remapping remap, const Mat3x4& input, const Mat3x4& restPos, const Mat3x4* reference, const BoundingBox& bounds, const float limbLength)
    {
        Vec3 remappedPosition;
        Quat remappedRotation;
        const Vec3 extents = bounds.Size();
        const Vec3 halfDim = bounds.HalfSize();
        const Vec3 refPt = reference ? reference->TranslatePart() : Vec3();
        const Vec3 restPt = restPos.TranslatePart();
        const Vec3 localPt = input.TranslatePart();

        if (remap & REMAP_Explicit)
        {

        }

        if (remap & REMAP_ForeignRelative)
        {
            Vec3 direction = localPt + refPt;
            remappedPosition = DENORMALIZE(direction, restPt, refPt);
        }
        else if (remap & REMAP_RestRelative)
        {
            Vec3 direction = localPt + restPt;
            remappedPosition = DENORMALIZE(direction, -halfDim, halfDim);
        }
        else if (remap & REMAP_LookAt)
        {
            //??? do nothing?
        }

        if (remap & REMAP_GroundRelative) // Sy = -((GyRy)-Ry)
            remappedPosition.y = -((localPt.y * restPt.y)*(-restPt.y));

        if (remap & REMAP_ScaledLength)
            remappedPosition *= limbLength;
        else if (remap & REMAP_ScaledSize)
            remappedPosition *= extents;

        return Mat3x4::FromTRS(remappedPosition, remappedRotation, Vec3::one);
    }

    Vec3 PositionToGeneralizedSpace(SpacialPosition* data, const Mat3x4& restPose, const BoundingBox& bounds)
    {
        const Vec3 posePosition = restPose.TranslatePart();
        Vec3 remappedPosition = data->position_;
        float limbLength = 1.0f;
        Vec3 foreignPosition;

        const Vec3 size = bounds.Size();
        
        // Ground relative remapping
        if (data->remapPosition_ & REMAP_GroundRelative)
            remappedPosition.y = (remappedPosition.y - posePosition.y) / -posePosition.y;

        // Core position deterimation
        if (data->remapPosition_ & REMAP_Explicit)
            remappedPosition = posePosition + data->position_;
        else if (data->remapPosition_ & REMAP_RestRelative) // G = P-R
            remappedPosition = data->position_ - posePosition;
        else if (data->remapPosition_ & REMAP_ForeignRelative) // G = Of - R
            remappedPosition = NORMALIZE(data->position_, posePosition, foreignPosition);

        // Scaling factors
        if (data->remapPosition_ & REMAP_ScaledLength)
            remappedPosition = remappedPosition / limbLength;
        else if (data->remapPosition_ & REMAP_ScaledSize)
            remappedPosition /= size;

        return remappedPosition;
    }
        
    Quat RotationToGeneralizedSpace(SpacialPosition* data, const Mat3x4& restPose, const BoundingBox& bounds)
    {
        const Quat poseRotation = restPose.RotatePart().ToQuat();
        Quat remappedRotation;
        float limbLength = 1.0f;

        if (data->remapRotation_ & REMAP_Explicit)
            remappedRotation = data->rotation_;
        else if (data->remapRotation_ & REMAP_RestRelative)
            remappedRotation = poseRotation * data->rotation_.Inverted();
        // Foreign relative has no meaning

        // Scaling factors
        // Bounds scaling doesn't make sense for rotation
        if (data->remapRotation_ & REMAP_ScaledLength)
            remappedRotation = Quat::identity.Slerp(remappedRotation, 1.0f / limbLength);
        
        // Ground relative scaling doesn't make sense for rotation

        return remappedRotation;
    }

    Mat3x4 ToGeneralizedSpace(SpacialPosition* data, const Mat3x4& restPose, const BoundingBox& bounds)
    {
        Vec3 position = restPose.TranslatePart();
        Quat rotation = restPose.RotatePart().ToQuat();
        if (data->remapPosition_)
            position = PositionToGeneralizedSpace(data, restPose, bounds);
        if (data->remapRotation_)
            rotation = RotationToGeneralizedSpace(data, restPose, bounds);
        return Mat3x4(rotation, position);
    }

    Vec3 PositionToSpecializedSpace(SpacialPosition* data, const Mat3x4& restPose, const BoundingBox& bounds)
    {
        Vec3 generalizedPosition = data->position_;
        Vec3 specializedPosition = generalizedPosition;
        Vec3 posePosition = restPose.TranslatePart();
        float limbLength = 1;
        Vec3 foreignPosition;

        if (data->remapPosition_ & REMAP_ScaledLength)
            specializedPosition = specializedPosition * limbLength;
        else if (data->remapPosition_ & REMAP_ScaledSize)
            specializedPosition *= bounds.Size();

        if (data->remapPosition_ & REMAP_RestRelative)
            specializedPosition = specializedPosition + posePosition;
        else if (data->remapPosition_ & REMAP_ForeignRelative)
            specializedPosition = DENORMALIZE(specializedPosition, posePosition, foreignPosition);

        if (data->remapPosition_ & REMAP_GroundRelative) // Sy = -((GyRy)-Ry)
            specializedPosition.y = -((generalizedPosition.y * posePosition.y) - posePosition.y);

        return specializedPosition;
    }

    Quat RotationToSpecializedSpace(SpacialPosition* data, const Mat3x4& restPose, const BoundingBox& bounds)
    {
        const Quat poseRotation = restPose.RotatePart().ToQuat();
        Quat remappedRotation = data->rotation_;
        float limbLength = 1.0f;

        if (data->remapRotation_ & REMAP_ScaledLength)
            remappedRotation = Quat::identity.Slerp(remappedRotation, limbLength);

        if (data->remapRotation_ & REMAP_RestRelative)
            remappedRotation = remappedRotation * poseRotation;

        // Look-at overrides everything else
        if (data->remapRotation_ & REMAP_LookAt)
        {
            // TODO
        }

        return remappedRotation;
    }

    Mat3x4 ToSpecializedSpace(SpacialPosition* data, const Mat3x4& restPose, const BoundingBox& bounds)
    {
        Vec3 position = restPose.TranslatePart();
        Quat rotation = restPose.RotatePart().ToQuat();
        if (data->remapPosition_)
            position = PositionToSpecializedSpace(data, restPose, bounds);
        if (data->remapRotation_)
            rotation = RotationToSpecializedSpace(data, restPose, bounds);

        return Mat3x4(rotation, position);
    }
}