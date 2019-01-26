#pragma once

#include <SprueEngine/ResourceLoader.h>
#include <SprueEngine/BlockMap.h>
#include <SprueEngine/VectorBuffer.h>

struct NSVGimage;

namespace SprueEngine
{

class SPRUE SVGResource : public Resource
{
    BASECLASSDEF(SVGResource, Resource);
    NOCOPYDEF(SVGResource);
    SVGResource() { }
public:
    SVGResource(const std::string& name, NSVGimage* image) { name_ = name; image_ = image; }
    virtual ~SVGResource();

    virtual StringHash GetResourceType() const override { return type_; }

    std::shared_ptr< FilterableBlockMap<RGBA> > GetImage(unsigned width, unsigned height) const;

    virtual std::shared_ptr<Resource> Clone() const override;

protected:
    NSVGimage* image_;

    static const StringHash type_;
};

class SPRUE SVGLoader : public ResourceLoader
{
    NOCOPYDEF(SVGLoader);
    BASECLASSDEF(SVGLoader, ResourceLoader);
public:
    SVGLoader() { }
    virtual std::string GetResourceURIRoot() const override;
    virtual StringHash GetResourceTypeID() const override;
    virtual std::shared_ptr<Resource> LoadResource(const char*) const override;
    virtual bool CanLoad(const char*) const override;

private:
    static const std::string resourceURI_;
    static const StringHash typeHash_;
};

}