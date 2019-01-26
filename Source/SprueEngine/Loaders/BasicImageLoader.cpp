#include "BasicImageLoader.h"

#include "../BlockMap.h"
#include "../Resource.h"
#include "../VectorBuffer.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "../Libs/stb_image.h"
#include "../Libs/stb_image_write.h"

#define STB_DXT_IMPLEMENTATION
#include <SprueEngine/Libs/stb_dxt.h>
#include <SprueEngine/Libs/soil/image_DXT.h>

#include <iostream>
#include <fstream>
#include <ios>

namespace SprueEngine
{

void BasicImageLoader::stbi_write_func(void *context, void *data, int size)
{
    VectorBuffer* ctx = (VectorBuffer*)context;
    ctx->Write(data, size);
}

const std::string BasicImageLoader::resourceURI_("Image");
const StringHash BasicImageLoader::typeHash_("Image");

std::string BasicImageLoader::GetResourceURIRoot() const
{
    return resourceURI_;
}

StringHash BasicImageLoader::GetResourceTypeID() const
{
    return typeHash_;
}

std::shared_ptr<Resource> BasicImageLoader::LoadResource(const char* file) const
{
    int width = 0;
    int height = 0;
    int comps = 0;
    if (EndsWith(file, ".hdr"))
    {
        float* data = stbi_loadf(file, &width, &height, &comps, 0);
        if (width && height && comps)
        {
            size_t dataIndex = 0;
            std::shared_ptr<FilterableBlockMap<RGBA>> img = std::make_shared<FilterableBlockMap<RGBA>>(width, height);
            for (unsigned y = 0, idx = 0; y < height; ++y)
            {
                for (unsigned x = 0; x < width; ++x, idx += comps)
                {
                    if (comps == 3)
                        img->set(RGBA(data[dataIndex], data[dataIndex+1], data[dataIndex+2]), x, y);
                    else if (comps == 4)
                        img->set(RGBA(data[dataIndex], data[dataIndex + 1], data[dataIndex + 2], data[dataIndex + 3]), x, y);
                    dataIndex += comps;
                }
            }

            stbi_image_free(data);
            return std::make_shared<BitmapResource>(file, img);
        }
    }
    else if (EndsWith(file, ".dds"))
    {
        //// Load a DDS image
        //int x = 0, y = 0, comps = 0;
        //unsigned char* data = stbi_dds_load((char*)file, &x, &y, &comps, 0);
        //if (data && width && height && comps)
        //{
        //    std::shared_ptr<FilterableBlockMap<RGBA>> img = std::make_shared<FilterableBlockMap<RGBA>>(width, height);
        //    for (unsigned y = 0, idx = 0; y < height; ++y)
        //    {
        //        for (unsigned x = 0; x < width; ++x, idx += comps)
        //            img->set(RGBA::FromData(data, idx, comps), x, y);
        //    }
        //
        //    stbi_image_free(data);
        //    return std::make_shared<BitmapResource>(file, img);
        //}
        //else if (data)
        //    stbi_image_free(data);
    }
    else
    {
        unsigned char* data = stbi_load(file, &width, &height, &comps, 0);
        if (width && height && comps)
        {
            std::shared_ptr<FilterableBlockMap<RGBA>> img= std::make_shared<FilterableBlockMap<RGBA>>(width, height);
            for (unsigned y = 0, idx = 0; y < height; ++y)
            {
                for (unsigned x = 0; x < width; ++x, idx += comps)
                    img->set(RGBA::FromData(data, idx, comps), x, y);
            }

            stbi_image_free(data);
            return std::make_shared<BitmapResource>(file, img);
        }
        else
            stbi_image_free(data);
    }

    return std::shared_ptr<Resource>();
}

bool BasicImageLoader::CanLoad(const char* path) const
{
    std::string text = ToLower(path);
    return EndsWith(text, ".png") || EndsWith(text, ".jpg") || EndsWith(text, ".jpeg") || EndsWith(text, ".tga") || EndsWith(text, ".bmp") || EndsWith(text, ".psd") || EndsWith(text, ".gif") || EndsWith(text, ".hdr");
}

void BasicImageLoader::SavePNG(const FilterableBlockMap<RGBA>* image, const char* fileName)
{
    const bool anyAlpha = AnyAlphaUsed(image);
    VectorBuffer buffer;
    for (unsigned y = 0; y < image->getHeight(); ++y)
        for (unsigned x = 0; x < image->getWidth(); ++x)
        {
            RGBA color = image->get(x, y);
            color.Clip();
            buffer.WriteByte(color.r * 255);
            buffer.WriteByte(color.g * 255);
            buffer.WriteByte(color.b * 255);
            if (anyAlpha)
                buffer.WriteByte(color.a * 255);
        }
    stbi_write_png(fileName, image->getWidth(), image->getHeight(), anyAlpha ? 4 : 3, buffer.GetData(), 0);
}

void BasicImageLoader::SavePNG(const FilterableBlockMap<RGBA>* image, VectorBuffer& buffer)
{
    const bool anyAlpha = AnyAlphaUsed(image);
    VectorBuffer imgBuffer;
    for (unsigned y = 0; y < image->getHeight(); ++y)
        for (unsigned x = 0; x < image->getWidth(); ++x)
        {
            RGBA color = image->get(x, y);
            color.Clip();
            imgBuffer.WriteByte(color.r * 255);
            imgBuffer.WriteByte(color.g * 255);
            imgBuffer.WriteByte(color.b * 255);
            if (anyAlpha)
                imgBuffer.WriteByte(color.a * 255);
        }
    
    stbi_write_png_to_func(&BasicImageLoader::stbi_write_func, &buffer, image->getWidth(), image->getHeight(), anyAlpha ? 4 : 3, imgBuffer.GetData(), 0);
    //int length = 0;
    //void* png = stbi_write_png_to_mem(imgBuffer.GetData(), 4, image->getWidth(), image->getHeight(), imgBuffer.GetSize(), &length);
    //buffer.Write(png, length);
    //free(png);
}

void BasicImageLoader::SaveTGA(const FilterableBlockMap<RGBA>* image, const char* fileName)
{
    const bool anyAlpha = AnyAlphaUsed(image);
    VectorBuffer imgBuffer;
    for (unsigned y = 0; y < image->getHeight(); ++y)
        for (unsigned x = 0; x < image->getWidth(); ++x)
        {
            RGBA color = image->get(x, y);
            color.Clip();
            imgBuffer.WriteByte(color.r * 255);
            imgBuffer.WriteByte(color.g * 255);
            imgBuffer.WriteByte(color.b * 255);
            if (anyAlpha)
                imgBuffer.WriteByte(color.a * 255);
        }
    stbi_write_tga(fileName, image->getWidth(), image->getHeight(), anyAlpha ? 4 : 3, imgBuffer.GetData());
}

void BasicImageLoader::SaveTGA(const FilterableBlockMap<RGBA>* image, VectorBuffer& buffer)
{
    const bool anyAlpha = AnyAlphaUsed(image);
    VectorBuffer imgBuffer;
    for (unsigned y = 0; y < image->getHeight(); ++y)
        for (unsigned x = 0; x < image->getWidth(); ++x)
        {
            RGBA color = image->get(x, y);
            color.Clip();
            imgBuffer.WriteByte(color.r * 255);
            imgBuffer.WriteByte(color.g * 255);
            imgBuffer.WriteByte(color.b * 255);
            if (anyAlpha)
                imgBuffer.WriteByte(color.a * 255);
        }
    stbi_write_tga_to_func(&BasicImageLoader::stbi_write_func, &buffer, image->getWidth(), image->getHeight(), anyAlpha ? 4 : 3, imgBuffer.GetData());
}

void BasicImageLoader::SaveHDR(const FilterableBlockMap<RGBA>* image, const char* fileName)
{
    const bool anyAlpha = AnyAlphaUsed(image);
    VectorBuffer imgBuffer;
    if (!anyAlpha)
    {
        for (unsigned y = 0; y < image->getHeight(); ++y)
            for (unsigned x = 0; x < image->getWidth(); ++x)
            {
                RGBA color = image->get(x, y);
                color.Clip();
                imgBuffer.WriteFloat(color.r);
                imgBuffer.WriteFloat(color.g);
                imgBuffer.WriteFloat(color.b);
            }
    }
    stbi_write_hdr(fileName, image->getWidth(), image->getHeight(), anyAlpha ? 4 : 3, anyAlpha ? (float*)image->getData() : (float*)imgBuffer.GetData());
}

void BasicImageLoader::SaveHDR(const FilterableBlockMap<RGBA>* image, VectorBuffer& buffer)
{
    const bool anyAlpha = AnyAlphaUsed(image);
    VectorBuffer imgBuffer;
    if (!anyAlpha)
    {
        for (unsigned y = 0; y < image->getHeight(); ++y)
            for (unsigned x = 0; x < image->getWidth(); ++x)
            {
                RGBA color = image->get(x, y);
                color.Clip();
                imgBuffer.WriteFloat(color.r);
                imgBuffer.WriteFloat(color.g);
                imgBuffer.WriteFloat(color.b);
            }
    }
    stbi_write_hdr_to_func(&BasicImageLoader::stbi_write_func, &buffer, image->getWidth(), image->getHeight(), anyAlpha ? 4 : 3, anyAlpha ? (float*)image->getData() : (float*)imgBuffer.GetData());
}

void BasicImageLoader::SaveDDS(const FilterableBlockMap<RGBA>* image, const char* fileName)
{
    const bool anyAlpha = AnyAlphaUsed(image);
    VectorBuffer imgBuffer;
    for (unsigned y = 0; y < image->getHeight(); ++y)
        for (unsigned x = 0; x < image->getWidth(); ++x)
        {
            RGBA color = image->get(x, y);
            color.Clip();
            imgBuffer.WriteByte(color.r * 255);
            imgBuffer.WriteByte(color.g * 255);
            imgBuffer.WriteByte(color.b * 255);
            if (anyAlpha)
                imgBuffer.WriteByte(color.a * 255);
        }

    save_image_as_DDS(fileName, image->getWidth(), image->getHeight(), anyAlpha ? 4 : 3, imgBuffer.GetData());
}

//void BasicImageLoader::SaveDDS(const FilterableBlockMap<RGBA>* image, VectorBuffer& buffer)
//{
//    //const bool anyAlpha = AnyAlphaUsed(image);
//    //nv::DDSHeader header;
//    //header.fourcc = anyAlpha ? nv::FOURCC_DXT5 : nv::FOURCC_DXT1;
//    //header.width = image->getWidth();
//    //header.height = image->getHeight();
//    //header.depth = image->getDepth();
//    //if (anyAlpha)
//    //    header.setPixelFormat(32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
//    //else
//    //    header.setPixelFormat(24, 0xFF0000, 0x00FF00, 0x0000FF, 0x0);
//    //
//    //
//    //buffer.WriteFileID("DDS ");
//    //buffer.Write(&header, 128 /*sizeof DDSHeader*/);
//
//
//}

bool BasicImageLoader::AnyAlphaUsed(const FilterableBlockMap<RGBA>* image)
{
    for (unsigned y = 0; y < image->getHeight(); ++y)
        for (unsigned x = 0; x < image->getWidth(); ++x)
            if (image->get(x, y).a < 1.0f)
                return true;
    return false;
}

}