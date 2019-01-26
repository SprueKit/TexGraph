#pragma once

#include "../GuiBuilder.h"

namespace SprueEditor
{

    class SharedDockBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor*) override;
    };

}