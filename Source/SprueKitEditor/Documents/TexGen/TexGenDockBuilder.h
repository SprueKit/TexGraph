#pragma once

#include "../../GuiBuilder/GuiBuilder.h"

namespace SprueEditor
{

    class TexGenDockBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor*) override;
    };

}