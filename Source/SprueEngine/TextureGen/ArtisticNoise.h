#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>

#include <SprueEngine/Libs/FastNoise.h>

namespace SprueEngine
{

    enum ArtisticNoiseBlend
    {
        ANB_Smart,
        ANB_AlternatingAddSub
    };

    class SPRUE ArtisticNoiseRecord : public IEditable
    {
        BASECLASSDEF(ArtisticNoiseRecord, IEditable);
        SPRUE_EDITABLE(ArtisticNoiseRecord);
    public:
        static void Register(Context*);

        ResourceHandle GetBitmapResourceHandle() const { return bitmapResourceHandle; }
        void SetBitmapResourceHandle(const ResourceHandle& handle) { bitmapResourceHandle = handle; }
        
        std::shared_ptr<BitmapResource> GetImageData() const { return ImageData; }
        void SetImageData(const std::shared_ptr<BitmapResource>& img) { ImageData = img; }

        std::shared_ptr<BitmapResource> ImageData;
        ResourceHandle bitmapResourceHandle;
        unsigned Splats = 1;
        float BlendPower = 1.0f;
        RangedFloat Offset = RangedFloat(0, 0);
        RangedFloat RotationRange = RangedFloat(0, 0);
        RangedFloat ScaleRange = RangedFloat(1,1);
    };

    class SPRUE ArtisticNoise : public PreviewableNode
    {
        BASECLASSDEF(ArtisticNoise, PreviewableNode);
    public:
        IMPL_TEXTURE_NODE(ArtisticNoise);

        ArtisticNoiseBlend Blending = ANB_Smart;
        std::vector<ArtisticNoiseRecord*> Records;

        // For property access
        Variant GetRecords() const;
        void SetRecords(Variant data);

    private:
        RGBA Sample(ArtisticNoiseRecord* record, const Variant& param, unsigned idx) const;
        mutable FastNoise noise_;
    };

}