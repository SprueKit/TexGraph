#pragma once

#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{

struct FABRIKChain;
struct FABRIKJoint;
struct FABRIKRig;
class Joint;
class Skeleton;

struct FABRIKBone
{
    FABRIKBone(FABRIKJoint* joint, const Vec3& start, const Vec3& end) : 
        joint_(joint),
        startLocation_(start),
        endLocation_(end)
    {
        length_ = (end - start).Length();
    }

    ~FABRIKBone();

    FABRIKJoint* joint_ = 0x0;
    FABRIKChain* chain_ = 0x0;
    Vec3 startLocation_;
    Vec3 endLocation_;
    /// Length of the bone.
    float length_;
    /// Identifier for where this FABRIK bone came from.
    unsigned sourceID_;
    /// Identifier for the actual joint that is at the end.
    unsigned endSourceID_;

    void UpdateFrom(Skeleton* joint);

    Vec3 GetDirection() const { return (endLocation_ - startLocation_).Normalized(); }
};

enum FABRIKJointType
{
    FJT_BALL,
    FJT_GLOBAL_HINGE,
    FJT_LOCAL_HINGE
};

/// Data for a joint and its' constraints
struct FABRIKJoint
{
    // Limits
    static const float MinConstraintAngles;
    static const float MaxConstraintAngles;

    float constraintDegrees_;       // Ball constraint
    float cwConstraintDegrees_;     // Hinge constraint
    float ccwConstraintDegrees_;    // Hinge constraint
    Vec3 axis_;                     // Axis of rotation for the hinge constraint
    Vec3 referenceAxis_;            // Reference axis for hinge constraint
    FABRIKJointType jointType_;     // What type of joint it is
    int chainIndex_;

    /// Utility function for initializing as a ball joint.
    void MakeIntoBall(float constraintAngle);
    /// Utility function for initializing as a hinge joint.
    void MakeIntoHinge(FABRIKJointType type, Vec3 rotAxis, Vec3 refAxis, float cwConstraint, float ccwConstraint);
    /// Specialization of above utility function for initializing as a global hinge.
    void MakeIntoGlobalHinge(Vec3 rotAxis, Vec3 refAxis, float cwConstraint, float ccwConstraint);
    /// Specialization of above utility function for initializing as a local hinge.
    void MakeIntoLocalHinge(Vec3 rotAxis, Vec3 refAxis, float cwConstaint, float ccwConstraint);
};

enum FABRIKBaseConstraintType
{
    FBCT_NONE,
    FBCT_GLOBAL_ROTOR,
    FBCT_LOCAL_ROTOR,
    FBCT_GLOBAL_HINGE,
    FBCT_LOCAL_HINGE
};

struct FABRIKChain
{   
    FABRIKChain();
    ~FABRIKChain();

    void SetRig(FABRIKRig* rig) { rig_ = rig; }
    
    void ParentTo(FABRIKBone* joint);

    void InsertBone(FABRIKBone* bone);
    /// Adds a bone to the IK chain.
    void AddBone(FABRIKBone* bone);
    /// Updates the target position then solves the IK.
    float UpdateTarget(const Vec3& newTarget);

    void SetTarget(const Vec3& target) { isTargetActive_ = true; target_ = target; }
    void ClearTarget() { isTargetActive_ = false; }
    
    /// Solves IK for the embedded target.
    float SolveIK() { if (isTargetActive_) return UpdateTarget(target_); }
    /// Calculates the new otpimium chain configuration for a specific target.
    float SolveIKForTarget(const Vec3& target);
    /// Recalculates the length of the chain.
    void UpdateChainLength();
    /// Update the base position for the root of the chain.
    void UpdateBasePosition(const Vec3& newBasePosition, bool fix) { fixedBase_ = newBasePosition; isFixedBase_ = fix; }

    float GetChainLength() { return chainLength_; }
    float RecalculateChainLength();

    FABRIKBone* GetFirstBone() { return bones_.front(); }
    FABRIKBone* GetLastBone() { return bones_.back(); }

    FABRIKChain* GetConnectedChain() { return connectedTo_; }
    FABRIKBone* GetConnectedToBone() { return connectedToBone_; }

    Vec3 baseBoneConstraintDir_;
    Vec3 baseBoneConstraintReference_;
    FABRIKBaseConstraintType baseConstraint_ = FBCT_NONE;
    float solveThreshold_;
    float solvingDistance_;
    float chainLength_;
    float minIterationAdjust_;
    int maxIterations_;

private:
    std::vector<FABRIKBone*> bones_;

    FABRIKRig* rig_;
    FABRIKChain* connectedTo_;
    FABRIKBone* connectedToBone_;

    Vec3 target_;
    bool isTargetActive_;
    Vec3 lastTarget_;
    Vec3 lastBase_;
    Vec3 fixedBase_;
    bool isFixedBase_;
};

/// Manages and updates collection of chains
struct FABRIKRig
{
    /// Construct an empty rig.
    FABRIKRig();
    /// Construct a FABRIK rig off of an existing skeleton.
    FABRIKRig(Skeleton*);
    /// Destruct.
    ~FABRIKRig();

    /// Add a chain to this rig.
    void AddChain(FABRIKChain* chain);
    /// Solves IK for all chains in the rig.
    void SolveIK();

private:
    void ProcessBranch(Skeleton* skeleton, FABRIKChain* chain, FABRIKBone* lastBone, Joint* joint);

    std::vector<FABRIKChain*> chains_;
};

}