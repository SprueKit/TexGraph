#pragma once

#include "../GuiBuilder.h"

namespace SprueEditor
{

    class EditMenuBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor*) override;
    };

}