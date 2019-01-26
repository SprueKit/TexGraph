#pragma once

#include "../GuiBuilder.h"

namespace SprueEditor
{

    class ViewportMenuBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor*) override;
    };

}