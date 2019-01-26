#include "SharedDockBuilder.h"

#include "../../GlobalAccess.h"
#include "../../Data/SprueDataSources.h"
#include "../../Data/UrhoDataSources.h"

#include "../../SprueKitEditor.h"
#include "../../InternalNames.h"
#include "../../Localization/LocalizedWidgets.h"
#include "../../Views/RenderWidget.h"
#include "../../Views/ViewManager.h"

#include "../../Panels/ResourcePanel.h"

#include <EditorLib/Controls/DataGridWidget.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Panels/DocumentsPanel.h>
#include <EditorLib/Controls/Document/DocumentViewStackedWidget.h>
#include <EditorLib/Panels/LogPanel.h>
#include <EditorLib/Controls/SearchResultList.h>
#include <EditorLib/Controls/UndoListWidget.h>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/FString.h>
#include <SprueEngine/IEditable.h>

#include <Urho3D/Scene/Component.h>

#include <QDockWidget>
#include <QUndoView>

#include <EditorLib/IM/IMWidget.h>

namespace SprueEditor
{

    unsigned PropertyObjectTypeHashHandler(std::shared_ptr<DataSource> object)
    {
        if (object != 0x0)
        {
            if (auto editable = std::dynamic_pointer_cast<IEditableDataSource>(object))
                return editable->GetEditable()->GetTypeHash();
            else 
                if (auto editable = std::dynamic_pointer_cast<UrhoEditor::SerializableDataSource>(object))
                    return SprueEngine::StringHash(editable->GetObject()->GetType().Value());
        }
        SprueEngine::StringHash hash;
        hash.value_ = 0;
        return hash;
    }

    void SharedDockBuilder::Build(SprueKitEditor* editor)
    {
        QDockWidget* logPanel = editor->GetOrCreateDock(LOG_PANEL, "Log", "ALT+L");
        LogPanel* log = new LogPanel();
        log->setObjectName(LOG_CONTROL);
        logPanel->setWidget(log);
        logPanel->setVisible(false);

        QDockWidget* documentsPanel = editor->GetOrCreateDock(DOCUMENTS_PANEL, "Documents", "ALT+D");
        DocumentsPanel* documents = new DocumentsPanel();
        documents->setObjectName(DOCUMENTS_CONTROL);
        documentsPanel->setWidget(documents);
        documentsPanel->setVisible(false);

    // Add the resource folders browser
        QDockWidget* resourcePanel = editor->GetOrCreateDock(RESOURCE_BROWSER_PANEL, "Resource Browser", "ALT+R");
        ResourcePanel* resources = new ResourcePanel();
        resources->setObjectName(RESOURCE_BROWSER_CONTROL);
        resourcePanel->setWidget(resources);
        editor->GetDockingWindow()->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, resourcePanel);
        resourcePanel->setVisible(false);

    // Add the multi-object datagrid
        QDockWidget* dataTable = editor->GetOrCreateDock(DATAGRID_PANEL, "Data Grid", "");
        DataGridWidget* grid = new DataGridWidget();

        grid->setObjectName(DATAGRID_CONTROL);
        dataTable->setWidget(grid);
        editor->GetDockingWindow()->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, dataTable);
        dataTable->setVisible(false);
        grid->SetSelectron(editor->GetObjectSelectron());

    // Common properties panel
        QDockWidget* propertiesDock = editor->GetOrCreateDock(PROPERTY_PANEL, "Properties", "ALT+P");
        DocumentViewStackedWidget* propStack = new DocumentViewStackedWidget();
        propertiesDock->setWidget(propStack);
        editor->GetDockingWindow()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, propertiesDock);

    // Add the history panel
        QDockWidget* historyPanel = editor->GetOrCreateDock(HISTORY_PANEL, "History", "ALT+H");
        UndoListWidget* undoList = new UndoListWidget();
        undoList->setObjectName(HISTORY_CONTROL);
        historyPanel->setWidget(undoList);

        editor->GetDockingWindow()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, historyPanel);
        historyPanel->setVisible(false);

        connect(Global_DocumentManager(), &DocumentManager::ActiveDocumentChanged, [=](DocumentBase* newDoc, DocumentBase* oldDoc) {
            if (newDoc)
                undoList->SetUndoStack(newDoc->GetUndoStack());
            else
                undoList->SetUndoStack(0x0);
        });

        QDockWidget* searchPanel = editor->GetOrCreateDock(SEARCH_RESULTS_PANEL, "Search Results", std::string());
        SearchResultList* searchList = new SearchResultList();
        searchPanel->setWidget(searchList);
        editor->GetDockingWindow()->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, searchPanel);
        searchPanel->setVisible(false);
    }

}