#include "UrhoMainMenus.h"

#include "../../../SprueKitEditor.h"

#include <EditorLib/Commands/RegisteredAction.h>

#include <QApplication>
#include <QMenu>

namespace UrhoEditor
{

    void UrhoMainMenus::Build(SprueEditor::SprueKitEditor* editor)
    {
        QMenu* menu = editor->GetOrCreateMenu("FILE_MENU", "&File");

        RegisteredAction* newAct = new RegisteredAction("File", "&New Scene");
        menu->addAction(newAct);

        RegisteredAction* openAct = new RegisteredAction("File", "&Open");
        menu->addAction(openAct);

        RegisteredAction* closeAct = new RegisteredAction("File", "Close");
        menu->addAction(closeAct);

        menu->addSeparator();

        RegisteredAction* saveAct = new RegisteredAction("File", "&Save");
        menu->addAction(saveAct);

        RegisteredAction* saveAsAct = new RegisteredAction("File", "Save As...");
        menu->addAction(saveAsAct);

        menu->addSeparator();

        QMenu* importMenu = menu->addMenu("&Import");
        
        /// Imports an XML/binary prefab file
        RegisteredAction* importPrefab = new RegisteredAction("File", "Prefab");
        importMenu->addAction(importPrefab);

        /// Will run the Model converter to convert the model first
        RegisteredAction* importModel = new RegisteredAction("File", "Model");
        importMenu->addAction(importModel);

        QMenu* exportMenu = menu->addMenu("&Export");
        RegisteredAction* exportAct = new RegisteredAction("File", "OBJ File");
        exportMenu->addAction(exportAct);

        menu->addSeparator();

        RegisteredAction* exitAct = new RegisteredAction("File", "Exit");
        menu->addAction(exitAct);

    // Connect the actions together

        connect(newAct, &QAction::triggered, [=](bool) {
            //TODO
        });

        connect(closeAct, &QAction::triggered, [=](bool) {
            //TODO
        });
        
        connect(saveAct, &QAction::triggered, [=](bool) {
            //TODO
        });

        connect(saveAsAct, &QAction::triggered, [=](bool) {
            //TODO
        });

        connect(importPrefab, &QAction::triggered, [=](bool) {
            //TODO
        });

        connect(importModel, &QAction::triggered, [=](bool) {
            //TODO
        });

        connect(exportAct, &QAction::triggered, [=](bool) {
            //TODO
        });

        connect(closeAct, &QAction::triggered, [=](bool) {
            QApplication::exit();
        });
    }

}