#pragma once

#include <SprueEngine/Math/Color.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{

    /// Data-container for the data used for rasterization
    struct RasterizerData
    {
        ~RasterizerData()
        {
            if (WrittenMask)
                delete[] WrittenMask;
        }

        /// Pixel data that is to be written to.
        RGBA* Pixels;
        Vec3* Normals = 0x0;
        Vec3* Positions = 0x0;
        /// Optional mask that will record which pixels have been written and which have not. May be null.
        bool* WrittenMask = 0x0;
        /// Width of the texture data.
        int Width;
        /// Height of the texture data.
        int Height;
        /// Depth of the texture data (in pseudo3d this is used as a hack value).
        int Depth;
        /// Where is the rasterizer's origin at?
        bool OriginAtTop = true;

        /// Optional mask that will modulate the written values. May be null.
        RGBA* Mask = 0x0;
        /// Width of the "Mask" data.
        int MaskWidth;
        /// Height of the "Mask" data.
        int MaskHeight;
        /// Depth of the "Mask" data.
        int MaskDepth;

        /// For projective techniques (particularly Triplanar (as base vector) and projective).
        Vec3 ProjectionVector;
        /// Pixel data for the projected texture. May be null.
        RGBA* ProjectedTexture = 0x0;
        /// Width of the projected texture.
        int ProjectedTexWidth;
        /// Height of the projected texture.
        int ProjectedTexHeight;
        /// Scaling factor for the projected texture.
        Vec2 TextureScale;
    };

    void PadEdges(RasterizerData* raster);
    void PadEdges(RasterizerData* raster, int passes);
    void InvertRasterizer(RasterizerData* raster);
    void FillUnwritten(RasterizerData* raster, const RGBA& color);
    void RasterizerDrawLine(RasterizerData* raster, const RGBA& color, const Vec2& from, const Vec2& to);
}