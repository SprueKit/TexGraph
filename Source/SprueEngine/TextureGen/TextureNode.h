#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/Graph/GraphNode.h>
#include <SprueEngine/Graph/GraphSocket.h>

namespace SprueEngine
{

#define TEXGRAPH_RGBA   (1)
#define TEXGRAPH_FLOAT  (1 << 1)
#define TEXGRAPH_MASK (1 << 2)
#define TEXGRAPH_CHANNEL (1 | (1 << 1))

#define TEXGRAPH_PREVIEW_SIZE 128

#define IMPL_TEXTURE_NODE(TYPENAME) public: virtual StringHash GetTypeHash() const override { return StringHash( #TYPENAME ); } virtual const char* GetTypeName() const { return #TYPENAME; } virtual int Execute(const Variant& parameter) override; virtual void Construct() override; static void Register(Context*);

class SPRUE TextureNode : public GraphNode
{
public:
    static Vec4 Make4D(Vec2 coord, Vec2 tiling);
    static float CalculateStepSize(float stepSize, const Vec4& coordinates);
};

class SPRUE PreviewableNode : public TextureNode
{
public:
    virtual bool CanPreview() const override { return true; }
    virtual std::shared_ptr<FilterableBlockMap<RGBA>> GetPreview(unsigned width = TEXGRAPH_PREVIEW_SIZE, unsigned height = TEXGRAPH_PREVIEW_SIZE) override;
};

class SPRUE SelfPreviewableNode : public TextureNode
{
public:
    virtual bool CanPreview() const override { return true; }
    virtual std::shared_ptr<FilterableBlockMap<RGBA>> GetPreview(unsigned width = TEXGRAPH_PREVIEW_SIZE, unsigned height = TEXGRAPH_PREVIEW_SIZE) override;
};

enum TexGenOutputType
{
    TGOT_Albedo,
    TGOT_Roughness,
    TGOT_Glossiness,
    TGOT_Metallic,
    TGOT_Normal,
    TGOT_Specular,
    TGOT_SurfaceThickness,
    TGOT_Subsurface,
    TGOT_Height,
    TGOT_Custom,
};

enum TexGenOutputFormat
{
    TGOF_RGB,
    TGOF_RGBA,
    TGOF_Alpha,
};

class SPRUE TextureOutputNode : public TextureNode
{
public:
    IMPL_TEXTURE_NODE(TextureOutputNode);

    TexGenOutputType OutputType;
    TexGenOutputFormat Format;
    RGBA DefaultColor = RGBA(0, 0, 0, 1);
    unsigned Width = 256;
    unsigned Height = 256;

    virtual bool CanPreview() const override { return true; }
    virtual std::shared_ptr<FilterableBlockMap<RGBA>> GetPreview(unsigned width = TEXGRAPH_PREVIEW_SIZE, unsigned height = TEXGRAPH_PREVIEW_SIZE) override;
};

class Context;
void RegisterTextureNodes(Context*);

}