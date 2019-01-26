#include "TransferBaker.h"

#include <SprueEngine/GeneralUtility.h>
#include <SprueEngine/Geometry/MeshData.h>
#include <SprueEngine/Texturing/RasterizerData.h>
#include <SprueEngine/Math/Triangle.h>
#include <SprueEngine/Math/Trig.h>

namespace SprueEngine
{

void ReprojectTransferBaker::ReprojectUV(MeshData* onto, MeshData* from)
{
    const unsigned destIndexCount = onto->indexBuffer_.size();
    const unsigned srcIndexCount = from->indexBuffer_.size();
    
    const unsigned destVertexCount = onto->GetPositionBuffer().size();
    const unsigned srcVertexCount = from->GetPositionBuffer().size();

    std::vector<Vec2> remappedUV;
    remappedUV.resize(destVertexCount);

    for (unsigned ti = 0; ti < destIndexCount; ti += 3)
    {
        std::vector<float> triangleWeights;
        triangleWeights.resize(destIndexCount / 3);
        memset(triangleWeights.data(), 0, sizeof(float) * triangleWeights.size());

        /// Iterate through each dest vert of this triangle
        for (unsigned i = 0; i < 3; ++i)
        {
            Vec3 thisVert = onto->GetPositionBuffer()[ti + i];

            for (unsigned o = 0; o < srcIndexCount; o += 3)
            {
                Triangle tri(from->GetPositionBuffer()[o], from->GetPositionBuffer()[o + 1], from->GetPositionBuffer()[o + 2]);
                Vec3 nearest = tri.ClosestPoint(thisVert);
                triangleWeights[o / 3] += (nearest - thisVert).LengthSq();
            }
        }

        unsigned bestTri = std::min_element(triangleWeights.begin(), triangleWeights.end()) - triangleWeights.begin();

        // iterate thorugh them again
        for (unsigned i = 0; i < 3; ++i)
        {
            Triangle tri(from->GetPositionBuffer()[bestTri * 3], from->GetPositionBuffer()[bestTri * 3 + 1], from->GetPositionBuffer()[bestTri * 3 + 2]);
            Vec3 thisVert = onto->GetPositionBuffer()[ti + i];
            Vec3 nearest = tri.ClosestPoint(thisVert);
            
            Vec3 bary = GetBarycentricFactors(tri.data(), nearest);
            Vec2* srcCoords = &from->GetUVBuffer()[bestTri * 3];
            remappedUV[ti + i] = srcCoords[0] * bary.x + srcCoords[1] * bary.y + srcCoords[2] * bary.z;
        }
    }

    onto->uvBuffer_ = remappedUV;
}

void ReprojectTransferBaker::Reproject(MeshData* onto, MeshData* from, FilterableBlockMap<RGBA>* targetTexture, FilterableBlockMap<RGBA>* sourceTexture)
{
    RasterizerData rasterData;
    rasterData.Pixels = targetTexture->getData();
    rasterData.WrittenMask = new bool[targetTexture->getWidth() * targetTexture->getHeight() * targetTexture->getDepth()];
    rasterData.Width = targetTexture->getWidth();
    rasterData.Height = targetTexture->getHeight();
    rasterData.Depth = targetTexture->getDepth();

    for (unsigned i = 0; i < onto->indexBuffer_.size() / 3; i += 3)
    {
        Vec2 uv[3];
        uv[0] = onto->GetUVBuffer()[i];
        uv[1] = onto->GetUVBuffer()[i + 1];
        uv[2] = onto->GetUVBuffer()[i + 2];

        Vec3 pos[3];
        pos[0] = onto->GetPositionBuffer()[i];
        pos[1] = onto->GetPositionBuffer()[i + 1];
        pos[2] = onto->GetPositionBuffer()[i + 2];

        CrossRasterizeTriangle(&rasterData, uv, pos, *sourceTexture, from);
    }
}

FilterableBlockMap<RGBA>* DisplacementTransferBaker::ConvertToNormalMap(const FilterableBlockMap<float>* image)
{
    Vec2 offset(2.0f, 0.0f);

    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(image->getWidth(), image->getHeight());
    ret->fill(RGBA(0.0f, 1.0f, 0.0f));

    for (unsigned y = 0; y < image->getHeight(); ++y)
    {
        for (unsigned x = 0; x < image->getWidth(); ++x)
        {
            int xI = (int)x;
            int yI = (int)y;

            float s11 = image->get(x, y);
            float s01 = image->get(x - 1, y);
            float s21 = image->get(x + 1, y);
            float s10 = image->get(x, y - 1);
            float s12 = image->get(x, y + 1);

            Vec3 va = Vec3(offset.x, offset.y, s21 - s01).Normalized();
            Vec3 vb = Vec3(offset.x, offset.y, s12 - s10).Normalized();
            RGBA write;
            write.Set(va.Cross(vb));
            ret->set(write, x, y);
        }
    }

    return ret;
}

inline Vec3 ToTangentNormal(const Vec3& osNormal, const Vec3& destNormal, const Vec3& tangent, const Vec3& bitangent)
{
    return Vec3(osNormal.Dot(bitangent), osNormal.Dot(tangent), osNormal.Dot(destNormal)).Normalized();
}

void RasterizeTangentNormals(RasterizerData* rasterData, const Vec2* destUVs, const Vec3* destPos, const Vec3* destNormal, const Vec3* destTan, const Vec3* destBi, MeshData* mesh)
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
                const Vec3 baryCoords = GetBarycentricFactors(destUVs, pt);
                // Get point in world space for this write
                const Vec3 writePos = destPos[0] * baryCoords.x + destPos[1] * baryCoords.y + destPos[2] * baryCoords.z;
                const Vec3 writeNorm = destNormal[0] * baryCoords.x + destNormal[1] * baryCoords.y + destNormal[2] * baryCoords.z;
                const Vec3 writeTan = destTan[0] * baryCoords.x + destTan[1] * baryCoords.y + destTan[2] * baryCoords.z;
                const Vec3 writeBit = destBi[0] * baryCoords.x + destBi[1] * baryCoords.y + destBi[2] * baryCoords.z;

                Vec3 srcPos[3]; // Position of our src vertices
                Vec3 srcNorm[3]; // Normals coords of our src vertices
                const Vec3 closetSrcPoint = mesh->Closest(writePos, srcPos, 0x0, srcNorm); // Closest point between src and dest points
                const Vec3 srcBary = GetBarycentricFactors(&srcPos[0], closetSrcPoint);
                Vec3 interpolatedNormal = (srcNorm[0] * srcBary.x + srcNorm[1] * srcBary.y + srcNorm[2] * srcBary.z).Normalized();
                interpolatedNormal = ToTangentNormal(interpolatedNormal, writeNorm, writeTan, writeBit) + 1.0f * 0.5f;

                const RGBA writeColor(interpolatedNormal.x, interpolatedNormal.y, interpolatedNormal.z);

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

void NormalTransferBaker::ComputeNormals(MeshData* dest, MeshData* source, FilterableBlockMap<RGBA>* image, float cageExtrusion)
{
    if (dest->normalBuffer_.empty())
        dest->CalculateNormals();
    if (dest->tangentBuffer_.empty())
        dest->CalculateTangents();

    RasterizerData rasterData;
    rasterData.Pixels = image->getData();
    rasterData.WrittenMask = new bool[image->getWidth() * image->getHeight() * image->getDepth()];
    rasterData.Width = image->getWidth();
    rasterData.Height = image->getHeight();
    rasterData.Depth = image->getDepth();

    for (unsigned i = 0; i < dest->indexBuffer_.size(); ++i)
    {
        Vec2 uv[3]; // UV coordinates for rasterizing to
        uv[0] = dest->uvBuffer_[i ];
        uv[1] = dest->uvBuffer_[i + 1];
        uv[2] = dest->uvBuffer_[i + 2];

        Vec3 n[3]; // Normals
        n[0] = dest->normalBuffer_[i];
        n[1] = dest->normalBuffer_[i + 1];
        n[2] = dest->normalBuffer_[i + 2];

        Vec3 p[3]; // Object space position, includes cage extrusion along the vertex normals
        p[0] = dest->positionBuffer_[i] + n[0] * cageExtrusion;
        p[1] = dest->positionBuffer_[i + 1] + n[0] * cageExtrusion;
        p[2] = dest->positionBuffer_[i + 2] + n[0] * cageExtrusion;

        Vec3 t[3]; // Tangets
        t[0] = dest->tangentBuffer_[i];
        t[1] = dest->tangentBuffer_[i + 1];
        t[2] = dest->tangentBuffer_[i + 2];

        Vec3 b[3]; // Bitangets
        b[0] = t[0].Cross(n[0]) * dest->tangentBuffer_[i].w;
        b[1] = t[0].Cross(n[0]) * dest->tangentBuffer_[i].w;
        b[2] = t[0].Cross(n[0]) * dest->tangentBuffer_[i].w;
        //b[0] = dest->bitangentBuffer_[i];
        //b[1] = dest->bitangentBuffer_[i + 1];
        //b[2] = dest->bitangentBuffer_[i + 2];

        RasterizeTangentNormals(&rasterData, uv, p, n, t, b, source);
    }
}

}