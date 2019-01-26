#include "ViewMenuBuilder.h"

#include "../../SprueKitEditor.h"

#include <EditorLib/Dialogs/ConfigDlg.h>
#include <EditorLib/Commands/RegisteredAction.h>

namespace SprueEditor
{

    void ViewMenuBuilder::Build(SprueKitEditor* editor)
    {
        auto configAction = new RegisteredAction("Edit", QIcon(":/Images/godot/icon_tools.png"), "Settings");
        auto menu = editor->GetOrCreateMenu("view", "&View");
        menu->addAction(configAction);

        connect(configAction, &QAction::triggered, [=](bool) {
            ConfigDlg::Show(editor->GetInstance()->GetSettings());
        });

        auto manualAction = new RegisteredAction("Help", QIcon(":/Images/godot/icon_help.png"), "Manual");
        menu->addAction(manualAction);

        connect(manualAction, &QAction::triggered, [=](bool) {
            //TODO
        });
    }

}