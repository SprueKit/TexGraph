#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEngine
{

class SPRUE InvertTextureModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(InvertTextureModifier);
};

class SPRUE SolarizeTextureModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SolarizeTextureModifier);
    float Threshold = 0.5f;
    bool InvertLower = true;
};

//TODO: in GUI add support for "prefab" nodes to include sharpen, box-blur, etc, convolution filter templates
class SPRUE ConvolutionFilter : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ConvolutionFilter);
    Mat3x3 Kernel = Mat3x3(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f); // Although this is a matrix it will be applied as a 3x3 convolution filter
    float StepSize = 1.0f;

    virtual bool WillForceExecute() const override { return true; }
};

class SPRUE SharpenFilter : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SharpenFilter);
    float Power = 1.0f;
    float StepSize = 1.0f;

    virtual bool WillForceExecute() const override { return true; }
};

class SPRUE GradientRampTextureModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(GradientRampTextureModifier);
    ColorRamp Gradient;
};

class SPRUE CurveTextureModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(CurveTextureModifier);
    ColorCurves Curves;
};

class SPRUE SobelTextureModifier : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SobelTextureModifier);
    float StepSize = 1.0f;
    virtual bool WillForceExecute() const override { return true; }
};

class SPRUE ClipTextureModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ClipTextureModifier);
    RangedFloat Range = RangedFloat(0.0f, 1.0f);
};

class SPRUE TileModifier : public PreviewableNode
{
public:
    TileModifier() : Tiling(1.0f, 1.0f) { }

    IMPL_TEXTURE_NODE(TileModifier);
        
    Vec2 Tiling;
};

class SPRUE WarpModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(WarpModifier);
    float Intensity = 0.01f;
};

class SPRUE TransformModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(TransformModifier);
    Mat3x3 Matrix;

    virtual Variant FilterParameter(const Variant& parameter) const override;
};

class SPRUE SimpleTransformModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SimpleTransformModifier);

    virtual Variant FilterParameter(const Variant& parameter) const override;

    Vec2 Offset;
    Vec2 Scale = Vec2(1, 1);
    float Rotation = 0.0f;
};

class SPRUE CartesianToPolarModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(CartesianToPolarModifier);
    virtual Variant FilterParameter(const Variant& parameter) const override;

    float velocity_ = 0.0f;
    float spacing_ = 1.0f;
};

class SPRUE PolarToCartesianModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(PolarToCartesianModifier);
    virtual Variant FilterParameter(const Variant& parameter) const override;
};

/// Divides space into two seperate partitions
class SPRUE DivModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(DivModifier);

    float Fraction = 0.5f;
    bool Vertical = false;
    bool NormalizeCoordinates = false;

    virtual bool WillForceExecute() const override { return true; }
};

class SPRUE TrimModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(TrimModifier);
    float TrimSize = 0.2f;
    bool Vertical = false;
    bool NormalizeCoordinates = false;

    virtual bool WillForceExecute() const override { return true; }
};

class SPRUE EmbossModifier : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(EmbossModifier);
    float Angle = 0.0f;
    float StepSize = 1.0f;
    float Bias = 0.5f;
    float Power = 1.0f;
    virtual bool WillForceExecute() const override { return true; }
};

class SPRUE PosterizeModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(PosterizeModifier);
    unsigned Range = 8;

    float Posterize(float in);
};

class SPRUE ErosionModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ErosionModifier);

    unsigned Iterations = 3;
    float Intensity = 1.0f;
    float StepSize = 1.0f;
    float Talus = 0.02f;

    virtual bool WillForceExecute() const override { return true; }
};

class SPRUE SampleSizeModifier : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SampleSizeModifier);

    virtual Variant FilterParameter(const Variant& param) const override;
    virtual bool WillForceExecute() const override { return true; }

    Vec2 newSize = Vec2(128, 128);
    bool Bilinear = true;
    std::shared_ptr<FilterableBlockMap<RGBA> > cache;
};

}