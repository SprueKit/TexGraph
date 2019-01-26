#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>
#include <SprueEngine/Libs/FastNoise.h>

namespace SprueEngine
{

class SPRUE ScratchesGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ScratchesGenerator);
    unsigned Density = 18;
    float Length = 0.1f;
    bool Inverted = false;
    bool FadeOff = false;
private:
    FastNoise noise_;
};

enum TextureFunctionFunction
{
    TFF_None,
    TFF_Sin,
    TFF_Cos,
    TFF_Saw,
    TFF_Square,
    TFF_Triangle
};

enum TextureFunctionMix
{
    TFM_Add,
    TFM_Subtract,
    TFM_Mul,
    TFM_Divide,
    TFM_Max,
    TFM_Min,
    TFM_Pow
};

class SPRUE TextureFunction2D : public PreviewableNode
{
public:
    IMPL_TEXTURE_NODE(TextureFunction2D);
    TextureFunctionFunction XFunction = TFF_Sin;
    TextureFunctionFunction YFunction = TFF_Sin;
    Vec2 Offset;
    Vec2 Period = Vec2(16, 16);
    TextureFunctionMix Mix = TFM_Add;

    float DoFunction(float val, TextureFunctionFunction func) const;
};

class SPRUE ChainGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ChainGenerator);

    RGBA Background;
    RGBA CenterLink;
    RGBA ConnectingLink;
};

class SPRUE ChainMailGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ChainMailGenerator);

    Vec2 ChainSize = Vec2(1, 1);
    RGBA Background;
    RGBA CenterLink;
    RGBA ConnectingLink;

private:
    float Remap(float a, float b, float* ra);
    float Pattern(float r1, float r2, float r3, int fx, int fy, float Rm, float Rp);
};

class SPRUE ScalesGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(ScalesGenerator);

    Vec2 ScaleSize = Vec2(1, 1);
    RGBA EvenColor;
    RGBA OddColor;
};

class SPRUE OddBlocksGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(OddBlocksGenerator);

    float LineWidth = 1.0f;
    float Scale = 1.0f;
    float Offset = 2.0;
    Vec2 Period = Vec2(8, 8);
    FastNoise noise_;
};

}