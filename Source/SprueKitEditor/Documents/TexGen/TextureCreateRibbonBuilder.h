#pragma once

#include "../../GuiBuilder/GuiBuilder.h"

namespace SprueEditor
{

    class TextureCreateRibbonBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor*) override;
    };

}