#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEngine
{

    class SPRUE PBRAlbedoEnforcerNode : public PreviewableNode
    {
    public:
        IMPL_TEXTURE_NODE(PBRAlbedoEnforcerNode);
        bool AlertMode = false;
        bool StrictMode = false;
    };


}