#pragma once

#include "../../GlobalAccess.h"
#include "../../SprueKitEditor.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Controls/Ribbon/RibbonPage.h>

namespace SprueEditor
{
    /// Baseclass for working around QObject types supporting templates.
    struct BaseRibbonDocumentPageType
    {
        virtual bool Valid(DocumentBase* doc) = 0;
    };

    /// Template implementation for QObject no template<> workaround.
    template<class T>
    struct RibbonDocumentPageType : BaseRibbonDocumentPageType
    {
        virtual bool Valid(DocumentBase* doc) override { return dynamic_cast<T*>(doc) != 0x0; }
    };

    /// Baseclass for a RibbonPage that is only visible when a specific type of document is active.
    class RibbonDocumentPage : public RibbonPage
    {
        Q_OBJECT
    public:
        /// Construct and bind.
        RibbonDocumentPage(BaseRibbonDocumentPageType* type, const QString& pageName) :
            RibbonPage(pageName),
            type_(type)
        {
            connect(Global_DocumentManager(), &DocumentManager::ActiveDocumentChanged, this, &RibbonDocumentPage::ActiveDocumentChanged);
            SetActive(false);
        }

    private slots:
        /// Set activity based on the new document.
        void ActiveDocumentChanged(DocumentBase* newDocument, DocumentBase* oldDocument) {
            if (type_->Valid(newDocument))
                SetActive(true);
            else
                SetActive(false);
        }

    private:
        BaseRibbonDocumentPageType* type_;
    };

}