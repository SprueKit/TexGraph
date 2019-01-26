#pragma once

#include <SprueEngine/ResourceLoader.h>
#include <SprueEngine/BlockMap.h>
#include <SprueEngine/VectorBuffer.h>

namespace SprueEngine
{

class SPRUE BasicImageLoader : public ResourceLoader
{
    NOCOPYDEF(BasicImageLoader);
    BASECLASSDEF(BasicImageLoader, ResourceLoader);
public:
    BasicImageLoader() { }
    virtual std::string GetResourceURIRoot() const override;
    virtual StringHash GetResourceTypeID() const override;
    virtual std::shared_ptr<Resource> LoadResource(const char*) const override;
    virtual bool CanLoad(const char*) const override;

    static void SavePNG(const FilterableBlockMap<RGBA>* image, const char* fileName);
    static void SavePNG(const FilterableBlockMap<RGBA>* image, VectorBuffer& buffer);
    static void SaveTGA(const FilterableBlockMap<RGBA>* image, const char* fileName);
    static void SaveTGA(const FilterableBlockMap<RGBA>* image, VectorBuffer& buffer);
    static void SaveHDR(const FilterableBlockMap<RGBA>* image, const char* fileName);
    static void SaveHDR(const FilterableBlockMap<RGBA>* image, VectorBuffer& buffer);
    static void SaveDDS(const FilterableBlockMap<RGBA>* image, const char* fileName);
    static bool AnyAlphaUsed(const FilterableBlockMap<RGBA>* image);

private:
    static void stbi_write_func(void *context, void *data, int size);

    static const std::string resourceURI_;
    static const StringHash typeHash_;
};

}