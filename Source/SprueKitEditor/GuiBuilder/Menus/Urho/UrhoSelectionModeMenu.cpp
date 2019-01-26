#include "UrhoSelectionModeMenu.h"

#include "../../../SprueKitEditor.h"

#include <EditorLib/Commands/RegisteredAction.h>

#include <QToolBar>

namespace UrhoEditor
{


    void UrhoSelectionModeMenu::Build(SprueEditor::SprueKitEditor* editor)
    {
        QAction* pickDrawables = new RegisteredAction("Picking Mode", "Pick Drawables");
        QAction* pickLights = new RegisteredAction("Picking Mode", "Pick Lights");
        QAction* pickPhysics = new RegisteredAction("Picking Mode", "Pick Physics");
        QAction* pickZones = new RegisteredAction("Picking Mode", "Pick Zones");
        QAction* pickUI = new RegisteredAction("Picking Mode", "Pick UI");

        QToolBar* bar = editor->GetOrCreateToolBar("URHO_SELECTION_PICKING", "Picking Mode");
        bar->addAction(pickDrawables);
        bar->addAction(pickLights);
        bar->addAction(pickPhysics);
        bar->addAction(pickZones);
        bar->addAction(pickUI);

        group_ = new QActionGroup(this);
        group_->addAction(pickDrawables);
        group_->addAction(pickLights);
        group_->addAction(pickPhysics);
        group_->addAction(pickZones);
        group_->addAction(pickUI);
    }

}