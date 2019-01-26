#pragma once

#include "../../GlobalAccess.h"
#include "../../SprueKitEditor.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Controls/Ribbon/RibbonPage.h>

namespace SprueEditor
{

    /// Baseclass for a ribbon page that is only visible when a document is not active.
    class RibbonNoDocumentPage : public RibbonPage
    {
        Q_OBJECT
    public:
        /// Construct and bind.
        RibbonNoDocumentPage(const QString& pageName) :
            RibbonPage(pageName)
        {
            connect(Global_DocumentManager(), &DocumentManager::ActiveDocumentChanged, this, &RibbonNoDocumentPage::ActiveDocumentChanged);
        }

        private slots:
            /// Set activity based on whether there is a new document or not.
            void ActiveDocumentChanged(DocumentBase* newDocument, DocumentBase* oldDocument) {
                if (newDocument)
                    SetActive(false);
                else
                    SetActive(true);
            }
    };

}