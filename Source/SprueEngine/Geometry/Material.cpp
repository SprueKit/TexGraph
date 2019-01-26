#include "Material.h"

#include <SprueEngine/Core/Context.h>

namespace SprueEngine
{

    static const char* MaterialTextureTypeNames[] = {
        "Diffuse",
        "Roughness",
        "Normal",
        "Specular",
        "Metallic",
        "Height",
        "SubsurfaceDepth",
        "SubsurfaceColor",
        "Mask",
        0x0
    };

    Material::Material()
    {
    }

    Material::~Material()
    {

    }

    void Material::Register(Context* context)
    {
        context->RegisterFactory<Material>("Material", "Describes the behavior of a collection of materials");
        COPY_PROPERTIES(IEditable, Material);
#define REG_TEXTURE(NAME, DESCR) REGISTER_RESOURCE(Material, BitmapResource, Get ## NAME ## Handle, Set ## NAME ## Handle, Get ## NAME ## Image, Set ## NAME ## Image, ResourceHandle("Image"), #NAME, DESCR, PS_Default);

        REG_TEXTURE(Diffuse, "Albedo texture for PBR");
        REG_TEXTURE(Roughness, "");
        REG_TEXTURE(Metallic, "");
        REG_TEXTURE(Normal, "");
        REG_TEXTURE(Height, "");
        REG_TEXTURE(SubsurfaceColor, "");
        REG_TEXTURE(SubsurfaceDepth, "");
        REG_TEXTURE(Mask, "");
        //REGISTER_RESOURCE(Material, BitmapResource, GetDiffuseHandle, SetDiffuseHandle, GetDiffuseImage, SetDiffuseImage, ResourceHandle("Image"), "Diffuse", "", PS_Default);
    }


    bool Material::HasTexture(MaterialTextureType type) const
    {
        switch (type)
        {
        case MTT_Diffuse:
            return !Diffuse_.Name.empty();
        case MTT_Roughness:
            return !Roughness_.Name.empty();
        case MTT_Metallic:
            return !Metallic_.Name.empty();
        case MTT_Normal:
            return !Normal_.Name.empty();
        case MTT_Height:
            return !Height_.Name.empty();
        case MTT_SubsurfaceColor:
            return !SubsurfaceColor_.Name.empty();
        case MTT_SubsurfaceDepth:
            return !SubsurfaceDepth_.Name.empty();
        case MTT_Mask:
            return !Mask_.Name.empty();
        }
        return false;
    }

    ResourceHandle Material::GetHandle(MaterialTextureType type) const
    {
        switch (type)
        {
        case MTT_Diffuse:
            return Diffuse_;
        case MTT_Roughness:
            return Roughness_;
        case MTT_Metallic:
            return Metallic_;
        case MTT_Normal:
            return Normal_;
        case MTT_Height:
            return Height_;
        case MTT_SubsurfaceColor:
            return SubsurfaceColor_;
        case MTT_SubsurfaceDepth:
            return SubsurfaceDepth_;
        case MTT_Mask:
            return Mask_;
        }
        return ResourceHandle("Image");
    }
    
    std::shared_ptr<BitmapResource> Material::GetBitmap(MaterialTextureType type) const
    {
        switch (type)
        {
        case MTT_Diffuse:
            return DiffuseImage_;
        case MTT_Roughness:
            return RoughnessImage_;
        case MTT_Metallic:
            return MetallicImage_;
        case MTT_Normal:
            return NormalImage_;
        case MTT_Height:
            return HeightImage_;
        case MTT_SubsurfaceColor:
            return SubsurfaceColorImage_;
        case MTT_SubsurfaceDepth:
            return SubsurfaceDepthImage_;
        case MTT_Mask:
            return MaskImage_;
        }
        return std::shared_ptr<BitmapResource>();
    }

    void Material::SetTexture(MaterialTextureType type, ResourceHandle handle, std::shared_ptr<BitmapResource> bitmap)
    {
#define SET(NAME) NAME ## _ = handle; NAME ## Image_ = bitmap
        if (type == MTT_Diffuse)
        {
            SET(Diffuse);
        }
        else if (type == MTT_Roughness)
        {
            SET(Roughness);
        }
        else if (type == MTT_Metallic)
        {
            SET(Metallic);
        }
        else if (type == MTT_Normal)
        {
            SET(Normal);
        }
        else if (type == MTT_Height)
        {
            SET(Height);
        }
        else if (type == MTT_SubsurfaceColor)
        {
            SET(SubsurfaceColor);
        }
        else if (type == MTT_SubsurfaceDepth)
        {
            SET(SubsurfaceDepth);
        }
        else if (type == MTT_Mask)
        {
            SET(Mask);
        }
    }
    
}