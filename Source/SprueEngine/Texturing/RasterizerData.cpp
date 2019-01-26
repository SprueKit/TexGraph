#include <SprueEngine/Texturing/RasterizerData.h>

namespace SprueEngine
{

void PadEdges(RasterizerData* rasterData)
{
    if (rasterData->WrittenMask == 0x0)
        return;

    const unsigned& depth = rasterData->Depth;
    const unsigned& height = rasterData->Height;
    const unsigned& width = rasterData->Width;
    for (int z = 0; z < rasterData->Depth; ++z)
    {
        for (int y = 0; y < rasterData->Height; ++y)
        {
            for (int x = 0; x < rasterData->Width; ++x)
            {
                if (!rasterData->WrittenMask[ToArrayIndex(x, y, z, width, height, depth)])
                {
                    RGBA sum;
                    int sumCt = 0;

                    for (int zz = z - 1; zz <= z + 1; ++zz)
                    {
                        if (zz >= depth || zz < 0)
                            continue;

                        for (int yy = y - 1; yy <= y + 1; ++yy)
                        {
                            if (yy >= rasterData->Height || yy < 0)
                                continue;
                            for (int xx = x - 1; xx <= x + 1; ++xx)
                            {
                                if (xx >= rasterData->Width || xx < 0)
                                    continue;
                            
                                // Only concerned about properly written pixels
                                if (rasterData->WrittenMask[ToArrayIndex(xx, yy, zz, width, height, depth)])
                                {
                                    sumCt += 1;
                                    sum += rasterData->Pixels[ToArrayIndex(xx, yy, zz, width, height, depth)];
                                }
                            }
                        }
                    }
                    if (sumCt > 0)
                        rasterData->Pixels[ToArrayIndex(x, y, z, width, height, depth)] = sum * (1.0f / (float)sumCt);
                }
            }
        }
    }
}

void PadEdges(RasterizerData* rasterData, int passes)
{
    if (rasterData->WrittenMask == 0x0)
        return;

    for (unsigned i = 0; i < passes; ++i)
    {
        bool* newWrittenMask = new bool[rasterData->Width * rasterData->Height * rasterData->Depth];
        for (unsigned w = 0; w < rasterData->Height * rasterData->Width; ++w)
            newWrittenMask[w] = false;

        for (int z = 0; z < rasterData->Depth; ++z)
        {
            for (int y = 0; y < rasterData->Height; ++y)
            {
                for (int x = 0; x < rasterData->Width; ++x)
                {
                    const unsigned writeIndex = ToArrayIndex(x, y, z, rasterData->Width, rasterData->Height, rasterData->Depth);
                    if (!rasterData->WrittenMask[writeIndex])
                    {
                        RGBA sum;
                        int sumCt = 0;

                        for (int zz = z - 1; zz <= z + 1; ++zz)
                        {
                            if (zz >= rasterData->Depth|| zz < 0)
                                continue;

                            for (int yy = y - 1; yy <= y + 1; ++yy)
                            {
                                if (yy >= rasterData->Height || yy < 0)
                                    continue;
                                for (int xx = x - 1; xx <= x + 1; ++xx)
                                {
                                    if (xx >= rasterData->Width || xx < 0)
                                        continue;
                                    if (xx == x && /*zz == z &&*/ yy == y)
                                        continue;

                                    // Only concerned about properly written pixels
                                    const unsigned readIdx = ToArrayIndex(xx, yy, zz, rasterData->Width, rasterData->Height, rasterData->Depth);
                                    if (rasterData->WrittenMask[readIdx])
                                    {
                                        sumCt += 1;
                                        sum += rasterData->Pixels[readIdx];
                                    }
                                }
                            }
                        }
                        if (sumCt > 0)
                        {
                            rasterData->Pixels[writeIndex] = sum * (1.0f / (float)sumCt);
                            newWrittenMask[writeIndex] = true;
                        }
                        else
                            newWrittenMask[writeIndex] = false;
                    }
                    else
                        newWrittenMask[writeIndex] = rasterData->WrittenMask[writeIndex];
                }
            }
        }

        delete[] rasterData->WrittenMask;
        rasterData->WrittenMask = newWrittenMask;
    }
}

void FillUnwritten(RasterizerData* raster, const RGBA& color)
{
    if (raster->WrittenMask == 0x0)
        return;
    for (unsigned z = 0; z < raster->Depth; ++z)
    {
        for (unsigned y = 0; y < raster->Height; ++y)
        {
            for (unsigned x = 0; x < raster->Width; ++x)
            {
                if (!raster->WrittenMask[ToArrayIndex(x, y, z, raster->Width, raster->Height, raster->Depth)])
                    raster->Pixels[ToArrayIndex(x, y, z, raster->Width, raster->Height, raster->Depth)] = color;
            }
        }
    }
}

void InvertRasterizer(RasterizerData* raster)
{
    for (unsigned z = 0; z < raster->Depth; ++z)
    {
        for (unsigned y = 0; y < raster->Height; ++y)
        {
            for (unsigned x = 0; x < raster->Width; ++x)
            {
                const int index = ToArrayIndex(x, y, z, raster->Width, raster->Height, raster->Depth);
                if (raster->WrittenMask[index])
                {
                    RGBA curColor = raster->Pixels[index];
                    raster->Pixels[index] = RGBA(1.0f - curColor.r, 1.0f - curColor.g, 1.0f - curColor.b);
                }
            }
        }
    }
}

void RasterizerDrawLine(RasterizerData* raster, const RGBA& color, const Vec2& from, const Vec2& to)
{
    float x1 = from.x * raster->Width;
    float y1 = from.y * raster->Height;
    float x2 = to.x * raster->Width;
    float y2 = to.y * raster->Height;

    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if (steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;
    for (int x = (int)x1; x<maxX; x++)
    {
        int index = 0;
        if (steep)
            index = ToArrayIndex(y, x, 1, raster->Width, raster->Height, raster->Depth);
        else
            index = ToArrayIndex(x, y, 1, raster->Width, raster->Height, raster->Depth);
        
        raster->Pixels[index] = color;
        raster->WrittenMask[index] = true;

        error -= dy;
        if (error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}

}