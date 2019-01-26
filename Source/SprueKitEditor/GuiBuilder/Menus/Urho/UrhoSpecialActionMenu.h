#pragma once

#include "../../GuiBuilder.h"

namespace UrhoEditor
{

    class UrhoSpecialActionMenu : public SprueEditor::GuiBuilder
    {
    public:
        virtual void Build(SprueEditor::SprueKitEditor* editor) override;
    };

}