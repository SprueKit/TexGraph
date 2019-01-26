#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/IEditable.h>
#include <SprueEngine/Resource.h>

#include <vector>

namespace SprueEngine
{

    enum MaterialTextureType
    {
        MTT_Diffuse,
        MTT_Roughness,
        MTT_Normal,
        MTT_Specular,
        MTT_Metallic,
        MTT_Height,
        MTT_SubsurfaceDepth,
        MTT_SubsurfaceColor,
        MTT_Mask,
        MTT_COUNT,
    };

    /// An aggregation of textures each with an intended purpose. Used for texture generation stages.
    /// This is a non-trivial object and fundamentally a "special" case for editing purposes. It is
    /// not just an indexed set, as the indices have names.
    class SPRUE Material : public IEditable
    {
        SPRUE_EDITABLE(Material);
        BASECLASSDEF(Material, IEditable);
    public:
        Material();
        virtual ~Material();

        static void Register(Context* context);


        bool HasTexture(MaterialTextureType) const;
        ResourceHandle GetHandle(MaterialTextureType) const;
        std::shared_ptr<BitmapResource> GetBitmap(MaterialTextureType) const;

        void SetTexture(MaterialTextureType type, ResourceHandle handle, std::shared_ptr<BitmapResource> bitmap);


#define MATERIAL_DECLARATION(NAME) public: ResourceHandle Get ## NAME ## Handle() const { return NAME ## _; } \
    void Set ## NAME ## Handle(const ResourceHandle& handle) { NAME ## _ = handle; } \
    private: ResourceHandle NAME ## _; \
    public: std::shared_ptr<BitmapResource> Get ## NAME ## Image() const { return NAME ## Image_; } \
    void Set ## NAME ## Image(const std::shared_ptr<BitmapResource>& img) { NAME ## Image_ = img; } \
    private: std::shared_ptr<BitmapResource> NAME ## Image_;

        MATERIAL_DECLARATION(Diffuse);
        MATERIAL_DECLARATION(Roughness);
        MATERIAL_DECLARATION(Metallic);
        MATERIAL_DECLARATION(Normal);
        MATERIAL_DECLARATION(Height);
        MATERIAL_DECLARATION(SubsurfaceColor);
        MATERIAL_DECLARATION(SubsurfaceDepth);
        MATERIAL_DECLARATION(Mask);
    };

}