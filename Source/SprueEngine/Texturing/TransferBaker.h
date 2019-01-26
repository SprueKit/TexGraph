#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/BlockMap.h>

namespace SprueEngine
{

class MeshData;

/// Reproject a texture from one mesh onto another
class SPRUE ReprojectTransferBaker
{
    NOCOPYDEF(ReprojectTransferBaker);
    ReprojectTransferBaker(); // prevent construction period
public:
    /// Remap vertex UV coordinates to the best interpolated coordinate to batch a a source input
    static void ReprojectUV(MeshData* onto, MeshData* from);

    /// Bake data mapped for one object onto another (closest triangle point, barycentric rasterization) - EXPENSIVE!
    static void Reproject(MeshData* onto, MeshData* from, FilterableBlockMap<RGBA>* targetTexture, FilterableBlockMap<RGBA>* sourceTexture);
};

class SPRUE DisplacementTransferBaker
{
    NOCOPYDEF(DisplacementTransferBaker);
    DisplacementTransferBaker(); // Prevent construction
public:
    /// Turn a grayscale height map into an RGB normal map
    static FilterableBlockMap<RGBA>* ConvertToNormalMap(const FilterableBlockMap<float>* image);
};

/// Calculates normals
class SPRUE NormalTransferBaker
{
    NOCOPYDEF(NormalTransferBaker);
public:
    static void ComputeNormals(MeshData* dest, MeshData* source, FilterableBlockMap<RGBA>* image, float cageExtrusion);
};

}