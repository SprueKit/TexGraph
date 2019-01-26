#pragma once

#include "../GuiBuilder.h"

namespace SprueEditor
{

    /// Constructs a ribbon page for "GUI Layout" that includes pane-layout save/restore as well ad Dock visiblity editing.
    class CoreGUIRibbonBuilder : public GuiBuilder
    {
    public:
        virtual void Build(SprueKitEditor* editor) override;
    };

}