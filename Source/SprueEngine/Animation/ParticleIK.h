#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{
#define SPRUE_PARTICLE_IK_LOWER_MULT 0.9f
#define SPRUE_PARTICLE_IK_UPPER_MULT 1.2f

    struct ParticleIK;

    struct SPRUE ParticleIKConstraint
    {
        float weight_ = 1.0f;
        virtual bool CheckConstraint(const Vec3& position, Vec3& vector) = 0;
    };

    struct SPRUE OtherParticleIKConstraint : public ParticleIKConstraint
    {
        ParticleIK* particle_ = 0x0;
        float distance_ = 0.0f;

        /// verify that our distance
        virtual bool CheckConstraint(const Vec3& position, Vec3& vector) override;
    };

    struct SPRUE ParticleIK
    {
        ParticleIK* parent_ = 0x0;
        Vec3 position_;
        /// At construction time this is the recorded length.
        float lengthFromParent_ = 0.0f;
        /// If set to true then our position is assumed to be an authoritative position.
        bool isEffector_ = false;
        /// 
        float mass_ = 1.0f;
        std::vector<ParticleIKConstraint*> constraints_;

        void CalculateLength()
        {
            if (parent_)
                lengthFromParent_ = (position_ - parent_->position_).Length();
            lengthFromParent_ = 0;
        }

        ParticleIK* Iterate(float maxDeltaLength)
        {
            Relax(maxDeltaLength);
            return parent_;
        }

        /// Perform a full effector to root iteration
        static void Iterate(ParticleIK* end, float maxDeltaLength)
        {
            do {
                end = end->Iterate(maxDeltaLength);
            } while (end);
        }

        bool IsValid()
        {
            if (parent_)
            {
                float len = (parent_->position_ - position_).Length();
                return len > lengthFromParent_ * SPRUE_PARTICLE_IK_LOWER_MULT && len < lengthFromParent_ * SPRUE_PARTICLE_IK_UPPER_MULT;
            }
            return true;
        }

        void Relax(float maxDeltaLength)
        {
            bool isFixed = false;
            if (parent_ == 0x0)
                isFixed = true;
            else if (isEffector_)
                isFixed = true;
            
            bool isParentFixed = false;
            if (parent_ && (parent_->isEffector_ || parent_->parent_ == 0x0))
                isParentFixed = true;

            if (isFixed && isParentFixed)
                return;

            Vec3 parentPos = parent_->position_;
            Vec3 pos = position_;
            Vec3 delta = parentPos - pos;
            float thisLength = delta.Length();
            float deltaLength = (thisLength - lengthFromParent_);
            if (deltaLength > maxDeltaLength)
                deltaLength = maxDeltaLength;

            float scaleFactor = 1.0f;
            if (!SprueEquals(lengthFromParent_, 0.0f) && !SprueEquals(thisLength, 0.0f))
                scaleFactor = deltaLength / thisLength;

            delta *= scaleFactor;

            if (isFixed)
                parent_->position_ = parent_->position_ - delta;
            else if (isParentFixed)
                position_ = position_ - delta;
            else
            {
                parent_->position_ = parent_->position_ - delta * 0.5f;
                position_ = position_ + delta * 0.5f;
            }

            // Check all of our constraints and constrain our position
            if (!isFixed)
            {
                Vec3 adjustVec;
                for (auto constraint : constraints_)
                {
                    if (!constraint->CheckConstraint(position_, adjustVec))
                        position_ = (position_ - adjustVec) * mass_;
                }
            }
        }
    };

    /// Preconditioner's run to filter
    struct SPRUE IKPreconditioner
    {

    };
}