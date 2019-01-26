#pragma once

#include <SprueEngine/Libs/FastNoise.h>
#include <SprueEngine/Resource.h>
#include <SprueEngine/Loaders/SVGLoader.h>
#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEngine
{

    enum Interpolation
    {
        SIP_None,
        SIP_Linear,
        SIP_Hermite,
        SIP_Quintic
    };

class SPRUE RowsGenerator : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(RowsGenerator);
    unsigned RowCount = 6;
    bool Vertical = true;
    float PerturbPower = 1.0f;
    bool AlternateDeadColumns = false;
};

class SPRUE CheckerGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(CheckerGenerator);
    RGBA ColorA;
    RGBA ColorB;
    IntVec2 TileCount;
};

class SPRUE BrickGenerator : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(BrickGenerator);
    virtual void VersionUpdate(unsigned) override;

    Vec2 TileSize = Vec2(0.5f, 0.5f);
    float RowOffset = 0.5f;
    Vec2 Gutter = Vec2(0.2f, 0.2f);
    Vec2 PerturbPower = Vec2(1, 1);
    RGBA BlockColor = RGBA::White;
    RGBA GroutColor = RGBA::Black;
};

class SPRUE NoiseGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(NoiseGenerator);
    bool Inverted;
    Vec3 Period = Vec3(8, 8, 8);
private:
    mutable FastNoise noise_;
};

class SPRUE FBMGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(FBMGenerator);
    bool Inverted = false;
    Vec3 Period = Vec3(8, 8, 8);
private:
    FastNoise noise_;
};

class SPRUE PerlinNoiseGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(PerlinNoiseGenerator);
    bool Inverted;
    Vec3 Period = Vec3(8, 8, 8);
private:
    FastNoise noise_;
};

class SPRUE VoronoiGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(VoronoiGenerator);
    bool Inverted;
    Vec3 Period = Vec3(8, 8, 8);
private:
    FastNoise noise_;
};

class SPRUE UberNoiseGenerator : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(UberNoiseGenerator);
    Vec3 Period = Vec3(8, 8, 8);
    float Lacunarity = 2.0f;
    float Gain = 0.5f;
    float Amplitude = 0.2f;
    float PerturbFeatures = 0.2f;
    float Sharpness = 0.7f;
    float AmplifyFeatures = 0.7f;
    float AltitudeErosion = 0.7f;
    float RidgeErosion = 0.7f;
    float SlopeErosion = 0.7f;
    unsigned Octaves = 3;
private:
    FastNoise noise_;
};

/// Randomly splats the given texture within in the range of 0..1
class SPRUE TextureBombGenerator : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(TextureBombGenerator);

    std::shared_ptr<BitmapResource> ImageData;
    ResourceHandle bitmapResourceHandle;

    RangedFloat AngularRange = RangedFloat(0,0);
    RangedFloat ScaleRange = RangedFloat(1,1);
    unsigned DesiredDensity = 4;

    ResourceHandle GetBitmapResourceHandle() const { return bitmapResourceHandle; }
    void SetBitmapResourceHandle(const ResourceHandle& handle) { bitmapResourceHandle = handle; }
    std::shared_ptr<BitmapResource> GetImageData() const { return ImageData; }
    void SetImageData(const std::shared_ptr<BitmapResource>& img) { ImageData = img; }

private:
    FastNoise noise_;
};

/// Samples a bitmap
class SPRUE BitmapGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(BitmapGenerator);

    std::shared_ptr<BitmapResource> ImageData;
    ResourceHandle bitmapResourceHandle;
    bool WrapX = true;
    bool WrapY = true;

    ResourceHandle GetBitmapResourceHandle() const { return bitmapResourceHandle; }
    void SetBitmapResourceHandle(const ResourceHandle& handle) { bitmapResourceHandle = handle; }
    std::shared_ptr<BitmapResource> GetImageData() const { return ImageData; }
    void SetImageData(const std::shared_ptr<BitmapResource>& img) { ImageData = img; }
};

class SPRUE SVGGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SVGGenerator);

    std::shared_ptr<SVGResource> svgResource;
    ResourceHandle svgResourceHandle;

    ResourceHandle GetSVGResourceHandle() const { return svgResourceHandle; }
    void SetSVGResourceHandle(const ResourceHandle& handle) { svgResourceHandle = handle; }
    
    std::shared_ptr<SVGResource> GetSVGData() const { return svgResource; }
    void SetSVGData(const std::shared_ptr<SVGResource>& img) { svgResource = img; }

    IntVec2 Size = IntVec2(128,128);

private:
    std::shared_ptr<FilterableBlockMap<RGBA> > rasterData;
};


enum TexGenGradient
{
    TGG_Linear,
    TGG_Reflected,
    TGG_Radial,
    TGG_Angle,
};

class SPRUE GradientGenerator : public SelfPreviewableNode
{
public:
    GradientGenerator() : type_(TGG_Linear) { }
    IMPL_TEXTURE_NODE(GradientGenerator);

    TexGenGradient GetGradientType() const { return type_; }
    void SetGradientType(TexGenGradient value) { type_ = value; }

    Vec2 Offset = Vec2(0,0);
    float Length = 1.0f;
    float Angle = 0.0f;
    RGBA Start = RGBA(1, 1, 1);
    RGBA End = RGBA(0, 0, 0);
    TexGenGradient type_ = TGG_Linear;
private:
};

class SPRUE GaborNoiseGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(GaborNoiseGenerator);

    float K = 1.0f;
    float Alpha = 0.05f;
    float F0 = 0.0625f;
    float Omega = PI / 4.0f;
    float Impulses = 64.0f;
    float Period = 256.0f;
    float Offset = 0;

private:
    float CellValue(int i, int j, float x, float y, float impulseDensity, float kernelRadius);
    unsigned Morton(unsigned x, unsigned y);
    float Gabor(float K, float a, float F_0, float omega_0, float x, float y);
    FastNoise noise_;
};

}