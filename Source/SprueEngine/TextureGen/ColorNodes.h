#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEngine
{
    
    class SPRUE ReplaceColorModifier : public PreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(ReplaceColorModifier);

        RGBA Replace;
        RGBA With;
        float Tolerance = 0.1f;
    };

    class SPRUE SelectColorModifier : public PreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(SelectColorModifier);

        RGBA Select = RGBA::Black;
        float Tolerance = 0.1f;
        bool Boolean = false;
    };

    class SPRUE IDMapGenerator : public PreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(IDMapGenerator);

        std::shared_ptr<BitmapResource> ImageData;
        ResourceHandle bitmapResourceHandle;

        ResourceHandle GetBitmapResourceHandle() const { return bitmapResourceHandle; }
        void SetBitmapResourceHandle(const ResourceHandle& handle) { bitmapResourceHandle = handle; }
        std::shared_ptr<BitmapResource> GetImageData() const { return ImageData; }
        void SetImageData(const std::shared_ptr<BitmapResource>& img);

        virtual bool Deserialize(Deserializer* src, const SerializationContext& context) override;

        // Temporary cache
        std::vector<RGBA> colors;
        bool inSerialization = false;

        virtual std::shared_ptr<FilterableBlockMap<RGBA>> GetPreview(unsigned width = TEXGRAPH_PREVIEW_SIZE, unsigned height = TEXGRAPH_PREVIEW_SIZE) override;
    };
}