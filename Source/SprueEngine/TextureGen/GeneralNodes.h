#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEngine
{

/// Specifies a constant color
class SPRUE ColorNode : public TextureNode
{
public:
    IMPL_TEXTURE_NODE(ColorNode);
    RGBA Value = RGBA(1,0,0);

    virtual bool CanPreview() const { return true; }
    virtual std::shared_ptr<FilterableBlockMap<RGBA> > GetPreview(unsigned width, unsigned height) override;
};

/// Specifies a constant float
class SPRUE FloatNode : public TextureNode
{
public:
    IMPL_TEXTURE_NODE(FloatNode);
    float Value = 0.5f;

    virtual bool CanPreview() const { return true; }
    virtual std::shared_ptr<FilterableBlockMap<RGBA> > GetPreview(unsigned width, unsigned height) override;
};

/// Applies cosine on the inputs
class SPRUE CosNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(CosNode);
};

/// Applies sine on the inputs
class SPRUE SinNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SinNode);
};

/// Applies tan on the inputs
class SPRUE TanNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SinNode);
};

/// Applies expf on the inputs
class SPRUE ExpNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ExpNode);
};

/// Takes 2 inputs and applies powf(A, B), to all color channels as well
class SPRUE PowNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(PowNode);
};

/// Takes 2 inputs and applies sqrtf(A, B), to all color channels as well
class SPRUE SqrtNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(SqrtNode);
};

/// Takes a Color and returns the RGB average
class SPRUE AverageNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(AverageNode);
};

/// Takes a Color and returns the smallest of R, G, or B
class SPRUE MinNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(MaxNode);
};

/// Takes a Color and returns the largest of R, G, or B
class SPRUE MaxNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(MaxNode);
};


/// Clips inputs to a 0-1 range
class SPRUE Clamp01Node : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(Clamp01Node);
};

/// Splits an RGBA channel into R, G, B, A floats
class SPRUE SplitNode : public TextureNode
{
public:
    IMPL_TEXTURE_NODE(SplitNode);

    virtual bool CanPreview() const override { return false; }
    virtual unsigned GetClassVersion() const override { return 2; }
    virtual void VersionUpdate(unsigned fromVersion) override;
};

/// Combines several float values into an RGBA channel
class SPRUE CombineNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(CombineNode);

    virtual unsigned GetClassVersion() const override { return 2; }
    virtual void VersionUpdate(unsigned fromVersion) override;
};

/// Converts an RGB input into an HSV output (as RGB)
class SPRUE RGBToHSVNode : public TextureNode
{
public:
    IMPL_TEXTURE_NODE(RGBToHSVNode);
};

/// Convers an HSV input (as RGB) into an RGB output
class SPRUE HSVToRGBNode : public TextureNode
{
public:
    IMPL_TEXTURE_NODE(RGBToHSVNode);
};

/// Extracts the luminance from an RGB color
class SPRUE BrightnessRGBNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(BrightnessRGBNode);
};

enum TexGenBlendMode
{
    TGB_Normal,
    TGB_Additive,
    TGB_Subtract,
    TGB_Multiply,
    TGB_Divide,
    TGB_ColorBurn,
    TGB_LinearBurn,
    TGB_Screen,
    TGB_ColorDodge,
    TGB_LinearDodge,
    TGB_Dissolve,
    TGB_NormalMap,
};

enum TexGenBlendAlpha
{
    TGBA_UseWeight,
    TGBA_UseDestAlpha,
    TGBA_UseSourceAlpha
};

/// Uses blending modes to blend floats/Colors, optionally uses a blend weight
class SPRUE BlendNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(BlendNode);

    TexGenBlendMode GetBlendMode() const { return mode_; }
    void SetBlendMode(TexGenBlendMode mode) { mode_ = mode; }

private:
    TexGenBlendAlpha alphaMode_ = TGBA_UseWeight;
    TexGenBlendMode mode_;
};

class SPRUE BrightnessNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(BrightnessNode);
    float Power = 1.2f;
};

class SPRUE ContrastNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ContrastNode);
    float Power = 1.2f;
};

class SPRUE ToGammaNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(FromGammaNode);
    float Gamma = 2.2f;
};

class SPRUE FromGammaNode : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(FromGammaNode);
    float Gamma = 2.2f;
};

class SPRUE ToNormalizedRange : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ToNormalizedRange);

    RangedFloat range_;
};

class SPRUE FromNormalizedRange : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(FromNormalizedRange);

    RangedFloat range_;
};

}