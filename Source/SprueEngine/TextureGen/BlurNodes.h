#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>
#include <map>

namespace SprueEngine
{

    class SPRUE BlurModifier : public SelfPreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(BlurModifier);
        int BlurRadius = 3;
        float BlurStepSize = 1.0f;
        float Sigma = 1.5f;

        virtual bool WillForceExecute() const override { return true; }
        void CalculateKernel(std::vector<float>& target);
    };

    class SPRUE StreakModifier : public PreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(StreakModifier);
        float StreakAngle = 0.0f;
        float StreakLength = 1.0f;
        unsigned Samples = 3;
        bool FadeOff = true;
        virtual bool WillForceExecute() const override { return true; }
    };

    class SPRUE AnisotropicBlur : public BlurModifier
    {
    public:
        IMPL_TEXTURE_NODE(AnisotropicBlur);
    };

}