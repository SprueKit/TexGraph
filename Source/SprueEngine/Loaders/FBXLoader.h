#pragma once

#include <SprueEngine/ResourceLoader.h>
#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/Math/BoneWeights.h>

#include <vector>

namespace fbxsdk
{
    class FbxMesh;
    class FbxNode;
}

namespace SprueEngine
{

class Joint;
class MeshData;
class Skeleton;
class LegacyAnimation;

class SPRUE FBXLoader : public ResourceLoader
{
public:
    virtual std::string GetResourceURIRoot() const override;
    virtual StringHash GetResourceTypeID() const override;
    virtual std::shared_ptr<Resource> LoadResource(const char*) const override;
    virtual bool CanLoad(const char*) const override;

    static void SaveModel(const std::vector<MeshData*>& meshData, const char* fileName);

private:
    struct Vertex
    {
        Vec3 position_;
        Vec3 normal_;
        Vec2 uv_;
        BoneWeights boneWeights_;

        bool operator==(const Vertex& rhs)
        {
            if (position_ == rhs.position_)
                if (normal_ == rhs.normal_)
                    if (uv_ == rhs.uv_)
                        return true;
            return false;
        }
    };

    Vec3 ReadNormal(int controlPoint, unsigned vertIndex, fbxsdk::FbxMesh* mesh) const;
    Vec2 ReadUV(int controlPoint, unsigned uvIndex, int layer, fbxsdk::FbxMesh* mesh) const;
    
    Skeleton* ReadSkeleton(fbxsdk::FbxNode* rootNode) const;
    
    void BuildSkeletonRecurse(Skeleton* skeleton, fbxsdk::FbxNode* node, Joint* currentJoint) const;

    std::vector<LegacyAnimation*> ReadAnimation() const;

    MeshData* ToMeshData(const char* name, std::vector<Vertex>& vertices, std::vector<unsigned>& indices) const;

    static const std::string resourceURI_;
    static const StringHash typeHash_;
};

}