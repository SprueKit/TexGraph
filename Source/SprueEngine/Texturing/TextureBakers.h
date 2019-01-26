#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/BlockMap.h>
#include <SprueEngine/Geometry/MeshData.h>
#include <SprueEngine/Libs/nvmesh/halfedge/Mesh.h>
#include <SprueEngine/Resource.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{
    
struct RasterizerData;

/// Texture bakers are used to accelerate some baking operations, ie triplanar texturing
class TextureBaker
{
public:
    TextureBaker(nv::HalfEdge::Mesh* mesh, MeshData* meshData) : mesh_(mesh), meshData_(meshData){ }
    virtual FilterableBlockMap<RGBA>* Bake() const = 0;

    unsigned GetWidth() const { return width_; }
    unsigned GetHeight() const { return height_; }

    void SetWidth(unsigned value) { width_ = value; }
    void SetHeight(unsigned value) { height_ = value; }

protected:
    unsigned width_;
    unsigned height_;
    nv::HalfEdge::Mesh* mesh_;
    MeshData* meshData_;
};

#define DECL_TEXTURE_BAKER(TYPENAME) class SPRUE TYPENAME : public TextureBaker { BASECLASSDEF(TYPENAME, TextureBaker); NOCOPYDEF(TYPENAME); public: TYPENAME(nv::HalfEdge::Mesh* mesh, MeshData* meshData) : base(mesh, meshData) { } virtual FilterableBlockMap<RGBA>* Bake() const override; }

class AmbientOcclusionBaker : public TextureBaker
{
    BASECLASSDEF(AmbientOcclusionBaker, TextureBaker);
public:
    AmbientOcclusionBaker(nv::HalfEdge::Mesh* mesh, MeshData* meshData) :
        base(mesh, meshData), 
        scalingFactor_(1.0f) 
    { 
    }

    virtual FilterableBlockMap<RGBA>* Bake() const override;

    float GetScalingFactor() const { return scalingFactor_; }
    void SetScalingFactor(float value) { scalingFactor_ = value; }

    float GetDarkLimit() const { return darkLimit_; }
    void SetDarkLimit(float value) { darkLimit_ = value; }

private:
    float scalingFactor_;
    float darkLimit_ = 0.15f;
};

DECL_TEXTURE_BAKER(CurvatureBaker);

class DominantPlaneBaker : public TextureBaker
{
    BASECLASSDEF(DominantPlaneBaker, TextureBaker);
public:
    DominantPlaneBaker(MeshData* meshData) : base(0x0, meshData) { }

    virtual FilterableBlockMap<RGBA>* Bake() const override;

    bool IsPerVertex() const { return perVertex_; }
    void SetPerVertex(bool value) { perVertex_ = value; }

private:
    bool perVertex_;
};

DECL_TEXTURE_BAKER(ObjectSpaceNormalBaker);
DECL_TEXTURE_BAKER(ObjectSpacePositionBaker);
DECL_TEXTURE_BAKER(ObjectSpaceGradientBaker);
DECL_TEXTURE_BAKER(VertexColorBaker);

class FacetBaker : public TextureBaker
{
    BASECLASSDEF(FacetBaker, TextureBaker);
public:
    FacetBaker(MeshData* meshData) : base(0x0, meshData) { }

    virtual FilterableBlockMap<RGBA>* Bake() const override;

    bool IsInvert() const { return invert_; }
    void SetInvert(bool invert) { invert_ = invert; }

    float GetAngleThreshold() const { return angleThreshold_; }
    void SetAngleThreshold(float value) { angleThreshold_ = value; }

    bool IsAllEdges() const { return allEdges_; }
    void SetAllEdges(bool value) { allEdges_ = value; }

private:
    /// Afterwards the value will be flipped.
    bool invert_ = false;
    /// Angular threshold will be ignored and all edges will have lines drawn.
    bool allEdges_ = false;
    /// Dihedral angle must be greater than this to draw a line.
    float angleThreshold_ = 0.5f;
};

class PlanarProjectionBaker : public TextureBaker
{
    BASECLASSDEF(PlanarProjectionBaker, TextureBaker);
public:
    PlanarProjectionBaker(nv::HalfEdge::Mesh* mesh, MeshData* meshData) : base(mesh, meshData)
    { 
    }

    virtual FilterableBlockMap<RGBA>* Bake() const override;

    std::shared_ptr<BitmapResource> GetTexture() const{ return texture_; }
    void SetTexture(std::shared_ptr<BitmapResource> bitmap) { texture_ = bitmap; }

    void SetMaxNormalDot(float value) { maxDot_ = value; }
    void SetMinNormalDot(float value) { minDot_ = value; }

    float GetMaxNormalDot() const { return maxDot_; }
    float GetMinNormalDot() const { return minDot_; }

private:
    std::shared_ptr<BitmapResource> texture_;
    float minDot_;
    float maxDot_;
};

// This baker is 
class TriPlanarProjectionBaker : public TextureBaker
{
    BASECLASSDEF(TriPlanarProjectionBaker, TextureBaker);
public:
    TriPlanarProjectionBaker(nv::HalfEdge::Mesh* mesh, MeshData* meshData) :
        base(mesh, meshData),
        scaling_(1.0f, 1.0f, 1.0f)
    {

    }
    virtual FilterableBlockMap<RGBA>* Bake() const override;

    bool MixViaAlpha() const { return alphaBlendSeams_; }
    void SetMixViaAlpha(bool value) { alphaBlendSeams_ = value; }

    FilterableBlockMap<RGBA>* GetTextureX() const{ return xTexture_; }
    void SetTextureX(FilterableBlockMap<RGBA>* bitmap) { xTexture_ = bitmap; }

    FilterableBlockMap<RGBA>* GetTextureY() const{ return yTexture_; }
    void SetTextureY(FilterableBlockMap<RGBA>* bitmap) { yTexture_ = bitmap; }

    FilterableBlockMap<RGBA>* GetTextureZ() const{ return zTexture_; }
    void SetTextureZ(FilterableBlockMap<RGBA>* bitmap) { zTexture_ = bitmap; }

    Vec3 GetScaling() const { return scaling_; }
    void SetScaling(const Vec3& value) { scaling_ = value; }

private:
    void RasterizeTextured(const BoundingBox& objectBounds, RasterizerData* rasterData, Vec2* uv, Vec3* norms, Vec3* pos) const;

    FilterableBlockMap<RGBA>* yTexture_ = 0x0;
    FilterableBlockMap<RGBA>* xTexture_ = 0x0;
    FilterableBlockMap<RGBA>* zTexture_ = 0x0;
    Vec3 scaling_;
    bool alphaBlendSeams_;
};



}