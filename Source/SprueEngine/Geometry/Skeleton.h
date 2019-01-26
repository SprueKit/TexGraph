#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/MathGeoLib/Geometry/LineSegment.h>
#include <SprueEngine/StringHash.h>

#include <vector>

namespace SprueEngine
{
    /// Specifies general rules for how the joint should be animated
    enum JointMechanics
    {
        JM_None = 0,
        JM_Ball = 1,
        JM_Hinge = 1 << 1,         // like a knee
        JM_Drag = 1 << 2,          // rotates away from velocity
        JM_Anticipates = 1 << 3,   // rotates toward velocity
        JM_Jiggle = 1 << 4         // Uses faux-physics jiggle
    };

    class Skeleton;

    /// Joint's are a specialized type of SceneObject that isn't directly accessible
    class SPRUE Joint
    {
        friend class Skeleton;
        NOCOPYDEF(Joint);
    public:
        Joint();
        virtual ~Joint();

        bool HasParent() const { return parent_ != 0x0; }
        Joint* GetParent() { return parent_; }
        const Joint* GetParent() const { return parent_; }

        /// Returns whether or not this joint has child joints.
        bool HasChildren() const { return children_.size() > 0; }
        /// Returns the list of child joints.
        std::vector<Joint*>& GetChildren() { return children_; }
        /// Returns the list of child joints.
        const std::vector<Joint*>& GetChildren() const { return children_; }
        /// Adds a child to this joint
        void AddChild(Joint* joint);
        /// Removes a child joint if it exists, returns true if removed and false if not found
        bool RemoveChild(Joint* joint, bool recurse = false);

        /// Gets the behavioural rules for this joint.
        JointMechanics GetMechanics() const { return mechanics_; }
        /// Sets the behavioural rules for this joint.
        void SetMechanics(JointMechanics mech) { mechanics_ = mech; }

        /// Sets the parent relative position of the joint.
        Vec3 GetPosition() const { return position_; }
        /// Sets the parent relative rotation of the joint.
        Quat GetRotation() const { return rotation_; }
        /// Sets the parent relative scale of the joint.
        Vec3 GetScale() const { return scale_; }
        /// Gets the combined 3x4 parent relative transform.
        Mat3x4 GetTransform() const;
        
        /// Gets the absolute model space position.
        Vec3 GetModelSpacePosition() const;
        /// Gets the aboslute model space rotation.
        Quat GetModelSpaceRotation() const;
        /// Gets the absolute model space scale.
        Vec3 GetModelSpaceScale() const;
        /// Gets the combined 3x4 model space transform.
        Mat3x4 GetModelSpaceTransform() const;

        void SetPosition(const Vec3& pos) { position_ = pos; }
        void SetRotation(const Quat& rot) { rotation_ = rot; }
        void SetScale(const Vec3& scale) { scale_ = scale; }
        void SetTransform(const Mat3x4& transform);
        void SetModelSpacePosition(const Vec3& pos);
        void SetModelSpaceRotation(const Quat& rot);
        void SetModelSpaceScale(const Vec3& scale);
        void SetModelSpaceTransform(const Mat3x4& transform);

        float GetForward() { return forward_; }
        float GetForward() const { return forward_; }
        void SetForward(float value) { forward_ = value; }

        void SetUserData(void* userData) { userData_ = userData; }
        void* GetUserData() { return userData_; }

        const std::string& GetName() const { return jointName_; }
        void SetName(const std::string& jointName) { jointName_ = jointName; }

        unsigned GetFlags() const { return flags_; }
        unsigned GetCapabilities() const { return capabilities_; }
        unsigned short GetRegion() const { return region_; }

        bool AllowAutoweight() const { return allowAutoWeight_; }
        void SetAutoweight(bool value) { allowAutoWeight_ = value; }

        void ClearVelocity() { velocity_ = Vec3(); }

        unsigned GetIndex() const { return index_; }
        void SetIndex(unsigned idx) { index_ = idx; }

        Skeleton* GetSkeleton() const { return skeleton_; }
        void SetSkeleton(Skeleton* skeleton) { skeleton_ = skeleton; }

        const StringHash& GetSourceID() const { return sourceIdentifier_; }
        void SetSourceID(const StringHash& sourceID) { sourceIdentifier_ = sourceID; }

    private:
        Joint* Clone() const;

        Vec3 position_ = Vec3(0,0,0);
        Quat rotation_ = Quat::identity;
        Vec3 scale_ = Vec3(1, 1, 1);

        /// Name of the joint.
        std::string jointName_;
        /// Indicates the model this came from.
        StringHash sourceIdentifier_;
        /// Skeleton containing the joint.
        Skeleton* skeleton_ = 0x0;
        /// The known parent joint.
        Joint* parent_ = 0x0;
        /// Physical mechanics of the joint.
        JointMechanics mechanics_ = JM_None;
        /// Index into the skeleton for the flat bone list.
        unsigned index_ = -1;
        /// Forward position of the joint, used for knowing elbow point
        float forward_ = 0.0f;
        /// Baked in list of flags.
        unsigned flags_ = 0;
        /// Baked in list of capabilities.
        unsigned capabilities_ = 0;
        /// Cached regional information.
        unsigned short region_ = 0;
        /// Distance the bone has moved.
        Vec3 velocity_ = Vec3(0,0,0);
        /// Bone is allowed to translate in order to take up length to reach ground plane
        bool balanceLength_ = false;
        /// Bone cannot be manipulated
        bool transformLocked_ = false;
        /// Whether automatic weights are allowed.
        bool allowAutoWeight_ = true;
        /// List of child bones
        std::vector<Joint*> children_;
        /// Arbitrary user data
        void* userData_ = 0x0;
    };

    class SPRUE Skeleton
    {
        NOCOPYDEF(Skeleton);
    public:
        Skeleton() { }
        virtual ~Skeleton();

        /// Adds a joint to the skeleton's (invoked by Joint::AddChild) flat list and associate the joint to the skeleton by index.
        unsigned AddJoint(Joint* joint);
        /// Gets the list of joints.
        std::vector<Joint*>& GetAllJoints() { return allJoints_; }
        /// Gets the list of joints.
        const std::vector<Joint*>& GetAllJoints() const { return allJoints_; }

        /// Returns a joint based pointer equality of the user data.
        Joint* GetByUserData(void* userData) const;

        /// Returns the index of a joint by searching it out in the list, this way it is guaranteed to be accurate.
        int IndexOf(Joint* joint) { return std::distance(allJoints_.begin(), std::find(allJoints_.begin(), allJoints_.end(), joint)); }

        /// Returns the root joint.
        Joint* GetRootJoint() { return rootJoint_; }
        /// Returns the root joint.
        const Joint* GetRootJoint() const { return rootJoint_; }

        /// A bone is the association of two joints.
        struct Bone {
            unsigned startIndex_;
            unsigned endIndex_;
            LineSegment segment_;
        };

        /// Returns a list of segment type bones.
        std::vector<Bone> GetBones() const;

        Skeleton* Clone() const;

        void DrawDebug(class IDebugRender* renderer, const Mat3x4& transform) const;

    private:
        void Clone(Joint* rhsJoint, Joint* curJoint) const;

        void _DrawJoint(const Joint* joint, class IDebugRender* renderer, const Mat3x4& transform) const;
        /// Recursive construction of the bones list.
        void _GetBones(std::vector<Bone>& boneList, Joint* currentJoint) const;

        /// Joint at the top of the tree.
        Joint* rootJoint_ = 0x0;
        /// Flat list of all joints.
        std::vector<Joint*> allJoints_;
    };

}