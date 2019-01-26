#include <SprueEngine/Resource.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/GeneralUtility.h>
#include <SprueEngine/Geometry/MeshData.h>
#include <SprueEngine/ResourceLoader.h>
#include <SprueEngine/Geometry/Skeleton.h>

#include <SprueEngine/Libs/UriParser.hpp>

namespace SprueEngine
{
 
const StringHash MeshResource::type_("Mesh");
const StringHash BitmapResource::type_("Image");

bool ResourceHandle::RemapPath(const std::string& relativeTo)
{
    if (relativeTo.empty())
        return true;
    if (IsPathRooted(Name))
        return true;
    Name = MakeAbsolutePath(Name, relativeTo);
    return true;
}

bool ResourceHandle::UnmapPath(const std::string& relativeTo)
{
    if (relativeTo.empty())
        return true;
    if (!IsPathRooted(Name))
        return true;
    //size_t idx = Name.find_first_of(relativeTo);
    //if (idx != std::string::npos)
    //{
    //    Name = Name.substr(relativeTo.length() + 1);
    //    return true;
    //}
    Name = MakeRelativePath(Name, relativeTo);
    return true;
}

int ResourceHandle::Fragment() const
{
    http::url uri = http::ParseHttpUrl(Name);
    return uri.GetFragmentInt();
}

Resource::Resource(std::shared_ptr<Resource> parent) :
    parentResource_(parent)
{
        
}

MeshResource::~MeshResource()
{
    for (auto i : meshes_)
        delete i;
    meshes_.clear();
    if (skeleton_)
        delete skeleton_;
    skeleton_ = 0x0;
}

std::shared_ptr<Resource> MeshResource::Clone() const
{
    std::shared_ptr<MeshResource> res(new MeshResource());
    for (auto i : meshes_)
        res->meshes_.push_back(i->CloneRaw());
    if (skeleton_)
        res->skeleton_ = skeleton_->Clone();
    return res;
}

unsigned MeshResource::GetTriangleCount() const
{
    unsigned ret = 0;
    for (unsigned i = 0; i < meshes_.size(); ++i)
        ret += meshes_[i]->GetIndexBuffer().size() / 3;
    return ret;
}

std::shared_ptr<Resource> BitmapResource::Clone() const
{
    std::shared_ptr<BitmapResource> res(new BitmapResource());
    res->data_.reset(new FilterableBlockMap<RGBA>());
    res->data_->Clone(data_.get());
    return res;
}

}