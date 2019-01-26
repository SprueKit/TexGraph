#include "TexGenDockBuilder.h"

#include "../../SprueKitEditor.h"
#include "../../InternalNames.h"
#include "../../Localization/LocalizedWidgets.h"

#include "Controls/NodeList.h"
#include "Controls/TextureGraphControl.h"
#include "Controls/TextureInspector.h"

#include <QDockWidget>

namespace SprueEditor
{

    void TexGenDockBuilder::Build(SprueKitEditor* editor)
    {

    // Require a list of available nodes
        QDockWidget* dock = editor->GetOrCreateDock(TEXTURE_NODES_PANEL, "Texture Nodes", "Alt+N");
        NodeList* nl = new NodeList();
        nl->setObjectName(TEXTURE_NODES_CONTROL);
        dock->setWidget(nl);

        QDockWidget* graphDock = editor->GetOrCreateDock(TEXGRAPH_PANEL, "Texture Graph", "ALT+G");
        TextureGraphControl* texGraph = new TextureGraphControl();
        texGraph->setObjectName(TEXGRAPH_CONTROL);
        texGraph->SetSelectron(editor->GetObjectSelectron());
        graphDock->setWidget(texGraph);
        editor->GetDockingWindow()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, graphDock);

        QDockWidget* inspectorDock = editor->GetOrCreateDock(TEXINSPECT_INSPECTOR_PANEL, "Texture Inspector", "");
        TextureInspector* inspector = new TextureInspector();
        inspector->setObjectName(TEXINSPECT_INSPECTOR_CONTROL);
        inspector->SetSelectron(editor->GetObjectSelectron());
        inspectorDock->setWidget(inspector);
        editor->GetDockingWindow()->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, inspectorDock);
    }
}