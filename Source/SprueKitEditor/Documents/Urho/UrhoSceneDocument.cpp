#include "UrhoSceneDocument.h"

#include "../../GlobalAccess.h"
#include "UrhoSceneView.h"
#include "../../Views/RenderWidget.h"

using namespace SprueEditor;

namespace UrhoEditor
{

    DocumentBase* UrhoSceneDocumentHandler::CreateNewDocument()
    {
        return new UrhoSceneDocument(this, Urho3D::SharedPtr<Urho3D::Scene>());
    }

    DocumentBase* UrhoSceneDocumentHandler::OpenDocument(const QString& path)
    {
        return 0x0;
    }

    QString UrhoSceneDocumentHandler::GetOpenFileFilter() const
    {
        return "Urho3D Scene File (*.bin *.xml);;Binary Scene File (*.bin);;XML Scene File (*.xml)";
    }

    QString UrhoSceneDocumentHandler::GetSaveFileFilter() const
    {
        return "XML Scene File (*.xml);;Binary Scene File (*.bin)";
    }

    QString UrhoSceneDocumentHandler::DocumentTypeName() const
    {
        return "Urho3D Scene";
    }

    UrhoSceneDocument::UrhoSceneDocument(DocumentHandler* handler, Urho3D::SharedPtr<Urho3D::Scene> scene) :
        BaseUrhoDocument(handler, scene)
    {
        Urho3D::Context* context = Global_RenderWidget()->GetUrhoContext();
        views_.push_back(std::shared_ptr<ViewBase>(new UrhoSceneView(context, this)));
        activeView_ = views_[0];
        scene_ = activeView_->GetScene();
    }

    UrhoSceneDocument::~UrhoSceneDocument()
    {

    }

    bool UrhoSceneDocument::Save()
    {
        return false;
    }

    bool UrhoSceneDocument::DoBackup(const QString& backupDir)
    {
        return false;
    }

}