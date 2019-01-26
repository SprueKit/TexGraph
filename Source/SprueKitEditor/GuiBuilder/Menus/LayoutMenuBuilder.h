#pragma once

#include "../GuiBuilder.h"

namespace SprueEditor
{

    class LayoutMenuBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor* editor) override;
    };

}