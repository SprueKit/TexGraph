#pragma once

#include <SprueEngine/ClassDef.h>

#include <SprueEngine/Math/Color.h>
#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/Math/RangedValue.h>
#include <SprueEngine/Resource.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <memory>

namespace SprueEngine
{

enum MaterialProjection
{
    MP_Trilinear,
    MP_TrilinearClean,
    MP_TrilinearSmooth,
    MP_XAxis,
    MP_YAxis,
    MP_ZAxis,
};

class TextureGenerator;

class SPRUE Material
{
public:
    ColorRamp AlbedoRamp;
    RangedFloat RoughnessRange;
    RangedFloat MetalnessRange;
    RangedFloat Displacement;   // Height-field displacement
    
    struct TexSetting
    {
        std::shared_ptr<BitmapResource> Bitmap;         // Bitmap used
        std::shared_ptr<Graph> Generator;    // Procedural generator used
        Vec2 BitmapTiling;      // In tiles per "biggest object dimensions" final formula: ((1.0f / LargestDim) * _____Tiling)
        Vec2 GeneratorTiling;   //      Texture coords are normalized to 0.0-1.0 of the maxdim as well
        unsigned GeneratorSeed;
        bool ModulateGeneratorWithTexture;
        bool ModulateTextureWithRamp;
    };

    TexSetting AlbedoTexture;
    TexSetting RoughnessTexture;
    TexSetting MetalnessTexture;
    TexSetting DisplacementTexture;

    unsigned GeneratorSeed;
};

}