#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{
    /// P, point
    /// R, rest position of body
    /// Of, original foreign body
    /// Nf, new foreign body
    /// G, generalization function
    /// S, specialization function
    /// Not all settings apply to all
    enum RemappingMode
    {
        REMAP_Ignore = 0,
        REMAP_Explicit = 1,             // Character relative, ie. for moving the root (jumping, etc)
        REMAP_RestRelative = 1 << 1,    // Character relative
                                            // G = P-R
                                            // S = R+P
        REMAP_ScaledSize = 1 << 2,      // Scaled based on creature size
                                            // G = P / BoundsExtent
                                            // S = P * BoundsExtent
        REMAP_ScaledLength = 1 << 3,    // Scaled based on distance between the body and the nearest spine joint, if parented to root then the nearest bone available is chosen
                                            // G = P / LimbLength
                                            // S = P * LimbLength
        REMAP_GroundRelative = 1 << 4,  // 1.0 = floor, 0.0 = rest pose, can combine with others
                                            // Gy = (Py - Ry) / -Ry
                                            // Sy = -((GyRy)-Ry)
        REMAP_SagitalRelative = 1 << 5, // 0.0 = sagital plane, 1.0 equals +X bounding box face, -1.0f equals -X bounding box face
                                            // Gz = Normalize(Pz, 0, halfBoundsZ)
                                            // Sz = Denormalize(Pz, 0, halfBoundsZ)
        REMAP_ForeignRelative = 1 << 6, // Relative to another position
                                            // G = Normalize(P, R, Of)      // Of - R
                                            // S = Denormalize(P, R, Nf)    // R + Nf
        REMAP_LookAt = 1 << 7,          // Look at the target
                                            // G = Normalized(P - R)
                                            // S = R + P * ARBITRARY_CONSTANT
    };
    typedef unsigned char Remapping;

    /// Data that will be transformed from generalized <-> specialized space
    struct SPRUE SpacialPosition // 30 bytes
    {
        Vec3 position_;
        Quat rotation_;
        Remapping remapPosition_; // How the position component is used, or not
        Remapping remapRotation_; // How the rotation component is used, or not
    };

    Mat3x4 ToGeneralizedSpace(SpacialPosition* data, const Mat3x4& restPose, const BoundingBox& bounds);
    Mat3x4 ToSpecializedSpace(SpacialPosition* data, const Mat3x4& restPose, const BoundingBox& bounds);

    Mat3x4 ToGeneralizedSpace(Remapping remap, const Mat3x4&, const Mat3x4& restPos, const Mat3x4* reference, const BoundingBox& bounds);
    Mat3x4 ToSpecializedSpace(Remapping remap, const Mat3x4&, const Mat3x4& restPos, const Mat3x4* reference, const BoundingBox& bounds);

}