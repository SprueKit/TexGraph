#include "SVGLoader.h"

#define NANOSVG_IMPLEMENTATION
#include <SprueEngine/Libs/nanosvg/nanosvg.h>
#define NANOSVGRAST_IMPLEMENTATION
#include <SprueEngine/Libs/nanosvg/nanosvgrast.h>

#include <memory>

namespace SprueEngine
{

const StringHash SVGResource::type_("SVG");
const std::string SVGLoader::resourceURI_("SVG");
const StringHash SVGLoader::typeHash_("SVG");

SVGResource::~SVGResource()
{
    if (image_)
        nsvgDelete(image_);
    image_ = 0x0;
}

std::shared_ptr< FilterableBlockMap<RGBA> > SVGResource::GetImage(unsigned width, unsigned height) const
{
    if (!image_)
        return 0x0;

    if (NSVGrasterizer* rasterizer = nsvgCreateRasterizer())
    {
        std::shared_ptr< FilterableBlockMap<RGBA> > ret = std::make_shared<FilterableBlockMap<RGBA> >(width, height);
        unsigned char* img = (unsigned char*)malloc(width*height* 4);
        nsvgRasterize(rasterizer, image_,
            0.0f, 0.0f, 1.0f,
            img, width, height, width * 4);

        for (unsigned y = 0, idx = 0; y < height; ++y)
        {
            for (unsigned x = 0; x < width; ++x, idx += 4)
                ret->set(RGBA::FromData(img, idx, 4), x, y);
        }

        free(img);
        nsvgDeleteRasterizer(rasterizer);
        return ret;
    }
    return std::shared_ptr< FilterableBlockMap<RGBA> >();
}

std::shared_ptr<Resource> SVGResource::Clone() const
{
    std::shared_ptr<SVGResource> ret(new SVGResource());
    // pointer copy
    ret->image_ = image_;
    return ret;
}

std::string SVGLoader::GetResourceURIRoot() const
{
    return resourceURI_;
}

StringHash SVGLoader::GetResourceTypeID() const
{
    return typeHash_;
}

std::shared_ptr<Resource> SVGLoader::LoadResource(const char* fileName) const
{
    if (NSVGimage* image = nsvgParseFromFile(fileName, "px", 96))
        return std::make_shared<SVGResource>(fileName, image);
    return std::shared_ptr<Resource>();
}

bool SVGLoader::CanLoad(const char* path) const
{
    std::string text = ToLower(path);
    return EndsWith(text, ".svg");
}

}