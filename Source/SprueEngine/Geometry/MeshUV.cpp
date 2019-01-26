#include "MeshData.h"

#include <SprueEngine/UVMapping/UVAtlas.h>
#include <SprueEngine/UVMapping/Adjacency.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Logging.h>

HRESULT __cdecl UVAtlasCallback(float fPercentDone)
{
    return 0;
}

void SprueEngine::MeshData::ComputeUVCoordinates(unsigned width, unsigned height, int quality, int charts, float stretch, float gutter)
{
    std::vector<unsigned> adjacency;
    adjacency.resize(indexBuffer_.size());

    quality = CLAMP(quality, 0, 2);
    stretch = CLAMP(stretch, 0.0f, 1.0f);
    charts = CLAMP(charts, 0, 100);
    gutter = CLAMP(gutter, 0.0f, 50.0f);

    if (!FAILED(DirectX::GenerateAdjacencyAndPointReps(indexBuffer_.data(), indexBuffer_.size() / 3, (DirectX::XMFLOAT3*)positionBuffer_.data(), positionBuffer_.size(), EPSILON, 0, adjacency.data())))
    {
        std::vector<DirectX::UVAtlasVertex> outMesh;
        std::vector<unsigned char> outIndices;
        std::vector<unsigned> remappedVertices;

        if (!FAILED(DirectX::UVAtlasCreate(
            (DirectX::XMFLOAT3*)positionBuffer_.data() /*vertex positions*/, 
            positionBuffer_.size() /*position count*/, 
            indexBuffer_.data() /*indices*/, 
            DXGI_FORMAT_R32_UINT /*index type*/, 
            indexBuffer_.size() / 3 /*N Faces*/,
            charts /*max charts*/,
            stretch /*max stretch*/,
            width /*width*/,
            height /*height*/,
            2.0f /*gutter*/,
            adjacency.data() /*adjacency data*/,
            0x0 /*reps*/,
            0x0 /*IMT array*/,
            UVAtlasCallback /*callback*/,
            0.0f /*callback frequency*/,
            quality /* options */,
            outMesh /* output */,
            outIndices /* output indices */,
            0x0,
            &remappedVertices
            )))
        {
            // Remap vertex positions
            std::vector<Vec3> newPositions(outMesh.size());
            for (unsigned i = 0; i < outMesh.size() && i < remappedVertices.size(); ++i)
                newPositions[i] = positionBuffer_[remappedVertices[i]];
            positionBuffer_ = newPositions;
            
            // Remap Normals if present
            if (normalBuffer_.size())
            {
                std::vector<Vec3> newNormals(outMesh.size());
                for (unsigned i = 0; i < outMesh.size() && i < remappedVertices.size(); ++i)
                    newNormals[i] = normalBuffer_[remappedVertices[i]].Normalized();
                normalBuffer_ = newNormals;
            }

            // Remap Vertex colors if present
            if (colorBuffer_.size())
            {
                std::vector<RGBA> newColors(outMesh.size());
                for (unsigned i = 0; i < outMesh.size() && i < remappedVertices.size(); ++i)
                    newColors[i] = colorBuffer_[remappedVertices[i]];
                colorBuffer_ = newColors;
            }

            // Transfer the new UV coordinates to the mesh
            uvBuffer_.resize(outMesh.size());
            for (unsigned i = 0; i < outMesh.size(); ++i)
                uvBuffer_[i] = Vec2(outMesh[i].uv.x, outMesh[i].uv.y);

            indexBuffer_ = *reinterpret_cast<std::vector<uint32_t>*>(&outIndices);

            CalculateTangents();

            SPRUE_LOG_INFO("Finished generating UV coordinates");
        }
        else
        {
            SPRUE_LOG_ERROR("Failed to generate UV coordinates");
        }
    }
    else
    {
        SPRUE_LOG_ERROR("Failed to generate adjacency data for UV coordinate generation");
    }
}