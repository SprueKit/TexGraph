#include "UrhoSpecialActionMenu.h"

#include "../../../SprueKitEditor.h"

#include <EditorLib/Commands/RegisteredAction.h>

#include <QToolBar>

namespace UrhoEditor
{

    void UrhoSpecialActionMenu::Build(SprueEditor::SprueKitEditor* editor)
    {
        QAction* rebuildNavMesh = new RegisteredAction("Specialized", "Rebuild navigation mesh");
        QAction* renderZoneCubeMap = new RegisteredAction("Specialized", "Render zone cubemap");
        QAction* addChildrenToSMGroup = new RegisteredAction("Specialized", "Add children to SM-Group");
        QAction* setChildrenSplineCyclic = new RegisteredAction("Specialized", "Set children as spline path (cyclic)");
        QAction* setChildrenSplineNoCycle = new RegisteredAction("Specialized", "Set children as spline path (non-cyclic)");

        QAction* playInEditor = new RegisteredAction("Specialized", "Play in editor");

        auto toolbar = editor->GetOrCreateToolBar("URHO_TOOLBAR_BUILD", "Build Tools");
        toolbar->addAction(rebuildNavMesh);
        toolbar->addAction(renderZoneCubeMap);
        
        toolbar = editor->GetOrCreateToolBar("URHO_TOOLBAR_CHILD_ACTIONS", "Child Actions");
        toolbar->addAction(addChildrenToSMGroup);
        toolbar->addAction(setChildrenSplineCyclic);
        toolbar->addAction(setChildrenSplineNoCycle);
    }

}