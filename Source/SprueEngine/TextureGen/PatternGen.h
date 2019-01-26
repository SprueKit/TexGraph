#pragma once

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEngine
{

class SPRUE WeaveGenerator : public SelfPreviewableNode
{
public:
    IMPL_TEXTURE_NODE(WeaveGenerator);
    
    int Underrun = 1;
    int Overrun = 1;
    int Skip = 1;
    float WarpWidth = 0.8f;
    float WeftWidth = 0.8f;
    RGBA WarpColor = RGBA(1, 1, 1);
    RGBA WeftColor = RGBA(0, 0, 0);
    RGBA BaseColor = RGBA(0, 0, 0);
    IntVec2 Tiling = IntVec2(4, 4);
};

}