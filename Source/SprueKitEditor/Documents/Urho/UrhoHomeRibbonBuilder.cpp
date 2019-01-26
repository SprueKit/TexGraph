#include "UrhoHomeRibbonBuilder.h"

#include "../../SprueKitEditor.h"
#include "../../InternalNames.h"

#include <EditorLib/Controls/Ribbon/Ribbon.h>
#include <EditorLib/Controls/Ribbon/RibbonButton.h>
#include <EditorLib/Controls/Ribbon/RibbonGallery.h>
#include <EditorLib/Controls/Ribbon/RibbonPage.h>
#include <EditorLib/Controls/Ribbon/RibbonSection.h>

#include "BaseUrhoDocument.h"
#include "../GuiBuilder/Ribbons/RibbonDocumentPage.h"

#include <EditorLib/Commands/RegisteredAction.h>

#include <QAction>

using namespace SprueEditor;

namespace UrhoEditor
{

    void UrhoHomeRibbonBuilder::Build(SprueKitEditor* editor)
    {

        auto ribbon = editor->GetRibbon();

        auto page = new RibbonDocumentPage(new RibbonDocumentPageType<BaseUrhoDocument>(), "Scene");
        page->SetActive(false);
        ribbon->AddPage(page);

        // Clipboard
        {
            auto section = new RibbonSection("Clipboard");
            page->AddSection(section);

            auto pasteBtn = new RibbonButton(RegisteredAction::GetAction("Paste"));
            auto cutBtn = new RibbonButton(RegisteredAction::GetAction("Cut"));
            cutBtn->MakeMedium();
            auto copyBtn = new RibbonButton(RegisteredAction::GetAction("Copy"));
            copyBtn->MakeMedium();

            section->AddWidgets(pasteBtn);
            section->AddWidgets(cutBtn, copyBtn);
        }

        // Edit
        {
            auto section = new RibbonSection("Edit");
            page->AddSection(section);

            auto selectBtn = new RibbonButton(RegisteredAction::GetAction("Select"));
            auto moveBtn = new RibbonButton(RegisteredAction::GetAction("Move"));
            auto rotateBtn = new RibbonButton(RegisteredAction::GetAction("Rotate"));
            auto scaleBtn = new RibbonButton(RegisteredAction::GetAction("Scale"));
            auto axialBtn = new RibbonButton(RegisteredAction::GetAction("Axial"));
            auto localBtn = new RibbonButton(RegisteredAction::GetAction("Local Space"));

            section->GetContentLayout()->addWidget(selectBtn);
            section->GetContentLayout()->addWidget(moveBtn);
            section->GetContentLayout()->addWidget(rotateBtn);
            section->GetContentLayout()->addWidget(scaleBtn);
            section->GetContentLayout()->addWidget(axialBtn);
            section->GetContentLayout()->addWidget(localBtn);

            auto snapPos = new RibbonButton(RegisteredAction::GetAction("Snap Position"));
            snapPos->MakeMedium();
            auto snapRot = new RibbonButton(RegisteredAction::GetAction("Snap Rotation"));
            snapRot->MakeMedium();
            section->AddWidgets(snapPos, snapRot);
        }
    }

}