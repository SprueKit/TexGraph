#pragma once

#include "../../GuiBuilder/GuiBuilder.h"

namespace UrhoEditor
{

    class UrhoHomeRibbonBuilder : public SprueEditor::GuiBuilder
    {
    public:
        virtual void Build(SprueEditor::SprueKitEditor* editor) override;
    };

}