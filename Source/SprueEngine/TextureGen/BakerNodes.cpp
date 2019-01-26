#include "BakerNodes.h"

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Texturing/TextureBakers.h>

namespace SprueEngine
{

    void TextureBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "TextureBakerNode");
        REGISTER_RESOURCE(TextureBakerNode, MeshResource, GetMeshResourceHandle, SetMeshResourceHandle, GetMeshData, SetMeshData, ResourceHandle("Mesh"), "Mesh", "", PS_VisualConsequence);
        REGISTER_PROPERTY_MEMORY(TextureBakerNode, unsigned, offsetof(TextureBakerNode, Width), 256, "Width", "Width of the generated image", PS_Default);
        REGISTER_PROPERTY_MEMORY(TextureBakerNode, unsigned, offsetof(TextureBakerNode, Height), 256, "Height", "Height of the generated image", PS_Default);
    }

    void AmbientOcclusionBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("TextureBakerNode", "AmbientOcclusionBakerNode");
        REGISTER_PROPERTY_MEMORY(AmbientOcclusionBakerNode, float, offsetof(AmbientOcclusionBakerNode, scalingFactor_), 1.0f, "AO Scaling Factor", "Adjusts the intensity of the AO shadowing", PS_TinyIncrement);
        REGISTER_PROPERTY_MEMORY(AmbientOcclusionBakerNode, float, offsetof(AmbientOcclusionBakerNode, darkLimit_), 0.15f, "Maximum Darkness", "Lowest allowed value to use in the occluded areas", PS_TinyIncrement);
    }

    void AmbientOcclusionBakerNode::Construct()
    {
        AddOutput("", TEXGRAPH_CHANNEL);
    }

    int AmbientOcclusionBakerNode::Execute(const Variant& param)
    {
        if (!Cache && meshData)
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));

            nv::HalfEdge::Mesh* mesh = meshData->GetMesh(0)->BuildHalfEdgeMesh();
            AmbientOcclusionBaker baker(mesh, meshData->GetMesh(0));
            baker.SetScalingFactor(scalingFactor_);
            baker.SetDarkLimit(darkLimit_);
            baker.SetHeight(Height);
            baker.SetWidth(Width);

            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            
            delete data;
            delete mesh;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            GetOutputSocket(0)->StoreValue(Cache->getBilinear(coord.x, coord.y));
        }
        return GRAPH_EXECUTE_COMPLETE;
    }

    void CurvatureBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("TextureBakerNode", "CurvatureBakerNode");
    }

    void CurvatureBakerNode::Construct()
    {
        AddOutput("", TEXGRAPH_CHANNEL);
        AddOutput("Concavity", TEXGRAPH_FLOAT);
        AddOutput("Convexity", TEXGRAPH_FLOAT);
    }

    int CurvatureBakerNode::Execute(const Variant& param)
    {
        if (!Cache && meshData)
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));

            nv::HalfEdge::Mesh* mesh = meshData->GetMesh(0)->BuildHalfEdgeMesh();
            CurvatureBaker baker(mesh, meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            delete mesh;
            delete data;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            auto value = Cache->getBilinear(coord.x, coord.y);
            GetOutputSocket(0)->StoreValue(value);
            GetOutputSocket(1)->StoreValue(value.r);
            GetOutputSocket(2)->StoreValue(value.g);
        }
        return GRAPH_EXECUTE_COMPLETE;
    }

    void ObjectSpacePositionBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("TextureBakerNode", "ObjectSpacePositionBakerNode");
    }

    void ObjectSpacePositionBakerNode::Construct()
    {
        AddOutput("", TEXGRAPH_CHANNEL);
    }

    int ObjectSpacePositionBakerNode::Execute(const Variant& param)
    {
        if (!Cache && meshData)
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));

            ObjectSpacePositionBaker baker(0x0, meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            delete data;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            GetOutputSocket(0)->StoreValue(Cache->getBilinear(coord.x, coord.y));
        }
        return GRAPH_EXECUTE_COMPLETE;
    }

    void ObjectSpaceNormalBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("TextureBakerNode", "ObjectSpaceNormalBakerNode");
    }

    void ObjectSpaceNormalBakerNode::Construct()
    {
        AddOutput("", TEXGRAPH_CHANNEL);
    }

    int ObjectSpaceNormalBakerNode::Execute(const Variant& param)
    {
        if (!Cache && meshData)
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));

            ObjectSpaceNormalBaker baker(0x0, meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            
            delete data;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            GetOutputSocket(0)->StoreValue(Cache->getBilinear(coord.x, coord.y));
        }
        return GRAPH_EXECUTE_COMPLETE;
    }

    void ObjectSpaceGradientBakerNode::Register(Context* context)
    {
        COPY_PROPERTIES(TextureBakerNode, ObjectSpaceGradientBakerNode);
    }

    void ObjectSpaceGradientBakerNode::Construct()
    {
        AddOutput("", TEXGRAPH_RGBA);
    }

    int ObjectSpaceGradientBakerNode::Execute(const Variant& param)
    {
        if (!Cache && meshData)
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));

            ObjectSpaceGradientBaker baker(0x0, meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            delete data;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            GetOutputSocket(0)->StoreValue(Cache->getBilinear(coord.x, coord.y));
        }
        return GRAPH_EXECUTE_COMPLETE;
    }

    void VertexColorBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("TextureBakerNode", "VertexColorBakerNode");
    }

    void VertexColorBakerNode::Construct()
    {
        AddOutput("", TEXGRAPH_CHANNEL);
    }

    int VertexColorBakerNode::Execute(const Variant& param)
    {
        if (!Cache && meshData)
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));

            VertexColorBaker baker(0x0, meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            delete data;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            GetOutputSocket(0)->StoreValue(Cache->getBilinear(coord.x, coord.y));
        }
        return GRAPH_EXECUTE_COMPLETE;
    }

    void FacetBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("TextureBakerNode", "FacetBakerNode");
        REGISTER_PROPERTY_MEMORY(FacetBakerNode, float, offsetof(FacetBakerNode, threshold_), 0.5f, "Threshold", "Edges whose curvature exceeds this value will be drawn", PS_TinyIncrement);
        REGISTER_PROPERTY_MEMORY(FacetBakerNode, bool, offsetof(FacetBakerNode, invert_), false, "Invert", "Use black for lines and white inside of triangles", PS_Default);
        REGISTER_PROPERTY_MEMORY(FacetBakerNode, bool, offsetof(FacetBakerNode, allEdges_), false, "Draw All Edges", "Draw lines for all edges instead of just those exceeding the threshold", PS_Default);
    }

    void FacetBakerNode::Construct()
    {
        AddOutput("", TEXGRAPH_CHANNEL);
    }

    int FacetBakerNode::Execute(const Variant& param)
    {
        if (!Cache && meshData)
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));

            FacetBaker baker(meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            baker.SetAngleThreshold(threshold_);
            baker.SetInvert(invert_);
            baker.SetAllEdges(allEdges_);
            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            delete data;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            GetOutputSocket(0)->StoreValue(Cache->getBilinear(coord.x, coord.y));
        }
        return GRAPH_EXECUTE_COMPLETE;
    }

    void DominantPlaneBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("TextureBakerNode", "DominantPlaneBakerNode");
    }

    void DominantPlaneBakerNode::Construct()
    {
        AddOutput("", TEXGRAPH_CHANNEL);
    }

    int DominantPlaneBakerNode::Execute(const Variant& param)
    {
        if (meshData && (!Cache || (Cache->getWidth() != Width || Cache->getHeight() != Height)))
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));

            DominantPlaneBaker baker(meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            delete data;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            GetOutputSocket(0)->StoreValue(Cache->getBilinear(coord.x, coord.y));
        }
        return GRAPH_EXECUTE_COMPLETE;
    }

    void TriplanarBakerNode::Register(Context* context)
    {
        context->CopyBaseProperties("TextureBakerNode", "TriplanarBakerNode");
        REGISTER_RESOURCE(TriplanarBakerNode, BitmapResource, GetBitmapResourceHandle, SetBitmapResourceHandle, GetImageData, SetImageData, ResourceHandle("Image"), "Bitmap", "Bitmap that will be projected onto the mesh", PS_VisualConsequence);
        REGISTER_PROPERTY_MEMORY(TriplanarBakerNode, float, offsetof(TriplanarBakerNode, scale_), 1.0f, "Scale", "Scaling factor for the texture to place on the mesh", PS_VisualConsequence | PS_TinyIncrement);
    }

    void TriplanarBakerNode::Construct()
    {
        AddInput("Sample", TEXGRAPH_CHANNEL);
        AddOutput("", TEXGRAPH_CHANNEL);
    }

    void TriplanarBakerNode::VersionUpdate(unsigned fromVersion)
    {
        if (fromVersion < 2)
            InsertInput(0, "RGB", TEXGRAPH_CHANNEL);
    }

    int TriplanarBakerNode::Execute(const Variant& param)
    {
        if (meshData && imageData_ && (!Cache || (Cache->getWidth() != Width || Cache->getHeight() != Height)))
        {
            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));
            TriPlanarProjectionBaker baker(0x0, meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            baker.SetScaling(Vec3(scale_, scale_, scale_));
            baker.SetTextureX(imageData_->GetImage());
            baker.SetTextureY(imageData_->GetImage());
            baker.SetTextureZ(imageData_->GetImage());

            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            delete data;
        }
        else if (meshData && GetInputSocket(0)->HasConnections() && !imageData_ && (!Cache || Cache->getWidth() != Width || Cache->getHeight() != Height))
        {
            std::unique_ptr<FilterableBlockMap<RGBA>> map(new FilterableBlockMap<RGBA>(Width, Height));
            Vec4 pos(0, 0, Width, Height);
            for (unsigned y = 0; y < Height; ++y)
            {
                pos.y = ((float)y) / Height;
                for (unsigned x = 0; x < Width; ++x)
                {
                    pos.z = ((float)x) / Width;
                    ForceExecuteUpstreamOnly(pos);
                    map->set(GetInputSocket(0)->GetValue().getColorSafe(true), x, y);
                }
            }

            Cache.reset(new FilterableBlockMap<RGBA>(Width, Height));
            TriPlanarProjectionBaker baker(0x0, meshData->GetMesh(0));
            baker.SetHeight(Height);
            baker.SetWidth(Width);
            baker.SetScaling(Vec3(scale_, scale_, scale_));
            baker.SetTextureX(map.get());
            baker.SetTextureY(map.get());
            baker.SetTextureZ(map.get());

            FilterableBlockMap<RGBA>* data = baker.Bake();
            memcpy(Cache->getData(), data->getData(), sizeof(RGBA) * Cache->getWidth() * Cache->getHeight());
            delete data;
        }

        if (Cache)
        {
            Vec2 coord = param.getVec2Safe();
            GetOutputSocket(0)->StoreValue(Cache->getBilinear(coord.x, coord.y));
        }
        return GRAPH_EXECUTE_COMPLETE;
    }
}