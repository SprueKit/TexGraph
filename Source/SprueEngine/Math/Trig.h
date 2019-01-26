#pragma once

#include <SprueEngine/BlockMap.h>
#include <SprueEngine/Math/Color.h>
#include <SprueEngine/Math/Rect.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

namespace SprueEngine
{

    class MeshData;
    struct RasterizerData;

    void RasterizeTriangle(RasterizerData* rasterData, const Vec2* uvs, const RGBA* inputs);

    void CrossRasterizeTriangle(RasterizerData* rasterData, const Vec2* destUVs, const Vec3* pos, FilterableBlockMap<RGBA>& srcTex, MeshData* meshData);

    Rect CalculateTriangleImageBounds(int width, int height, const Vec2* uvs);

    bool IsPointContained(const Vec2* uvs, float x, float y);

    Vec3 GetBarycentricFactors(const Vec2* uvs, const Vec2& point);

    Vec3 GetBarycentricFactors(const Vec3* points, const Vec3& forPoint);

    RGBA TexelToWorldSpace(const Vec2* uvCoords, const Vec2& pointCoords, const Vec3* points);

    Vec3 AttributeToWorldSpace(const Vec2* uvCoords, const Vec2& pointCoords, const Vec3* points);

    RGBA TexelToWorldSpace(const Vec2* uvCoords, const Vec2& pointCoords, const RGBA* points);

    inline Vec3 ClosestPoint(const Vec3& a, const Vec3& b, const Vec3& point)
    {
        Vec3 dir = b - a;
        return a + CLAMP01((point - a).Dot(dir) / dir.LengthSq()) * dir;
    }

    inline Vec2 ClosestPoint(const Vec2& a, const Vec2& b, const Vec2& point)
    {
        Vec2 dir = b - a;
        return a + CLAMP01((point - a).Dot(dir) / dir.LengthSq()) * dir;
    }
}