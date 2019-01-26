#include "ResourceStore.h"

#include "Loaders/BasicImageLoader.h"
#include "Loaders/FBXLoader.h"
#include "Loaders/OBJLoader.h"
#include "Loaders/SVGLoader.h"

#include "Core/Context.h"
#include "FString.h"
#include "Logging.h"
#include "Resource.h"
#include "ResourceLoader.h"
#include "Libs/UriParser.hpp"

namespace SprueEngine
{

ResourceStore::ResourceStore(Context* context)
{
    context->RegisterService(this);
    context->RegisterResourceLoader(new BasicImageLoader());
    context->RegisterResourceLoader(new OBJLoader());
    context->RegisterResourceLoader(new FBXLoader());
    context->RegisterResourceLoader(new SVGLoader());
}

ResourceStore::~ResourceStore()
{
    
}

std::shared_ptr<Resource> ResourceStore::GetOrLoadResource(const char* fileName, const StringHash& resourceType)
{
    // Can we find or is it unloaded?
    auto found = resources_.find(fileName);
    if (found != resources_.end() && !found->second.expired())
    {
        if (std::shared_ptr<Resource> ret = found->second.lock())
            return ret;
    }

    if (ResourceLoader* loader = Context::GetInstance()->GetResourceLoader(resourceType, fileName))
    {
        if (std::shared_ptr<Resource> resource = loader->LoadResource(fileName))
        {
            resources_[fileName] = resource;
            return resource;
        }
    }
    SPRUE_LOG_ERROR(FString("Unable to load resource: %1", fileName).c_str());
    return 0x0;
}

std::shared_ptr<Resource> ResourceStore::LoadFromURI(const char* resourceURI)
{
    http::url uri = http::ParseHttpUrl(std::string(resourceURI));
    StringHash typeID = uri.protocol;
    return GetOrLoadResource(uri.path.c_str(), typeID);
}

void ResourceStore::StoreResource(const char* fileName, std::shared_ptr<Resource> resource)
{
    resources_[fileName] = resource;
}

}