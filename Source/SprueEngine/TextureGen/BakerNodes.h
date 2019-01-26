#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEngine
{
    class SPRUE DependentTextureBakerNode : public PreviewableNode
    {

    };

    class SPRUE TextureBakerNode : public SelfPreviewableNode
    {
    public:
        static void Register(Context*);

        std::shared_ptr<FilterableBlockMap<RGBA> > Cache;

        ResourceHandle meshResourceHandle;
        std::shared_ptr<MeshResource> meshData;

        ResourceHandle GetMeshResourceHandle() const { return meshResourceHandle; }
        void SetMeshResourceHandle(const ResourceHandle& handle) { meshResourceHandle = handle; }
        
        std::shared_ptr<MeshResource> GetMeshData() const { return meshData; }
        void SetMeshData(const std::shared_ptr<MeshResource>& data) { meshData = data; }

        unsigned Width = 256;
        unsigned Height = 256;
    };


    class SPRUE AmbientOcclusionBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(AmbientOcclusionBakerNode);

        float scalingFactor_ = 1.0f;
        float darkLimit_ = 0.15f;
    };

    class SPRUE CurvatureBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(CurvatureBakerNode);
    };

    class SPRUE ObjectSpaceNormalBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(ObjectSpaceNormalBakerNode);
    };

    class SPRUE ObjectSpaceGradientBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(ObjectSpaceGradientBakerNode);
    };

    class SPRUE ObjectSpacePositionBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(ObjectSpacePositionBakerNode);
    };

    class SPRUE VertexColorBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(VertexColorBakerNode);
    };

    class SPRUE FacetBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(FacetBakerNode);

        float threshold_ = 0.5f;
        bool invert_ = false;
        bool allEdges_ = false;
    };

    class SPRUE DominantPlaneBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(DominantPlaneBakerNode);
    };

    /// Can work with other nodes to decide what to project into the UV space (most likely to be just a bitmap though)
    class SPRUE PlanarProjectionBakerNode : public DependentTextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(PlanarProjectionBakerNode);
    };

    /// Can work with other nodes to decide what to project (most likely to be just a bitmap though)
    class SPRUE TriplanarBakerNode : public TextureBakerNode
    {
    public:
        IMPL_TEXTURE_NODE(TriplanarBakerNode);

        virtual bool WillForceExecute() const override { return true; }

        ResourceHandle GetBitmapResourceHandle() const { return bitmapResourceHandle_; }
        void SetBitmapResourceHandle(const ResourceHandle& handle) { bitmapResourceHandle_ = handle; }

        std::shared_ptr<BitmapResource> GetImageData() const { return imageData_; }
        void SetImageData(const std::shared_ptr<BitmapResource>& img) { imageData_ = img; }

        std::shared_ptr<BitmapResource> imageData_;
        ResourceHandle bitmapResourceHandle_;

        float scale_ = 1.0f;

        virtual unsigned GetClassVersion() const override { return 2; }
        virtual void VersionUpdate(unsigned fromVersion) override;
    };
}