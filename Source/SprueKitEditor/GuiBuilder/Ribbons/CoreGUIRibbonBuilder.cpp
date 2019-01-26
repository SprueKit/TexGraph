#include "CoreGUIRibbonBuilder.h"

#include "../../SprueKitEditor.h"
#include "../../InternalNames.h"
#include "../../Localization/LocalizedWidgets.h"

#include <EditorLib/Commands/RegisteredAction.h>
#include <EditorLib/Controls/Ribbon/Ribbon.h>
#include <EditorLib/Controls/Ribbon/RibbonButton.h>
#include <EditorLib/Controls/Ribbon/RibbonGallery.h>
#include <EditorLib/Controls/Ribbon/RibbonPage.h>
#include <EditorLib/Controls/Ribbon/RibbonSection.h>

#include <QAction>
#include <QDockWidget>

namespace SprueEditor
{

    void CoreGUIRibbonBuilder::Build(SprueKitEditor* editor)
    {
        Ribbon* ribbon = editor->GetRibbon();

        RibbonPage* layoutPage = new RibbonPage("Layout");
        ribbon->AddPage(layoutPage);

        // Layout section
        {
            RibbonSection* layoutSection = new RibbonSection("Layout");
            layoutPage->AddSection(layoutSection);

            QVBoxLayout* layoutState = new QVBoxLayout();
            layoutSection->GetContentLayout()->addLayout(layoutState);
            
            QAction* saveLayoutAction = RegisteredAction::GetAction("Save Layout");
            RibbonButton* saveLayout = new RibbonButton(saveLayoutAction);
            saveLayout->MakeRegular();
            layoutState->addWidget(saveLayout);

            QAction* loadLayoutAction = RegisteredAction::GetAction("Restore Layout");
            RibbonButton* loadLayout = new RibbonButton(loadLayoutAction);
            loadLayout->MakeRegular();
            layoutState->addWidget(loadLayout);

            QGridLayout* subLayout = new QGridLayout();
            RibbonButton* dockTopLeft = new RibbonButton(RegisteredAction::GetAction("Toggle Dock Top Left Corner"));
            dockTopLeft->MakeMedium();
            RibbonButton* dockTopRight = new RibbonButton(RegisteredAction::GetAction("Toggle Dock Top Right Corner"));
            dockTopRight->MakeMedium();

            RibbonButton* dockBottomLeft = new RibbonButton(RegisteredAction::GetAction("Toggle Dock Bottom Left Corner"));
            dockBottomLeft->MakeMedium();
            RibbonButton* dockBottomRight = new RibbonButton(RegisteredAction::GetAction("Toggle Dock Bottom Right Corner"));
            dockBottomRight->MakeMedium();

            subLayout->addWidget(dockTopLeft, 0, 0);
            subLayout->addWidget(dockTopRight, 0, 1);
            subLayout->addWidget(dockBottomLeft, 1, 0);
            subLayout->addWidget(dockBottomRight, 1, 1);

            layoutSection->GetContentLayout()->addLayout(subLayout);
        }

        // Dock Panels section
        {
            RibbonSection* panelsSection = new RibbonSection("Dock Panels");
            layoutPage->AddSection(panelsSection);

            auto allDocks = editor->GetAllDocks();
            int currentCount = 0;
            QLayout* lastLayout = 0x0;
            for (auto currentDock = allDocks.begin(); currentDock != allDocks.end(); ++currentDock, ++currentCount)
            {
                if (lastLayout == 0x0 || currentCount == 2)
                {
                    lastLayout = new QVBoxLayout();
                    lastLayout->setMargin(0);
                    lastLayout->setAlignment(Qt::AlignTop);
                    panelsSection->GetContentLayout()->addLayout(lastLayout);
                    if (currentCount == 2)
                        currentCount = 0;
                }
                RibbonButton* btn = new RibbonButton(currentDock->second->toggleViewAction());
                btn->MakeMedium(true);
                lastLayout->addWidget(btn);
            }
        }
    }

}