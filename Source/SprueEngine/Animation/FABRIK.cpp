#include "FABRIK.h"

#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/Geometry/Skeleton.h>

namespace SprueEngine
{

const float FABRIKJoint::MinConstraintAngles = 0.0f;
const float FABRIKJoint::MaxConstraintAngles = 180.0f;

FABRIKBone::~FABRIKBone()
{
    if (joint_)
        delete joint_;
}

void FABRIKBone::UpdateFrom(Skeleton* skeleton)
{
    auto bones = skeleton->GetAllJoints();
    startLocation_ = bones[sourceID_]->GetPosition();
    endLocation_ = bones[endSourceID_]->GetPosition();
    length_ = (endLocation_ - startLocation_).Length();
}

FABRIKChain::FABRIKChain()
{

}

FABRIKChain::~FABRIKChain()
{
    for (auto bone : bones_)
        delete bone;
}

void FABRIKChain::InsertBone(FABRIKBone* bone)
{
    bones_.insert(bones_.begin(), bone);
    bone->chain_ = this;
    UpdateChainLength();
}

void FABRIKChain::AddBone(FABRIKBone* bone)
{
    bones_.push_back(bone);
    bone->chain_ = this;
    UpdateChainLength();
}

void FABRIKChain::ParentTo(FABRIKBone* bone)
{
    connectedTo_ = bone->chain_;
    connectedToBone_ = bone;
}

float FABRIKChain::UpdateTarget(const Vec3& target)
{
    if (SprueEquals(lastTarget_.x, target.x) && SprueEquals(lastTarget_.y, target.y) && SprueEquals(lastTarget_.z, target.z))
        return this->solvingDistance_;
    
    float bestSolveDist = FLT_MAX;
    float lastSolveDistance = FLT_MAX;
    for (int loop = 0; loop < maxIterations_; ++loop)
    {
        solvingDistance_ = SolveIKForTarget(target);
        if (solvingDistance_ < bestSolveDist)
        {
            
            if (solvingDistance_ < solveThreshold_)
                break;
        }
        else
        {
            if (fabsf(solvingDistance_ - lastSolveDistance) < minIterationAdjust_)
                break;

        }
        lastSolveDistance = solvingDistance_;
    }

    solvingDistance_ = bestSolveDist;
    //lastBase_ = GetBase();
    lastTarget_ = target;

    return solvingDistance_;
}

float FABRIKChain::SolveIKForTarget(const Vec3& target)
{
    if (bones_.size() == 0) 
        return FLT_MAX;

// BACKWARD PASS

    for (int loop = bones_.size() - 1; loop >= 0; --loop)
    {
        FABRIKBone* thisBone = bones_[loop];
        const float thisBoneLength = thisBone->length_;
        FABRIKJoint* thisBoneJoint = thisBone->joint_;
        FABRIKJointType thisBoneJointType = thisBone->joint_->jointType_;

        if (loop != bones_.size() - 1)
        {
            Vec3 outerBoneOuterToInnerUV = -bones_[loop + 1]->GetDirection();
            Vec3 thisBoneOuterToInnerUV = -thisBone->GetDirection();

            if (thisBoneJointType == FABRIKJointType::FJT_BALL)
            {
                // Constrain to relative angle between this bone and the outer bone if required
                float angleBetweenDegs = outerBoneOuterToInnerUV.AngleDeg(thisBoneOuterToInnerUV);
                float constraintAngleDegs = thisBoneJoint->constraintDegrees_;
                if (angleBetweenDegs > constraintAngleDegs)
                    thisBoneOuterToInnerUV = thisBoneOuterToInnerUV.AngleLimitedDeg(outerBoneOuterToInnerUV, constraintAngleDegs);
            }
            else if (thisBoneJointType == FABRIKJointType::FJT_GLOBAL_HINGE)
                thisBoneOuterToInnerUV = thisBoneOuterToInnerUV.ProjectOntoPlane(thisBoneJoint->axis_);
            else if (thisBoneJointType == FABRIKJointType::FJT_LOCAL_HINGE)
            {
                Quat quat = Quat::LookAt(Vec3::PositiveZ, bones_[loop - 1]->GetDirection(), Vec3::PositiveY, Vec3::PositiveY);
                Vec3 relativeHingeRotationAxis;
                if (loop > 0)
                    relativeHingeRotationAxis = quat * (thisBoneJoint->axis_).Normalized();
                else
                    relativeHingeRotationAxis = baseBoneConstraintDir_;
                thisBoneOuterToInnerUV = thisBoneOuterToInnerUV.ProjectOntoPlane(relativeHingeRotationAxis);
            }
            Vec3 newStartLocation = thisBone->endLocation_ + thisBoneOuterToInnerUV * thisBoneLength;
            thisBone->startLocation_ = newStartLocation;
            if (loop > 0)
                bones_[loop - 1]->endLocation_ = newStartLocation;
        }
        else
        {
        // End bone of the system
            thisBone->endLocation_ = target;
            Vec3 thisBoneOuterToInnerUV = -thisBone->GetDirection();
            switch (thisBoneJointType)
            {
            case FJT_BALL:
                break;
            case FJT_GLOBAL_HINGE:
                thisBoneOuterToInnerUV = thisBoneOuterToInnerUV.ProjectOntoPlane(thisBoneJoint->axis_);
                break;
            case FJT_LOCAL_HINGE:
                Quat m = Quat::LookAt(Vec3::PositiveZ, bones_[loop - 1]->GetDirection(), Vec3::PositiveY, Vec3::PositiveY);
                Vec3 relativeHingeRotationAxis = m * (thisBoneJoint->axis_).Normalized();
                thisBoneOuterToInnerUV = thisBoneOuterToInnerUV.ProjectOntoPlane(relativeHingeRotationAxis);
                break;
            }

            Vec3 newStartLocation = target + (thisBoneOuterToInnerUV * thisBoneLength);
            thisBone->startLocation_ = newStartLocation;

            if (loop > 0)
                bones_[loop - 1]->endLocation_ = newStartLocation;
        }

    }

// FORWARD PASS

    for (int loop = 0; loop < bones_.size(); ++loop)
    {
        FABRIKBone* thisBone = bones_[loop];
        float thisBoneLength = thisBone->length_;
        if (loop != 0)
        {
            Vec3 thisBoneInnerToOuterUV = thisBone->GetDirection();
            Vec3 prevBoneInnerToOuterUV = bones_[loop - 1]->GetDirection();
            FABRIKJoint* thisBoneJoint = thisBone->joint_;
            FABRIKJointType jointType = thisBoneJoint->jointType_;
            if (jointType == FABRIKJointType::FJT_BALL)
            {
                float angleBetweenDegs = prevBoneInnerToOuterUV.AngleDeg(thisBoneInnerToOuterUV);
                float constraintAngleDegs = thisBoneJoint->constraintDegrees_;
                if (angleBetweenDegs > constraintAngleDegs)
                    thisBoneInnerToOuterUV = thisBoneInnerToOuterUV.AngleLimitedDeg(prevBoneInnerToOuterUV, constraintAngleDegs);
            }
            else if (jointType == FABRIKJointType::FJT_GLOBAL_HINGE)
            {
                Vec3 hingeRotationAxis = thisBoneJoint->axis_;
                thisBoneInnerToOuterUV = thisBoneInnerToOuterUV.ProjectOntoPlane(hingeRotationAxis);

                float cwConstraintDegs = -thisBoneJoint->cwConstraintDegrees_;
                float acwConstraintDegs = thisBoneJoint->ccwConstraintDegrees_;
                if (!(SprueEqualsApprox(cwConstraintDegs, -FABRIKJoint::MaxConstraintAngles, 0.001f)) && !(SprueEqualsApprox(acwConstraintDegs, FABRIKJoint::MaxConstraintAngles, 0.001f)))
                {
                    Vec3 hingeReferenceAxis = thisBoneJoint->referenceAxis_;
                    float signedAngleDegs = hingeReferenceAxis.SignedAngleDeg(thisBoneInnerToOuterUV, hingeRotationAxis);
                    if (signedAngleDegs > acwConstraintDegs)
                        thisBoneInnerToOuterUV = hingeReferenceAxis.RotateAroundAxis(acwConstraintDegs, hingeRotationAxis).Normalized();
                    else if (signedAngleDegs < cwConstraintDegs)
                        thisBoneInnerToOuterUV = hingeReferenceAxis.RotateAroundAxis(cwConstraintDegs, hingeRotationAxis).Normalized();
                }
            }
            else if (jointType == FABRIKJointType::FJT_LOCAL_HINGE)
            {
                Vec3 hingeRotationAxis = thisBoneJoint->axis_;
                Quat m = Quat::LookAt(Vec3::PositiveZ, prevBoneInnerToOuterUV, Vec3::PositiveY, Vec3::PositiveY);
                Vec3 relativeHingeRotationAxis = m * hingeRotationAxis.Normalized();
                thisBoneInnerToOuterUV = thisBoneInnerToOuterUV.ProjectOntoPlane(relativeHingeRotationAxis);
                float cwConstraintDegs = -thisBoneJoint->cwConstraintDegrees_;
                float acwConstraintDegs = thisBoneJoint->ccwConstraintDegrees_;
                if (!(SprueEqualsApprox(cwConstraintDegs, -FABRIKJoint::MaxConstraintAngles, 0.001f)) && !(SprueEqualsApprox(acwConstraintDegs, FABRIKJoint::MaxConstraintAngles, 0.001f)))
                {
                    Vec3 relativeHingeReferenceAxis = m * thisBoneJoint->axis_.Normalized();

                    float signedAngleDegs = relativeHingeReferenceAxis.SignedAngleDeg(thisBoneInnerToOuterUV, relativeHingeRotationAxis);

                    if (signedAngleDegs > acwConstraintDegs)
                        thisBoneInnerToOuterUV = relativeHingeReferenceAxis.RotateAroundAxis(acwConstraintDegs, relativeHingeRotationAxis).Normalized();
                    else if (signedAngleDegs < cwConstraintDegs)
                        thisBoneInnerToOuterUV = relativeHingeReferenceAxis.RotateAroundAxis(cwConstraintDegs, relativeHingeRotationAxis).Normalized();
                }
            }

            Vec3 newEndLocation = thisBone->startLocation_ + (thisBoneInnerToOuterUV * thisBoneLength);
            thisBone->endLocation_ = newEndLocation;
            if (loop < bones_.size() - 1) 
                bones_[loop + 1]->startLocation_ = newEndLocation;
        }
        else
        {
            if (isFixedBase_)
                thisBone->startLocation_ = fixedBase_;
            else
                thisBone->startLocation_ = thisBone->endLocation_ - (thisBone->GetDirection() * thisBoneLength);

            if (baseConstraint_ == FABRIKBaseConstraintType::FBCT_NONE)
            {
                Vec3 newEndLocation = thisBone->startLocation_ + (thisBone->GetDirection() * thisBoneLength);
                thisBone->endLocation_ = newEndLocation;
                if (bones_.size() > 1) 
                    bones_[1]->startLocation_ = newEndLocation;
            }
            else
            {
                if (baseConstraint_ == FABRIKBaseConstraintType::FBCT_GLOBAL_ROTOR)
                {
                    Vec3 thisBoneInnerToOuterUV = thisBone->GetDirection();
                    float angleBetweenDegs = baseBoneConstraintDir_.AngleDeg(thisBoneInnerToOuterUV);
                    float constraintAngleDegs = thisBone->joint_->constraintDegrees_;
                    if (angleBetweenDegs > constraintAngleDegs)
                        thisBoneInnerToOuterUV = thisBoneInnerToOuterUV.AngleLimitedDeg(baseBoneConstraintDir_, constraintAngleDegs);
                    Vec3 newEndLocation = thisBone->startLocation_ + (thisBoneInnerToOuterUV * thisBoneLength);
                    thisBone->endLocation_ = newEndLocation;
                    if (bones_.size() > 1) 
                        bones_[1]->startLocation_ = newEndLocation;
                }
                else if (baseConstraint_ == FABRIKBaseConstraintType::FBCT_GLOBAL_ROTOR)
                {
                    Vec3 thisBoneInnerToOuterUV = thisBone->GetDirection();

                    float angleBetweenDegs = baseBoneConstraintDir_.AngleDeg(thisBoneInnerToOuterUV);
                    float constraintAngleDegs = thisBone->joint_->constraintDegrees_;
                    if (angleBetweenDegs > constraintAngleDegs)
                        thisBoneInnerToOuterUV = thisBoneInnerToOuterUV.AngleLimitedDeg(baseBoneConstraintDir_, constraintAngleDegs);
                    Vec3 newEndLocation = thisBone->startLocation_ + (thisBoneInnerToOuterUV * thisBoneLength);
                    thisBone->endLocation_ = newEndLocation;
                    if (bones_.size() > 1) 
                        bones_[1]->startLocation_ = newEndLocation;
                }
                else if (baseConstraint_ == FABRIKBaseConstraintType::FBCT_LOCAL_HINGE)
                {
                    FABRIKJoint* thisJoint = thisBone->joint_;
                    Vec3 hingeRotationAxis = thisJoint->axis_;
                    float cwConstraintDegs = -thisJoint->cwConstraintDegrees_;
                    float acwConstraintDegs = thisJoint->ccwConstraintDegrees_;
                    Vec3 thisBoneInnerToOuterUV = thisBone->GetDirection().ProjectOntoPlane(hingeRotationAxis);
                    if (!(SprueEqualsApprox(cwConstraintDegs, FABRIKJoint::MaxConstraintAngles, 0.01f) && SprueEqualsApprox(acwConstraintDegs, FABRIKJoint::MaxConstraintAngles, 0.01f)))
                    {
                        Vec3 hingeReferenceAxis = thisJoint->referenceAxis_;
                        float signedAngleDegs = hingeReferenceAxis.SignedAngleDeg(thisBoneInnerToOuterUV, hingeRotationAxis);

                        if (signedAngleDegs > acwConstraintDegs)
                            thisBoneInnerToOuterUV = hingeReferenceAxis.RotateAroundAxis(acwConstraintDegs, hingeRotationAxis).Normalized();
                        else if (signedAngleDegs < cwConstraintDegs)
                            thisBoneInnerToOuterUV = hingeReferenceAxis.RotateAroundAxis(cwConstraintDegs, hingeRotationAxis).Normalized();
                    }
                    Vec3 newEndLocation = thisBone->startLocation_ + (thisBoneInnerToOuterUV * thisBoneLength);
                    thisBone->endLocation_ = newEndLocation;
                    if (bones_.size() > 1)
                        bones_[1]->startLocation_ = newEndLocation;
                }
                else if (baseConstraint_ == FABRIKBaseConstraintType::FBCT_LOCAL_HINGE)
                {
                    FABRIKJoint* thisJoint = thisBone->joint_;
                    Vec3 hingeRotationAxis = baseBoneConstraintDir_;
                    float cwConstraintDegs = -thisJoint->cwConstraintDegrees_;
                    float acwConstraintDegs = thisJoint->ccwConstraintDegrees_;
                    Vec3 thisBoneInnerToOuterUV = thisBone->GetDirection().ProjectOntoPlane(hingeRotationAxis);
                    if (!(SprueEqualsApprox(cwConstraintDegs, FABRIKJoint::MaxConstraintAngles, 0.01f) && SprueEqualsApprox(acwConstraintDegs, FABRIKJoint::MaxConstraintAngles, 0.01f)))
                    {
                        Vec3 hingeReferenceAxis = baseBoneConstraintDir_;
                        float signedAngleDegs = hingeReferenceAxis.SignedAngleDeg(thisBoneInnerToOuterUV, hingeRotationAxis);
                        if (signedAngleDegs > acwConstraintDegs)
                            thisBoneInnerToOuterUV = hingeReferenceAxis.RotateAroundAxis(acwConstraintDegs, hingeRotationAxis).Normalized();
                        else if (signedAngleDegs < cwConstraintDegs)
                            thisBoneInnerToOuterUV = hingeReferenceAxis.RotateAroundAxis(cwConstraintDegs, hingeRotationAxis).Normalized();
                    }

                    Vec3 newEndLocation = thisBone->startLocation_ + (thisBoneInnerToOuterUV * thisBoneLength);
                    thisBone->endLocation_ = newEndLocation;
                    if (bones_.size() > 1) 
                        bones_[1]->startLocation_ = newEndLocation;
                }
            }
        }
    }
    lastTarget_ = target;
    return (bones_[bones_.size() - 1]->endLocation_ - target).Length();
}

float FABRIKChain::RecalculateChainLength()
{
    float len = 0.0f;
    for (auto bone : bones_)
        len += bone->length_;
    return chainLength_;
}

FABRIKRig::FABRIKRig()
{

}

FABRIKRig::FABRIKRig(Skeleton* skeleton)
{
    auto root = skeleton->GetRootJoint();
    
    for (auto child : root->GetChildren())
    {
        FABRIKChain* chain = new FABRIKChain();
        auto bone = new FABRIKBone(new FABRIKJoint(), root->GetPosition(), child->GetPosition());
        bone->sourceID_ = skeleton->IndexOf(root);
        bone->endSourceID_ = skeleton->IndexOf(child);
        chain->AddBone(bone);

        ProcessBranch(skeleton, chain, bone, child);
        AddChain(chain);
    }
}

FABRIKRig::~FABRIKRig()
{
    for (auto chain : chains_)
        delete chain;
}

void FABRIKRig::AddChain(FABRIKChain* chain)
{
    chains_.push_back(chain);
    chain->SetRig(this);
}

void FABRIKRig::SolveIK()
{
    for (int i = 0; i < chains_.size(); ++i)
    {
        FABRIKChain* chain = chains_[i];
        FABRIKChain* connectedTo = chain->GetConnectedChain();

        if (connectedTo == 0x0)
            chain->SolveIK();
        else
        {
            FABRIKBone* bone = chain->GetConnectedToBone();

            switch (chain->baseConstraint_)
            {
            case FABRIKBaseConstraintType::FBCT_NONE:
            case FABRIKBaseConstraintType::FBCT_GLOBAL_ROTOR:
            case FABRIKBaseConstraintType::FBCT_GLOBAL_HINGE:
                break;
            case FABRIKBaseConstraintType::FBCT_LOCAL_ROTOR:
            case FABRIKBaseConstraintType::FBCT_LOCAL_HINGE:
                {
                    Vec3 baseBoneDir = bone->GetDirection();
                    Mat3x3 connectionBoneMatrix;
                    chain->baseBoneConstraintDir_ = (connectionBoneMatrix * chain->baseBoneConstraintDir_).Normalized();
                
                    if (chain->baseConstraint_ == FBCT_LOCAL_HINGE)
                        chain->baseBoneConstraintReference_ = (connectionBoneMatrix * chain->GetFirstBone()->joint_->axis_).Normalized();
                }
            }
            
            chain->UpdateBasePosition(bone->startLocation_, true);
            chain->SolveIK();
        }
    }
}

void FABRIKRig::ProcessBranch(Skeleton* skeleton, FABRIKChain* chain, FABRIKBone* lastBone, Joint* joint)
{
    auto children = joint->GetChildren();

    if (children.size() > 1)
    {
        // Create a fork
        for (auto child : children)
        {
            FABRIKChain* newChain = new FABRIKChain();
            auto bone = new FABRIKBone(new FABRIKJoint(), joint->GetPosition(), child->GetPosition());
            bone->sourceID_ = skeleton->IndexOf(joint);
            bone->endSourceID_ = skeleton->IndexOf(child);
            newChain->AddBone(bone);

            ProcessBranch(skeleton, chain, bone, child);
            chain->ParentTo(bone);
            AddChain(newChain);
        }
    }
    else if (children.size() == 1)
    {
        // Single segment
        auto bone = new FABRIKBone(new FABRIKJoint(), joint->GetPosition(), children[0]->GetPosition());
        bone->sourceID_ = skeleton->IndexOf(joint);
        bone->endSourceID_ = skeleton->IndexOf(children[0]);
        chain->AddBone(bone);

        ProcessBranch(skeleton, chain, bone, children[0]);
    }
}

}