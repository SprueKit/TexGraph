#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/StringHash.h>

#include <memory>

namespace SprueEngine
{

class Resource;

/// Base type for implementations of resource loaders
class SPRUE ResourceLoader
{
public:
    /// Implementation should return the root/protocol portion for a URI (ie. MeshData:// or Texture2D:// [without the ://])
    virtual std::string GetResourceURIRoot() const = 0;
    
    /// Hash identifier for this resource Return value should effectively be static equal to the hash of the URI root (ie. just hash the URI root and store it statically)
    virtual StringHash GetResourceTypeID() const = 0;
    
    /// Perform the work of loading the resource.
    virtual std::shared_ptr<Resource> LoadResource(const char*) const = 0;

    virtual bool CanLoad(const char*) const = 0;
};

}