#pragma once

#include "../../GuiBuilder.h"

#include <QActionGroup>

namespace UrhoEditor
{

    class UrhoSelectionModeMenu : public SprueEditor::GuiBuilder
    {
    public:
        virtual void Build(SprueEditor::SprueKitEditor* editor);

    private:
        QActionGroup* group_;
    };

}