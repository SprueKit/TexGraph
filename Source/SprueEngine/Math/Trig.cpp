#include "SprueEngine/Math/Trig.h"

#include "SprueEngine/Geometry/MeshData.h"
#include "SprueEngine/Math/MathDef.h"
#include "SprueEngine/Texturing/RasterizerData.h"

namespace SprueEngine
{

    /// Renders a triangle with the given UVs and inputs to be interpolated into the RasterizerData pixels
    void RasterizeTriangle(RasterizerData* rasterData, const Vec2* uvs, const RGBA* inputs)
    {
        const float xSize = 1.0f / rasterData->Width;
        const float ySize = 1.0f / rasterData->Width;
        const float halfX = xSize * 0.5f;
        const float halfY = xSize * 0.5f;

        // Calculate bounds to minimize the area sampled
        Rect triBounds = CalculateTriangleImageBounds(rasterData->Width, rasterData->Height, uvs);
        for (float y = floorf(triBounds.yMin); y <= ceilf(triBounds.yMax) && y < rasterData->Height; y += 1.0f)
        {
            if (!rasterData->OriginAtTop && y < 0)
                y *= -1;
            if (y < 0 || y >= rasterData->Height)
                continue;

            float yCoord = y / rasterData->Height;
            for (float x = floorf(triBounds.xMin); x <= ceilf(triBounds.xMax); x += 1.0f)
            {
                if (x < 0 || x >= rasterData->Width)
                    continue;

                float xCoord = x / rasterData->Width;
                Vec2 pt;
                pt.x = xCoord; pt.y = yCoord;

                // Fragment corners for conservative rasterization
                const float mul = rasterData->OriginAtTop ? 1.0f : -1.0f;
                const Vec2 pixelCorners[] = {
                    { pt.x, pt.y * mul },
                    { pt.x + xSize, pt.y * mul },
                    { pt.x, (pt.y + ySize) * mul },
                    { pt.x + xSize, (pt.y + ySize) * mul },
                };

                // If any corner is overlapped then we pass for rendering under conservative rasterization
                bool anyCornersContained = false;
                for (unsigned i = 0; i < 4; ++i)
                    anyCornersContained |= IsPointContained(uvs, pixelCorners[i].x, pixelCorners[i].y);

                if (anyCornersContained)//IsPointContained(uvs, xCoord, yCoord)) 
                {
                    RGBA writeColor = TexelToWorldSpace(uvs, pt, inputs);
                    //if (callback != 0x0)
                    //    callback(writeColor);

                    // If there's a mask than grab the appropriate mask pixel and multiply it with the write target
                    if (rasterData->Mask != 0x0)
                    {
                        float maskWidthFraction = (float)rasterData->MaskWidth / (float)rasterData->Width;
                        float maskHeightFraction = (float)rasterData->MaskHeight / (float)rasterData->Height;
                        int maskIndex = CLAMP((int)(x*maskWidthFraction), 0, rasterData->MaskWidth - 1) + CLAMP((int)(y*maskHeightFraction), 0, rasterData->MaskHeight - 1)*rasterData->MaskWidth;
                        rasterData->Pixels[(int)x + (int)y*rasterData->Width] = writeColor * rasterData->Mask[maskIndex];
                    }
                    else
                    {
                        rasterData->Pixels[(int)x + (int)y*rasterData->Width] = writeColor;
                    }

                    // If we have a mask then record that we've written there
                    if (rasterData->WrittenMask != 0x0)
                        rasterData->WrittenMask[(int)x + (int)y * rasterData->Width] = true;
                }
            }
        }
    }

    void CrossRasterizeTriangle(RasterizerData* rasterData, const Vec2* destUVs, const Vec3* destPos, FilterableBlockMap<RGBA>& srcTex, MeshData* mesh)
    {
        // Calculate bounds to minimize the area sampled
        Rect triBounds = CalculateTriangleImageBounds(rasterData->Width, rasterData->Height, destUVs);
        for (float y = triBounds.yMin; y <= triBounds.yMax && y < rasterData->Height; y += 1.0f)
        {
            if (y < 0 || y >= rasterData->Height)
                continue;

            float yCoord = y / rasterData->Height;
            for (float x = triBounds.xMin; x <= triBounds.xMax; x += 1.0f)
            {
                if (x < 0 || x >= rasterData->Width)
                    continue;

                float xCoord = x / rasterData->Width;
                Vec2 pt;
                pt.x = xCoord; pt.y = yCoord;
                if (IsPointContained(destUVs, xCoord, yCoord))
                {
                    Vec3 baryCoords = GetBarycentricFactors(destUVs, pt);
                    // Get point in world space for this write
                    Vec3 writePos = destPos[0] * baryCoords.x + destPos[1] * baryCoords.y + destPos[2] * baryCoords.z;
                    
                    Vec3 srcPos[3]; // Position of our src vertices
                    Vec2 srcUVs[3]; // UV coords of our src vertices
                    Vec3 closetSrcPoint = mesh->Closest(writePos, srcPos, srcUVs); // Closest point between src and dest points
                    Vec3 srcBary = GetBarycentricFactors(&srcPos[0], closetSrcPoint);
                    Vec2 srcSampleUV = srcUVs[0] * srcBary.x + srcUVs[1] * srcBary.y + srcUVs[2] * srcBary.z; // Compute UV at that point in world space

                    RGBA writeColor = srcTex.get(srcSampleUV.x, srcSampleUV.y); // sample the source texture

                    //if (callback != 0x0)
                    //    callback(writeColor);

                    // If there's a mask than grab the appropriate mask pixel and multiply it with the write target
                    if (rasterData->Mask != 0x0)
                    {
                        float maskWidthFraction = (float)rasterData->MaskWidth / (float)rasterData->Width;
                        float maskHeightFraction = (float)rasterData->MaskHeight / (float)rasterData->Height;
                        int maskIndex = CLAMP((int)(x*maskWidthFraction), 0, rasterData->MaskWidth - 1) + CLAMP((int)(y*maskHeightFraction), 0, rasterData->MaskHeight - 1)*rasterData->MaskWidth;
                        rasterData->Pixels[(int)x + (int)y*rasterData->Width] = writeColor * rasterData->Mask[maskIndex];
                    }
                    else
                    {
                        rasterData->Pixels[(int)x + (int)y*rasterData->Width] = writeColor;
                    }

                    // If we have a mask then record that we've written there
                    if (rasterData->WrittenMask != 0x0)
                        rasterData->WrittenMask[(int)x + (int)y * rasterData->Width] = true;
                }
            }
        }
    }

    bool IsPointContained(const Vec2* uvs, float x, float y)
    {
        Vec2 pt(x, y);
        Vec2 v0 = uvs[2] - uvs[0];
        Vec2 v1 = uvs[1] - uvs[0];
        Vec2 v2 = pt - uvs[0];

        float dot00 = v0.Dot(v0); //??
        float dot01 = v0.Dot(v1);
        float dot02 = v0.Dot(v2);
        float dot11 = v1.Dot(v1);
        float dot12 = v1.Dot(v2);

        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        return (u >= 0) && (v >= 0) && (u + v < 1.0f);
    }

    Rect CalculateTriangleImageBounds(int width, int height, const Vec2* uvs)
    {
        int xMin = (int)SprueMin(uvs[0].x * width, SprueMin(uvs[1].x * width, uvs[2].x * width));
        int xMax = (int)SprueMax(uvs[0].x * width, SprueMax(uvs[1].x * width, uvs[2].x * width));

        int yMax = (int)SprueMax(uvs[0].y * height, SprueMax(uvs[1].y * height, uvs[2].y * height));
        int yMin = (int)SprueMin(uvs[0].y * height, SprueMin(uvs[1].y * height, uvs[2].y * height));
        Rect r(xMin, yMin, xMax - xMin, yMax - yMin);
        r.xMin = xMin - 1; r.xMax = xMax + 1;
        r.yMin = yMin - 1; r.yMax = yMax + 1;
        return r;
    }

    Vec3 GetBarycentricFactors(const Vec2* uvs, const Vec2& point)
    {
        Vec2 v0 = uvs[2] - uvs[0];
        Vec2 v1 = uvs[1] - uvs[0];
        Vec2 v2 = point - uvs[0];

        float dot00 = v0.Dot(v0); //??
        float dot01 = v0.Dot(v1);
        float dot02 = v0.Dot(v2);
        float dot11 = v1.Dot(v1);
        float dot12 = v1.Dot(v2);

        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
        float w = 1.0f - v - u;

        return Vec3(u,v,w);
    }

    Vec3 GetBarycentricFactors(const Vec3* points, const Vec3& forPoint)
    {
        Vec3 f1 = points[0] - forPoint;
        Vec3 f2 = points[1] - forPoint;
        Vec3 f3 = points[2] - forPoint;

        float a = ((points[0] - points[1]).Cross(points[0] - points[2])).Length();
        float a1 = f2.Cross(f3).Length() / a;
        float a2 = f3.Cross(f1).Length() / a;
        float a3 = f1.Cross(f2).Length() / a;

        return Vec3(a1,a2,a3);
    }

// Three functions use this exact same code, opted against function call because the using methods are called ALOT
#define TEXEL_TO_WORLD_COMMON float T[2][2]; \
    float iT[2][2]; \
    memset(T, 0, sizeof(float) * 4); \
    memset(iT, 0, sizeof(float) * 4); \
    \
    T[0][0] = uvCoords[0].x - uvCoords[2].x; \
    T[0][1] = uvCoords[1].x - uvCoords[2].x; \
    T[1][0] = uvCoords[0].y - uvCoords[2].y; \
    T[1][1] = uvCoords[1].y - uvCoords[2].y; \
    \
    float d = T[0][0] * T[1][1] - T[0][1] * T[1][0]; \
    \
    iT[0][0] = T[1][1] / d;  \
    iT[0][1] = -T[0][1] / d; \
    iT[1][0] = -T[1][0] / d; \
    iT[1][1] = T[0][0] / d;  \
    \
    float lambda0 = iT[0][0] * (pointCoords.x - uvCoords[2].x) + iT[0][1] * (pointCoords.y - uvCoords[2].y);\
    float lambda1 = iT[1][0] * (pointCoords.x - uvCoords[2].x) + iT[1][1] * (pointCoords.y - uvCoords[2].y);\
    float lambda2 = 1.0f - lambda0 - lambda1;                                                               

    RGBA TexelToWorldSpace(const Vec2* uvCoords, const Vec2& pointCoords, const Vec3* points)
    {
        TEXEL_TO_WORLD_COMMON

        float x = points[0].x * lambda0 + points[1].x * lambda1 + points[2].x * lambda2;
        float y = points[0].y * lambda0 + points[1].y * lambda1 + points[2].y * lambda2;
        float z = points[0].z * lambda0 + points[1].z * lambda1 + points[2].z * lambda2;
        
        return RGBA(x, y, z, 1.0f);
    }

    Vec3 AttributeToWorldSpace(const Vec2* uvCoords, const Vec2& pointCoords, const Vec3* points)
    {
        TEXEL_TO_WORLD_COMMON

        float x = points[0].x * lambda0 + points[1].x * lambda1 + points[2].x * lambda2;
        float y = points[0].y * lambda0 + points[1].y * lambda1 + points[2].y * lambda2;
        float z = points[0].z * lambda0 + points[1].z * lambda1 + points[2].z * lambda2;
        
        return Vec3(x, y, z);
    }

    RGBA TexelToWorldSpace(const Vec2* uvCoords, const Vec2& pointCoords, const RGBA* points)
    {
        TEXEL_TO_WORLD_COMMON

        float x = points[0].r * lambda0 + points[1].r * lambda1 + points[2].r * lambda2;
        float y = points[0].g * lambda0 + points[1].g * lambda1 + points[2].g * lambda2;
        float z = points[0].b * lambda0 + points[1].b * lambda1 + points[2].b * lambda2;
        float a = points[0].a * lambda0 + points[1].a * lambda1 + points[2].a * lambda2;
        
        return RGBA(x,y,z,a);
    }
}