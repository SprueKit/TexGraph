#pragma once

#include <SprueEngine/Texturing/TextureBakers.h>

#include <vector>

namespace SprueEngine
{

    class SprueModel;
    class TexturingComponent;

    struct RasterizerData;

    class SprueTextureBaker : public TextureBaker
    {
        BASECLASSDEF(SprueTextureBaker, TextureBaker);
    public:
        SprueTextureBaker(SprueModel* model, const std::vector<TexturingComponent*>& components);
        virtual ~SprueTextureBaker();

        virtual FilterableBlockMap<RGBA>* Bake() const override;

    protected:
        void RasterizeTextured(RasterizerData* rasterData, Vec2* uv, Vec3* norms, Vec3* pos) const;

        std::vector<TexturingComponent*> texturingComponents_;
        SprueModel* model_;
    };

}