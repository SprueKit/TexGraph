#pragma once

#include <SprueEngine/BlockMap.h>
#include <SprueEngine/ClassDef.h>
#include <SprueEngine/IContextService.h>
#include <SprueEngine/StringHash.h>

#include <string>
#include <vector>
#include <map>

namespace SprueEngine
{

class Context;
class MeshData;
class Skeleton;

/// Means of acquiring a resource
struct SPRUE ResourceHandle
{
    StringHash Type;
    std::string Name;

    ResourceHandle() { }
    ResourceHandle(StringHash type) { Type = type; }
    ResourceHandle(StringHash type, const std::string& name) { Type = type; Name = name; }

    /// Remaps against a specific file path so that it is relative to the given directory. Used at load time. Path is expected to be a valid directory such as C:/MyParentDir/MyDir and the C:/MyParentDir/MyDir/MySubDir/MyFile.file
    bool RemapPath(const std::string& relativeTo);
    /// Unmaps a path so that is stored only in a form relative to the given path. Used at save time. Path is expected to be a valid directory such as C:/MyParentDir/MyDir and the result will be MySubDir/MyFile.file
    bool UnmapPath(const std::string& relativeTo);

    /// Returns a fragment index if present, or -1 if not present.
    int Fragment() const;
};

class SPRUE Resource
{
    NOCOPYDEF(Resource);
public:
    Resource(std::shared_ptr<Resource> res = std::shared_ptr<Resource>());
    virtual ~Resource() { }

    virtual std::string GetName() const { return name_; }

    virtual StringHash GetResourceType() const = 0;

    ResourceHandle GetHandle() { return ResourceHandle { GetResourceType(), GetName() }; }

    virtual std::shared_ptr<Resource> Clone() const = 0;

    std::shared_ptr<Resource> GetParent() const { return parentResource_; }

protected:
    std::string name_;
    /// This resource is a sub-resource.
    std::shared_ptr<Resource> parentResource_;
};

class SPRUE MeshResource : public Resource
{
    BASECLASSDEF(MeshResource, Resource);
    NOCOPYDEF(MeshResource);
public:
    MeshResource() { }
    virtual ~MeshResource();

    virtual StringHash GetResourceType() const override { return type_; }
    std::vector<MeshData*>& GetMeshes() { return meshes_; }
    const std::vector<MeshData*>& GetMeshes() const { return meshes_; }

    unsigned GetTriangleCount() const;

    size_t GetMeshCount() const { return meshes_.size(); }
    MeshData* GetMesh(unsigned i) { return meshes_[i]; }
    const MeshData* GetMesh(unsigned i) const { return meshes_[i]; }
    bool IsEmpty() const { return meshes_.empty(); }

    Skeleton* GetSkeleton() const { return skeleton_; }
    void SetSkeleton(Skeleton* skeleton) { skeleton_ = skeleton; }

    virtual std::shared_ptr<Resource> Clone() const override;

protected:
    std::vector<MeshData*> meshes_;
    Skeleton* skeleton_ = 0x0;
    static const StringHash type_;
};

class SPRUE BitmapResource : public Resource
{
    BASECLASSDEF(BitmapResource, Resource);
    NOCOPYDEF(BitmapResource);

    BitmapResource() { }
public:
    BitmapResource(const std::string& name, std::shared_ptr<FilterableBlockMap<RGBA> >& blockMap) { name_ = name; data_ = blockMap; }

    virtual StringHash GetResourceType() const override { return type_; }

    FilterableBlockMap<RGBA>* GetImage() { return data_.get(); }
    const FilterableBlockMap<RGBA>* GetImage() const { return data_.get(); }

    virtual std::shared_ptr<Resource> Clone() const override;

protected:
    std::shared_ptr< FilterableBlockMap<RGBA> > data_;

    static const StringHash type_;
};

}