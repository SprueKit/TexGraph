#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEngine
{

    class NormalMapDeviation : public PreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(NormalMapDeviation);
    };

    class NormalMapNormalize : public PreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(NormalMapNormalize);
    };

    /// Since this is per sample, how the hell would it work
    class SPRUE NormalMapTextureModifier : public SelfPreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(NormalMapTextureModifier);
        float StepSize = 1.0f;
        float Power = 1.0f;
        virtual bool WillForceExecute() const override { return true; }
    };
}