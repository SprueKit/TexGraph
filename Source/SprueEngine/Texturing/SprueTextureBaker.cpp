#include "SprueTextureBaker.h"

#include <SprueEngine/Logging.h>
#include <SprueEngine/Texturing/RasterizerData.h>
#include <SprueEngine/Core/SprueModel.h>
#include <SprueEngine/Core/Components/TexturingComponent.h>
#include <SprueEngine/Math/Trig.h>
#include <SprueEngine/Texturing/Sampling.h>

namespace SprueEngine
{

#define DECL_RASTER  RasterizerData rasterData; \
    rasterData.Pixels = ret->getData(); \
    rasterData.Mask = 0x0; \
    rasterData.WrittenMask = new bool[ret->getWidth() * ret->getHeight() * ret->getDepth()]; \
    memset(rasterData.WrittenMask, 0, ret->getWidth() * ret->getHeight() * ret->getDepth()); \
    rasterData.Width = ret->getWidth(); \
    rasterData.Height = ret->getHeight(); \
    rasterData.Depth = ret->getDepth();

    SprueTextureBaker::SprueTextureBaker(SprueModel* model, const std::vector<TexturingComponent*>& components) :
        TextureBaker(0x0, 0x0),
        model_(model),
        texturingComponents_(components)
    {
        SetWidth(1024);
        SetHeight(1024);
    }

    SprueTextureBaker::~SprueTextureBaker()
    {

    }

    FilterableBlockMap<RGBA>* SprueTextureBaker::Bake() const
    {
        FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
        DECL_RASTER;

        for (unsigned i = 0; i < model_->GetMeshedParts()->GetMeshCount(); ++i)
        {
            const auto mesh = model_->GetMeshedParts()->GetMesh(i);
            if (mesh->uvBuffer_.empty() || mesh->normalBuffer_.empty() || mesh->positionBuffer_.empty())
                continue;
            for (unsigned i = 0; i < mesh->indexBuffer_.size(); i += 3)
            {
                const unsigned index0 = mesh->indexBuffer_[i];
                const unsigned index1 = mesh->indexBuffer_[i + 1];
                const unsigned index2 = mesh->indexBuffer_[i + 2];

                Vec2 uv[3];
                uv[0] = mesh->uvBuffer_[index0];
                uv[1] = mesh->uvBuffer_[index1];
                uv[2] = mesh->uvBuffer_[index2];

                Vec3 p[3];
                p[0] = mesh->positionBuffer_[index0];
                p[1] = mesh->positionBuffer_[index1];
                p[2] = mesh->positionBuffer_[index2];

                Vec3 n[3];
                n[0] = mesh->normalBuffer_[index0];
                n[1] = mesh->normalBuffer_[index1];
                n[2] = mesh->normalBuffer_[index2];

                RasterizeTextured(&rasterData, uv, n, p);
            }
        }

        PadEdges(&rasterData, 4);
        return ret;
    }

    void SprueTextureBaker::RasterizeTextured(RasterizerData* rasterData, Vec2* uv, Vec3* norms, Vec3* pos) const
    {
        const float xSize = 1.0f / rasterData->Width;
        const float ySize = 1.0f / rasterData->Width;

        Rect triBounds = CalculateTriangleImageBounds(rasterData->Width, rasterData->Height, uv);
        for (float y = triBounds.yMin; y <= triBounds.yMax; y += 1.0f)
        {
            if (y < 0 || y >= rasterData->Height)
                continue;

            float yCoord = y / rasterData->Height;
            for (float x = triBounds.xMin; x <= triBounds.xMax; x += 1.0f)
            {
                if (x < 0 || x >= rasterData->Width)
                    continue;

                float xCoord = x / rasterData->Width;
                Vec2 pt(xCoord, yCoord);

                // Fragment corners for conservative rasterization
                const float mul = rasterData->OriginAtTop ? 1.0f : -1.0f;
                const Vec2 pixelCorners[] = {
                    { pt.x, pt.y * mul },
                    { pt.x + xSize, pt.y * mul },
                    { pt.x, (pt.y + ySize) * mul },
                    { pt.x + xSize, (pt.y + ySize) * mul },
                    { pt.x + xSize*0.5f, (pt.y+ySize*0.5f) * mul },
                };

                // If any corner is overlapped then we pass for rendering under conservative rasterization
                bool anyCornersContained = false;
                for (unsigned i = 0; i < 5; ++i)
                    anyCornersContained |= IsPointContained(uv, pixelCorners[i].x, pixelCorners[i].y);

                if (anyCornersContained) //if (IsPointContained(uv, xCoord, yCoord))
                {
                    Vec3 interpolatedNormal = AttributeToWorldSpace(uv, pt, norms);
                    Vec3 interpolatedPosition = AttributeToWorldSpace(uv, pt, pos);

                    RGBA toWrite = RGBA::Invalid;
                    for (auto comp : texturingComponents_)
                    {
                        RGBA writeColor = comp->SampleColorProjection(interpolatedPosition, interpolatedNormal);
                        if (writeColor.IsValid())
                            toWrite = writeColor;
                    }
                    if (toWrite.IsValid())
                    {
                        const int writeIndex = ToArrayIndex(x, y, 0, rasterData->Width, rasterData->Height, rasterData->Depth);
                        // If there's a mask than grab the appropriate mask pixel and multiply it with the write target
                        rasterData->Pixels[writeIndex] = toWrite;
                        if (rasterData->WrittenMask)
                            rasterData->WrittenMask[writeIndex] = true;
                    }
                }
            }
        }
    }
}