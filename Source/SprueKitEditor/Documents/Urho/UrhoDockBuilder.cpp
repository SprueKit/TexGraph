#include "UrhoDockBuilder.h"

#include "../../GlobalAccess.h"
#include "../../SprueKitEditor.h"
#include "../../Views/ViewManager.h"
#include "../../Views/RenderWidget.h"
#include "../../InternalNames.h"
#include "../../Localization/LocalizedWidgets.h"

#include "../../Data/UrhoDataSources.h"

#include "Controls/UrhoProfilerView.h"
#include "Controls/UrhoResourceCache.h"
#include "Controls/UrhoIMPropertyEditor.h"
#include "Controls/UrhoSceneTree.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Controls/Document/DocumentViewStackedWidget.h>

#include <QDockWidget>

namespace SprueEditor
{

    void UrhoDockBuilder::Build(SprueKitEditor* editor)
    {
    // Urho3D tree
        QDockWidget* urhoSceneDock = editor->GetOrCreateDock(URHO_SCENE_TREE_PANEL, "Urho3D Scene Tree", "ALT+U");
        UrhoEditor::UrhoSceneTree* tree = new UrhoEditor::UrhoSceneTree();
        tree->SetSelectron(editor->GetObjectSelectron());
        tree->setObjectName(URHO_SCENE_TREE_CONTROL);
        urhoSceneDock->setWidget(tree);
        
    // Profiler
        QDockWidget* urhoProfilerDock = editor->GetOrCreateDock(URHO_PROFILER_PANEL, "Profiler", "");
        UrhoEditor::UrhoProfilerView* profView = new UrhoEditor::UrhoProfilerView(Global_RenderWidget()->GetUrhoContext());
        profView->setObjectName(URHO_PROFILER_CONTROL);
        urhoProfilerDock->setWidget(profView);
        editor->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, urhoProfilerDock);

    // Resource Cache
        QDockWidget* urhoCacheDock = editor->GetOrCreateDock(URHO_RESOURCE_CACHE_PANEL, "Resource Cache", "");
        UrhoEditor::UrhoResourceCache* cacheView = new UrhoEditor::UrhoResourceCache(Global_RenderWidget()->GetUrhoContext());
        cacheView->setObjectName(URHO_RESOURCE_CACHE_CONTROL);
        urhoCacheDock->setWidget(cacheView);
        editor->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, urhoCacheDock);

    // Include properties page
        DocumentViewStackedWidget* propsSwitcher = (DocumentViewStackedWidget*)editor->GetDock(PROPERTY_PANEL)->widget();
        UrhoEditor::UrhoIMPropertyEditor* urhoIMProperties = new UrhoEditor::UrhoIMPropertyEditor();
        urhoIMProperties->SetSelectron(editor->GetObjectSelectron());
        propsSwitcher->AddWidget(new DocumentViewStackedWidget::DataSourceTypeChecker<UrhoEditor::SerializableDataSource > (), urhoIMProperties);
    }

}