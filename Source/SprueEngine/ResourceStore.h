#pragma once

#include <SprueEngine/IContextService.h>
#include <SprueEngine/StringHash.h>

#include <map>
#include <memory>

namespace SprueEngine
{

class Context;
class Resource;

/*
Path resources as URIs?
    Image://MyFileFolder/MyFilePath.jpg
    Mesh://MyFileFolder/MyFilePath.obj?param=Value&param2=Value2
*/
/// Container for resources
class SPRUE ResourceStore : public IContextService
{
public:
    ResourceStore(Context* context);
    ~ResourceStore();

    /// Request a resource from the cache
    template<class T>
    std::shared_ptr<T> GetResource(const char* fileName)
    {
        auto found = resources_.find(fileName);
        if (found != resources_.end())
        {
            if (std::shared_ptr<T> ret = std::dynamic_pointer_cast<T>(found->second.lock()))
                return ret;
        }
        return 0x0;
    }

    template<class T>
    std::shared_ptr<T> GetOrLoadResource(const char* fileName, const StringHash& resourceType)
    {
        return std::dynamic_pointer_cast<T>(GetOrLoadResource(fileName, resourceType));
    }

    template<class T>
    std::shared_ptr<T> LoadFromURI(const char* uri)
    {
        return std::dynamic_pointer_cast<T>(LoadFromURI(uri));
    }

    std::shared_ptr<Resource> GetOrLoadResource(const char* fileName, const StringHash& resourceType);
    
    /// Load a resource via a URI. Protocol indicates the resource type.
    std::shared_ptr<Resource> LoadFromURI(const char* uri);

    void StoreResource(const char* fileName, std::shared_ptr<Resource> resource);

private:
    std::map<StringHash, std::weak_ptr<Resource> > resources_;
};

}