#pragma once

#include "../../GuiBuilder/GuiBuilder.h"

namespace SprueEditor
{

    class UrhoDockBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor*);
    };

}