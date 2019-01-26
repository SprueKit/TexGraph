#pragma once

#include "BaseUrhoDocument.h"

#include <QString>

namespace UrhoEditor
{

    class UrhoSceneDocumentHandler : public DocumentHandler
    {
    public:
        virtual DocumentBase* CreateNewDocument() override;
        virtual DocumentBase* OpenDocument(const QString& path) override;
        virtual QString GetOpenFileFilter() const override;
        virtual QString GetSaveFileFilter() const override;
        virtual QString DocumentTypeName() const override;
    };

    class UrhoSceneDocument : public BaseUrhoDocument
    {
    public:
        UrhoSceneDocument(DocumentHandler* handler, Urho3D::SharedPtr<Urho3D::Scene> scene);
        virtual ~UrhoSceneDocument();
            
        virtual bool Save() override;
        virtual bool DoBackup(const QString& backupDir) override;
    };

}