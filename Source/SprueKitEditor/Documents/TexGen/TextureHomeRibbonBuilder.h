#pragma once

#include "../../GuiBuilder/GuiBuilder.h"

namespace SprueEditor
{

    class TextureHomeRibbonBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor* editor) override;
    };

}