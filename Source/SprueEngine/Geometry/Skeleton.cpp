#include "SprueEngine/Geometry/Skeleton.h"

#include <SprueEngine/IDebugRender.h>

#include <set>

namespace SprueEngine
{

    Joint::Joint() :
        parent_(0x0),
        userData_(0x0),
        skeleton_(0x0)
    {

    }

    Joint::~Joint()
    {
        for (auto child : children_)
            delete child;
        children_.clear();
    }

    void Joint::AddChild(Joint* joint)
    {
        joint->parent_ = this;
        skeleton_->AddJoint(joint);
        children_.push_back(joint);
    }

    bool Joint::RemoveChild(Joint* joint, bool recurse)
    {
        auto found = std::find(children_.begin(), children_.end(), joint);
        if (found != children_.end())
        {
            children_.erase(found);
            return true;
        }
        if (recurse)
        {
            for (auto child : children_)
                child->RemoveChild(joint, recurse);
        }
        return false;
    }

    Vec3 Joint::GetModelSpacePosition() const
    {
        return GetModelSpaceTransform().TranslatePart();
    }

    Quat Joint::GetModelSpaceRotation() const
    {
        if (parent_)
            return parent_->GetModelSpaceRotation() * rotation_;
        return rotation_;
    }

    Vec3 Joint::GetModelSpaceScale() const
    {
        if (parent_)
            return parent_->GetModelSpaceScale() * scale_;
        else
            return scale_;
    }

    Mat3x4 Joint::GetTransform() const
    {
        return Mat3x4::FromTRS(position_, rotation_, scale_);
    }

    Mat3x4 Joint::GetModelSpaceTransform() const
    {
        if (parent_)
            return parent_->GetModelSpaceTransform() * GetTransform();
        return GetTransform();
    }

    void Joint::SetTransform(const Mat3x4& transform)
    {
        transform.Decompose(position_, rotation_, scale_);
    }

    void Joint::SetModelSpacePosition(const Vec3& pos)
    {
        if (parent_)
            position_ = (parent_->GetModelSpaceTransform().Inverted() * pos).xyz();
        else
            SetPosition(pos);
    }

    void Joint::SetModelSpaceRotation(const Quat& rot)
    {
        if (parent_)
            rotation_ = parent_->GetModelSpaceRotation().Inverted() * rot;
        else
            SetRotation(rot);
    }

    void Joint::SetModelSpaceScale(const Vec3& scale)
    {
        if (parent_)
            scale_ = (1.0f / parent_->GetModelSpaceScale()) * scale;
        else
            SetScale(scale);
    }
    
    void Joint::SetModelSpaceTransform(const Mat3x4& transform)
    {
        if (parent_)
            SetTransform(parent_->GetModelSpaceTransform().Inverted() * transform);
        else
            SetTransform(transform);
    }

    Joint* Joint::Clone() const
    {
        Joint* ret = new Joint();

        ret->position_ = position_;
        ret->rotation_ = rotation_;
        ret->scale_ = scale_;

        ret->jointName_ = jointName_;
        ret->sourceIdentifier_ = sourceIdentifier_;
        ret->mechanics_ = mechanics_;
        ret->index_ = index_;
        ret->forward_ = forward_;
        ret->flags_ = flags_;
        ret->capabilities_ = capabilities_;
        ret->region_ = region_;
        ret->balanceLength_ = balanceLength_;
        ret->transformLocked_ = transformLocked_;
        ret->allowAutoWeight_ = allowAutoWeight_;
        return ret;
    }

    Skeleton::~Skeleton()
    {
        if (rootJoint_)
            delete rootJoint_;
        rootJoint_ = 0x0;
        //for (auto joint : allJoints_)
        //    delete joint;
        //allJoints_.clear();
    }

    unsigned Skeleton::AddJoint(Joint* joint)
    {
        const unsigned index = allJoints_.size();
        allJoints_.push_back(joint);
        joint->SetIndex(index);
        joint->SetSkeleton(this);
        if (rootJoint_ == 0x0)
            rootJoint_ = joint;
        return index;
    }

    Joint* Skeleton::GetByUserData(void* userData) const
    {
        for (auto joint : allJoints_)
            if (joint->GetUserData() == userData)
                return joint;
        return 0x0;
    }

    std::vector<Skeleton::Bone> Skeleton::GetBones() const
    {
        std::vector<Skeleton::Bone> ret;
        if (rootJoint_)
            _GetBones(ret, rootJoint_);
        return ret;
    }

    void Skeleton::DrawDebug(IDebugRender* renderer, const Mat3x4& transform) const
    {
        static const RGBA colors[] = {
            RGBA::Red,
            RGBA::Green,
            RGBA::Blue,
            RGBA::Green
        };
        auto bones = GetBones();
        for (unsigned i = 0; i < bones.size(); ++i)
            renderer->DrawLine(bones[i].segment_.a, bones[i].segment_.b, colors[i]);
        //if (GetRootJoint())
        //    _DrawJoint(GetRootJoint(), renderer, transform);
    }

    void Skeleton::_DrawJoint(const Joint* joint, IDebugRender* renderer, const Mat3x4& transform) const
    {
        for (auto child : joint->GetChildren())
        {
            renderer->DrawLine(transform * joint->GetModelSpacePosition(), transform * child->GetModelSpacePosition(), RGBA::White);
            _DrawJoint(child, renderer, transform);
        }
    }

    void Skeleton::_GetBones(std::vector<Bone>& boneList, Joint* currentJoint) const
    {
        if (currentJoint)
        {
            for (auto child : currentJoint->GetChildren())
            {
                if (child)
                {
                    if (currentJoint->AllowAutoweight())
                        boneList.push_back({ currentJoint->GetIndex(), child->GetIndex(), LineSegment(currentJoint->GetModelSpacePosition(), child->GetModelSpacePosition()) });
                    _GetBones(boneList, child);
                }
            }
        }
    }

    Skeleton* Skeleton::Clone() const
    {
        Skeleton* ret = new Skeleton();
        ret->AddJoint(rootJoint_->Clone());
        Clone(ret->rootJoint_, rootJoint_);
        return ret;
    }

    void Skeleton::Clone(Joint* rhsCur, Joint* curJoint) const
    {
        for (unsigned i = 0; i < curJoint->children_.size(); ++i)
        {
            auto j = curJoint->children_[i]->Clone();
            rhsCur->AddChild(j);
            Clone(j, curJoint->children_[i]);
        }
    }
}