#include <SprueEngine/Texturing/TextureBakers.h>

#include <SprueEngine/Logging.h>
#include <SprueEngine/Texturing/RasterizerData.h>
#include <SprueEngine/Math/Trig.h>
#include <SprueEngine/Texturing/Sampling.h>

#include <SprueEngine/Libs/nvmesh/halfedge/Mesh.h>
#include <SprueEngine/Libs/nvmesh/halfedge/Vertex.h>
#include <SprueEngine/Libs/nvmesh/halfedge/Face.h>
#include <SprueEngine/Libs/nvmesh/halfedge/Edge.h>

using namespace nv::HalfEdge;

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

FilterableBlockMap<RGBA>* AmbientOcclusionBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    DECL_RASTER;

    std::vector<float> vertexWeights;
    vertexWeights.resize(meshData_->GetNormalBuffer().size());
    memset(vertexWeights.data(), 0, sizeof(float) * vertexWeights.size());
    float maxVal = 0.0f;
    float minVal = 0.0f;

    std::vector<float> radii(meshData_->positionBuffer_.size(), 0.0f);
    for (unsigned i = 0; i < meshData_->positionBuffer_.size(); ++i)
    {
        auto vert = mesh_->vertexAt(i);
        auto edge = vert->edges();
        float radius = 0.0f;
        while (!edge.isDone())
        {
            radius = std::max(radius, (edge.current()->to()->pos - vert->pos).Length());
            edge.advance();
        }
        radii[i] = radius;
    }

    for (unsigned i = 0; i < meshData_->positionBuffer_.size(); ++i)
    {
        float thisRadii = radii[i];
        auto thisPos = meshData_->positionBuffer_[i];
        auto thisNorm = meshData_->normalBuffer_[i];
        auto plane = Plane(thisPos, thisNorm);
        for (unsigned j = 0; j < meshData_->positionBuffer_.size(); ++j)
        {
            if (j == i)
                continue;
            float otherRadii = radii[j];
            auto otherPos = meshData_->positionBuffer_[j];
            auto otherNorm = meshData_->normalBuffer_[j];

            if (plane.SignedDistance(otherPos) > 0.0f && otherPos != thisPos)
                vertexWeights[i] += fabsf(thisNorm.Dot(otherNorm)) * std::max(otherRadii, thisRadii);
        }
        maxVal = std::max(maxVal, vertexWeights[i]);
    }

    for (unsigned i = 0; i < vertexWeights.size(); ++i)
        vertexWeights[i] = NORMALIZE(vertexWeights[i], 0.0f, maxVal);

    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        unsigned i0 = meshData_->indexBuffer_[i];
        unsigned i1 = meshData_->indexBuffer_[i + 1];
        unsigned i2 = meshData_->indexBuffer_[i + 2];

        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[i0];
        uv[1] = meshData_->uvBuffer_[i1];
        uv[2] = meshData_->uvBuffer_[i2];

        float weights[3];
        weights[0] = vertexWeights[i0];
        weights[1] = vertexWeights[i1];
        weights[2] = vertexWeights[i2];

        RGBA colors[3];
        colors[0] = RGBA::White * (1.0f - weights[0]);
        colors[1] = RGBA::White * (1.0f - weights[1]);
        colors[2] = RGBA::White * (1.0f - weights[2]);
        colors[0].a = 1.0f;
        colors[1].a = 1.0f;
        colors[2].a = 1.0f;

        RasterizeTriangle(&rasterData, uv, colors);
    }

    PadEdges(&rasterData);
    return ret;
}

FilterableBlockMap<RGBA>* CurvatureBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    DECL_RASTER;

    std::vector<float> curvatures;
    curvatures.reserve(32);
    std::vector<float> vertexWeights;
    vertexWeights.resize(meshData_->GetNormalBuffer().size());
    memset(vertexWeights.data(), 0, sizeof(float) * vertexWeights.size());
    float maxVal = 0.0f;
    float minVal = 0.0f;

    /// Compute vertex curvatures
    for (unsigned i = 0; i < mesh_->edgeCount(); ++i)
    {
        curvatures.clear();
        Edge* start = mesh_->edgeAt(i);
        Edge* current = start;
    
        /// Don't calculate vertices that have definitely been computed
        if (vertexWeights[start->vertex->id] != 0.0f)
            continue;
    
        Vec3 thisPos = start->vertex->pos;
        Vec3 thisNormal = start->vertex->nor;
        math::Plane plane(thisPos, thisNormal);
        do {
            if (current == 0x0)
                break;
            
            Vec3 otherPos = current->to()->pos;
            curvatures.push_back(plane.SignedDistance(otherPos));
    
        } while ((current = current->circular()) != start);
    
        float val = Average(curvatures.data(), curvatures.size());
        vertexWeights[start->vertex->id] = val;
        start->vertex->SynchronizeColocals(vertexWeights, val);
        maxVal = std::max(maxVal, vertexWeights[start->vertex->id]);
        minVal = std::min(minVal, vertexWeights[start->vertex->id]);
    }

    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        unsigned i0 = meshData_->indexBuffer_[i];
        unsigned i1 = meshData_->indexBuffer_[i+1];
        unsigned i2 = meshData_->indexBuffer_[i+2];

        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[i0];
        uv[1] = meshData_->uvBuffer_[i1];
        uv[2] = meshData_->uvBuffer_[i2];

        float weights[3];
        weights[0] = vertexWeights[i0];
        weights[1] = vertexWeights[i1];
        weights[2] = vertexWeights[i2];

        RGBA colors[3];
        colors[0] = weights[0] < 0.0f ? RGBA::Red * NORMALIZE(fabs(weights[0]), 0.0f, fabs(minVal)) : RGBA::Green * weights[0] / maxVal;
        colors[1] = weights[1] < 0.0f ? RGBA::Red * NORMALIZE(fabs(weights[1]), 0.0f, fabs(minVal)) : RGBA::Green * weights[1] / maxVal;
        colors[2] = weights[2] < 0.0f ? RGBA::Red * NORMALIZE(fabs(weights[2]), 0.0f, fabs(minVal)) : RGBA::Green * weights[2] / maxVal;

        RasterizeTriangle(&rasterData, uv, colors);
    }

    PadEdges(&rasterData);
    return ret;
}

static const Vec3 PlaneAxes[] = {
    Vec3::PositiveY,
    Vec3::NegativeY,
    Vec3::NegativeX,
    Vec3::PositiveX,
    Vec3::PositiveZ,
    Vec3::NegativeZ
};

static const RGBA PlaneColors[] = {
    RGBA(0.5f, 1.0f, 0.5f),
    RGBA(0.5f, 0.0f, 0.5f),
    RGBA(0.0f, 0.5f, 0.5f),
    RGBA(1.0f, 0.5f, 0.5f),
    RGBA(0.5f, 0.5f, 1.0f),
    RGBA(0.5f, 0.5f, 0.0f)
};

FilterableBlockMap<RGBA>* DominantPlaneBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    DECL_RASTER;
    
    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        const unsigned index0 = meshData_->indexBuffer_[i];
        const unsigned index1 = meshData_->indexBuffer_[i + 1];
        const unsigned index2 = meshData_->indexBuffer_[i + 2];

        //TriangleData* triData = mesh_->triangles[i];
        
        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[index0];
        uv[1] = meshData_->uvBuffer_[index1];
        uv[2] = meshData_->uvBuffer_[index2];

        if (IsPerVertex())
        {
            Vec3 p[3];
            p[0] = meshData_->positionBuffer_[index0];
            p[1] = meshData_->positionBuffer_[index1];
            p[2] = meshData_->positionBuffer_[index2];

            const Vec3 normal = (p[1] - p[0]).Cross(p[2] - p[0]);
            float bestDot = -1.0f;
            int bestIndex = 0;
            for (int i = 0; i < 6; ++i)
            {
                float dot = normal.Dot(PlaneAxes[i]);
                if (dot > bestDot)
                {
                    bestDot = dot;
                    bestIndex = i;
                }
            }

            RGBA colors[3];
            colors[0] = PlaneColors[bestIndex];
            colors[1] = PlaneColors[bestIndex];
            colors[2] = PlaneColors[bestIndex];
            //colors[0].Set(p[0]);
            //colors[1].Set(p[1]);
            //colors[2].Set(p[2]);

            RasterizeTriangle(&rasterData, uv, colors);
        }
        else
        {
            Vec3 p[3];
            p[0] = meshData_->normalBuffer_[index0];
            p[1] = meshData_->normalBuffer_[index1];
            p[2] = meshData_->normalBuffer_[index2];

            int bestIndex[3];
            memset(bestIndex, 0, sizeof(int) * 4);
            for (int idx = 0; idx < 3; ++idx)
            {
                float bestDot = -1.0f;
                for (int i = 0; i < 6; ++i)
                {
                    float dot = p[idx].Dot(PlaneAxes[i]);
                    if (dot > bestDot)
                    {
                        bestDot = dot;
                        bestIndex[idx] = i;
                    }
                }
            }

            RGBA colors[3];
            colors[0] = PlaneColors[bestIndex[0]];
            colors[1] = PlaneColors[bestIndex[1]];
            colors[2] = PlaneColors[bestIndex[2]];

            RasterizeTriangle(&rasterData, uv, colors);
        }
    }
    PadEdges(&rasterData);
    return ret;
}

FilterableBlockMap<RGBA>* ObjectSpaceNormalBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    DECL_RASTER;
    
    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        const unsigned index0 = meshData_->indexBuffer_[i];
        const unsigned index1 = meshData_->indexBuffer_[i + 1];
        const unsigned index2 = meshData_->indexBuffer_[i + 2];

        //TriangleData* triData = mesh_->triangles[i];

        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[index0];
        uv[1] = meshData_->uvBuffer_[index1];
        uv[2] = meshData_->uvBuffer_[index2];

        RGBA colors[3];
        colors[0].Set(meshData_->normalBuffer_[index0]);
        colors[1].Set(meshData_->normalBuffer_[index1]);
        colors[2].Set(meshData_->normalBuffer_[index2]);

        colors[0] += 1.0f;
        colors[1] += 1.0f;
        colors[2] += 1.0f;
        colors[0] *= 0.5f;
        colors[1] *= 0.5f;
        colors[2] *= 0.5f;

        RasterizeTriangle(&rasterData, uv, colors);
    }
    PadEdges(&rasterData, 6);
    return ret;
}

FilterableBlockMap<RGBA>* ObjectSpacePositionBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    ret->fill(RGBA::Clear);
    DECL_RASTER;
    
    Vec3 scaleBoundsBy = meshData_->CalculateBounds().Size().xyz();
    scaleBoundsBy.x = 1.0f / scaleBoundsBy.x;
    scaleBoundsBy.y = 1.0f / scaleBoundsBy.y;
    scaleBoundsBy.z = 1.0f / scaleBoundsBy.z;

    Vec3 offsetBy(0.5f, 0.5f, 0.5f);

    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        const unsigned index0 = meshData_->indexBuffer_[i];
        const unsigned index1 = meshData_->indexBuffer_[i+1];
        const unsigned index2 = meshData_->indexBuffer_[i+2];

        //TriangleData* triData = mesh_->triangles[i];

        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[index0];
        uv[1] = meshData_->uvBuffer_[index1];
        uv[2] = meshData_->uvBuffer_[index2];

        Vec3 p[3];
        p[0] = meshData_->positionBuffer_[index0];
        p[1] = meshData_->positionBuffer_[index1];
        p[2] = meshData_->positionBuffer_[index2];

        for (int i = 0; i < 3; ++i)
            p[i] = (p[i] * scaleBoundsBy) + offsetBy;

        RGBA colors[3];
        colors[0].Set(p[0]);
        colors[1].Set(p[1]);
        colors[2].Set(p[2]);

        RasterizeTriangle(&rasterData, uv, colors);
    }

    PadEdges(&rasterData);
    return ret;
}

FilterableBlockMap<RGBA>* ObjectSpaceGradientBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    ret->fill(RGBA::Clear);
    DECL_RASTER;

    auto bounds = meshData_->CalculateBounds();
    Vec3 scaleBoundsBy = bounds.Size().xyz();
    scaleBoundsBy.x = 1.0f / scaleBoundsBy.x;
    scaleBoundsBy.y = 1.0f / scaleBoundsBy.y;
    scaleBoundsBy.z = 1.0f / scaleBoundsBy.z;

    Vec3 offsetBy(0.5f, 0.5f, 0.5f);

    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        const unsigned index0 = meshData_->indexBuffer_[i];
        const unsigned index1 = meshData_->indexBuffer_[i + 1];
        const unsigned index2 = meshData_->indexBuffer_[i + 2];

        //TriangleData* triData = mesh_->triangles[i];

        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[index0];
        uv[1] = meshData_->uvBuffer_[index1];
        uv[2] = meshData_->uvBuffer_[index2];

        Vec3 p[3];
        p[0] = meshData_->positionBuffer_[index0];
        p[1] = meshData_->positionBuffer_[index1];
        p[2] = meshData_->positionBuffer_[index2];

        for (int i = 0; i < 3; ++i)
            p[i] = NORMALIZE(p[i], bounds.minPoint, bounds.maxPoint);

        RGBA colors[3];
        colors[0].Set(p[0]);
        colors[1].Set(p[1]);
        colors[2].Set(p[2]);

        RasterizeTriangle(&rasterData, uv, colors);
    }

    PadEdges(&rasterData);
    return ret;
}

FilterableBlockMap<RGBA>* VertexColorBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    DECL_RASTER;

    if (meshData_->colorBuffer_.size() == 0)
        return ret;

    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        const unsigned index0 = meshData_->indexBuffer_[i];
        const unsigned index1 = meshData_->indexBuffer_[i + 1];
        const unsigned index2 = meshData_->indexBuffer_[i + 2];

        //TriangleData* triData = mesh_->triangles[i];

        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[index0];
        uv[1] = meshData_->uvBuffer_[index1];
        uv[2] = meshData_->uvBuffer_[index2];

        RGBA p[3];
        p[0] = meshData_->colorBuffer_[index0];
        p[1] = meshData_->colorBuffer_[index1];
        p[2] = meshData_->colorBuffer_[index2];

        RasterizeTriangle(&rasterData, uv, p);
    }
    PadEdges(&rasterData);
    return ret;
}

FilterableBlockMap<RGBA>* FacetBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    DECL_RASTER;

    if (!meshData_)
        return ret;

    RGBA colors[3];
    if (invert_)
    {
        colors[0].Set(1, 1, 1);
        colors[1].Set(1, 1, 1);
        colors[2].Set(1, 1, 1);
    }
    else
    {
        colors[0].Set(0, 0, 0);
        colors[1].Set(0, 0, 0);
        colors[2].Set(0, 0, 0);
    }

    const RGBA color = invert_ ? RGBA(0, 0, 0) : RGBA(1, 1, 1);
    
    // Fill all of our triangles as black (or white if inverted)
    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        const unsigned index0 = meshData_->indexBuffer_[i];
        const unsigned index1 = meshData_->indexBuffer_[i + 1];
        const unsigned index2 = meshData_->indexBuffer_[i + 2];

        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[index0];
        uv[1] = meshData_->uvBuffer_[index1];
        uv[2] = meshData_->uvBuffer_[index2];

        RasterizeTriangle(&rasterData, uv, colors);
    }

    // Fill edges exceeding the threshold as white
    if (nv::HalfEdge::Mesh* mesh = meshData_->BuildHalfEdgeMesh())
    {
        for (auto faces = mesh->faces(); !faces.isDone(); faces.advance())
        {
            auto face = faces.current();
            Vec3 faceNormal = face->normal();
            const unsigned startIndices = face->id * 3;
            
            unsigned indices[] = {
                meshData_->indexBuffer_[startIndices],
                meshData_->indexBuffer_[startIndices + 1],
                meshData_->indexBuffer_[startIndices + 2],
            };

            Vec2 uv[] = {
                meshData_->uvBuffer_[indices[0]],
                meshData_->uvBuffer_[indices[1]],
                meshData_->uvBuffer_[indices[2]],
                meshData_->uvBuffer_[indices[0]]
            };
            
            for (unsigned i = 0; i < face->edgeCount(); ++i)
            {
                auto edge = face->edgeAt(i);
                if (edge->pair && edge->pair->face)
                {
                    const Vec3 otherNor = edge->pair->face->normal();
                    if (allEdges_ || fabsf(faceNormal.Dot(otherNor)) <= angleThreshold_)
                        RasterizerDrawLine(&rasterData, color, uv[i], uv[i + 1]);
                }
            }
        }

        delete mesh;
    }

    PadEdges(&rasterData);
    return ret;
}

FilterableBlockMap<RGBA>* PlanarProjectionBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    DECL_RASTER;

    return ret;
}

static Vec3 ProjectionAxes[] = {
    Vec3::PositiveY,
    Vec3::PositiveX,
    Vec3::PositiveZ
};



FilterableBlockMap<RGBA>* TriPlanarProjectionBaker::Bake() const
{
    FilterableBlockMap<RGBA>* ret = new FilterableBlockMap<RGBA>(GetWidth(), GetHeight());
    DECL_RASTER;

    if (meshData_->uvBuffer_.empty() || meshData_->normalBuffer_.empty())
    {
        SPRUE_LOG_ERROR("Triplanar texture baking requires UV coordinates and vertex normals");
        return ret;
    }

    auto bounds = meshData_->CalculateBounds();
    for (unsigned i = 0; i < meshData_->indexBuffer_.size(); i += 3)
    {
        const unsigned index0 = meshData_->indexBuffer_[i];
        const unsigned index1 = meshData_->indexBuffer_[i + 1];
        const unsigned index2 = meshData_->indexBuffer_[i + 2];

        //TriangleData* triData = mesh_->triangles[i];

        float projectionWeights[] = { 
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f 
        };

        Vec2 uv[3];
        uv[0] = meshData_->uvBuffer_[index0];
        uv[1] = meshData_->uvBuffer_[index1];
        uv[2] = meshData_->uvBuffer_[index2];
    
        Vec3 p[3];
        p[0] = meshData_->positionBuffer_[index0];
        p[1] = meshData_->positionBuffer_[index1];
        p[2] = meshData_->positionBuffer_[index2];

        Vec3 n[3];
        n[0] = meshData_->normalBuffer_[index0];
        n[1] = meshData_->normalBuffer_[index1];
        n[2] = meshData_->normalBuffer_[index2];

        RasterizeTextured(bounds, &rasterData, uv, n, p);
    }


    PadEdges(&rasterData);
    return ret;
}

void TriPlanarProjectionBaker::RasterizeTextured(const BoundingBox& objectBounds, RasterizerData* rasterData, Vec2* uv, Vec3* norms, Vec3* pos) const
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
            };

            // If any corner is overlapped then we pass for rendering under conservative rasterization
            bool anyCornersContained = false;
            for (unsigned i = 0; i < 4; ++i)
                anyCornersContained |= IsPointContained(uv, pixelCorners[i].x, pixelCorners[i].y);
            
            if (anyCornersContained) //if (IsPointContained(uv, xCoord, yCoord))
            {
                Vec3 interpolatedNormal = AttributeToWorldSpace(uv, pt, norms);
                //interpolatedNormal.x = fabsf(interpolatedNormal.x);
                //interpolatedNormal.y = fabsf(interpolatedNormal.y);
                //interpolatedNormal.z = fabsf(interpolatedNormal.z);
                //interpolatedNormal.Normalize();
                interpolatedNormal = interpolatedNormal.Abs();
                interpolatedNormal.Normalize();
                float weightSum = interpolatedNormal.x + interpolatedNormal.y + interpolatedNormal.z;
                interpolatedNormal /= weightSum;

                Vec3 interpolatedPosition = AttributeToWorldSpace(uv, pt, pos);
                //interpolatedPosition = objectBounds.TopRelativeNormalized(interpolatedPosition);

                Vec2 coord1 = Vec2(interpolatedPosition.y, interpolatedPosition.z) * scaling_.x;
                Vec2 coord2 = Vec2(interpolatedPosition.x, interpolatedPosition.z) * scaling_.y;
                Vec2 coord3 = Vec2(interpolatedPosition.x, interpolatedPosition.y) * scaling_.z;

                RGBA col1 = yTexture_->getBilinear(coord1.x / scaling_.x, coord1.y / scaling_.y);
                RGBA col2 = xTexture_->getBilinear(coord2.x / scaling_.x, coord2.y / scaling_.y);
                RGBA col3 = zTexture_->getBilinear(coord3.x / scaling_.x, coord3.y / scaling_.y);

                RGBA writeColor = col1 * interpolatedNormal.x + col2 * interpolatedNormal.y + col3 * interpolatedNormal.z;
                writeColor.a = 1.0f;
                //Color writeColor = col1*weights[0] + col2*weights[1] + col3*weights[2];

                const int writeIndex = ToArrayIndex(x, y, 0, rasterData->Width, rasterData->Height, rasterData->Depth);
                // If there's a mask than grab the appropriate mask pixel and multiply it with the write target
                if (rasterData->Mask)
                {
                    float maskWidthFraction = (float)rasterData->MaskWidth / (float)rasterData->Width;
                    float maskHeightFraction = (float)rasterData->MaskHeight / (float)rasterData->Height;
                    int maskIndex = CLAMP((int)(x*maskWidthFraction), 0, rasterData->MaskWidth - 1) + CLAMP((int)(y*maskHeightFraction), 0, rasterData->MaskHeight - 1)*rasterData->MaskWidth;
                    rasterData->Pixels[writeIndex] = writeColor*rasterData->Mask[maskIndex];
                }
                else
                {
                    rasterData->Pixels[writeIndex] = writeColor;
                }

                if (rasterData->WrittenMask)
                    rasterData->WrittenMask[writeIndex] = true;
            }
        }
    }
}

}