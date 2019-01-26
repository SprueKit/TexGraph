#pragma once

#include "../../GuiBuilder/GraphDocument.h"
#include "../../Controls/BaseGraphControl.h"

#include <SprueEngine/TextureGen/TextureNode.h>

#include <Urho3D/Graphics/Material.h>

class QGraphicsScene;

namespace SprueEngine
{
    class Graph;
    struct SerializationContext;
}

namespace SprueEditor
{
    class TextureDocumentShelf;

    class TextureDocumentHandler : public DocumentHandler
    {
    public:
        virtual DocumentBase* CreateNewDocument() override;
        virtual DocumentBase* OpenDocument(const QString& path) override;
        virtual QString GetOpenFileFilter() const override;
        virtual QString GetSaveFileFilter() const override;
        virtual QString DocumentTypeName() const override { return "Texture Graph"; }
    };

    class TextureDocument : public GraphDocument
    {
        friend class TextureDocumentGraphVisitor;
    public:
        TextureDocument(DocumentHandler* handler, SprueEngine::Graph* graph, const QString& path);
        virtual ~TextureDocument();

        virtual bool Save() override;
        virtual bool DoBackup(const QString& backupDir) override;
        virtual bool HasExport() const override { return true; }
        virtual void BeginExport() override;
        virtual bool HasReports() const override;
        virtual void BeginReport() override;

        SprueEngine::Graph* GetGraph() { return graph_; }

        int GetDocumentIndex() const { return documentIndex_; }

        Urho3D::SharedPtr<Urho3D::Material> GetMaterial() const { return !showHeight_ ? GetPreviewMat().first : GetPreviewMat().second; }

        virtual QWidget* GetToolshelfWidget() override { return (QWidget*)shelfWidget_; }

        virtual QNEBlock* CreateBlockFor(SprueEngine::GraphNode*) override;
        virtual GRAPH_SOCKET_BRUSH_STRATEGY GetPortStrategy(SprueEngine::GraphSocket* forSocket) override;
        virtual QString GetGraphClipboardID() const override { return "SPRUEENGINE::TEXTUREGRAPH"; }
        virtual QMenu* GetContextMenu(QGraphicsView*, QGraphicsItem*, QPointF, std::vector<QAction*>& standardActions) override;
        virtual std::vector<QAction*> CreateNodeActions(QGraphicsView* view) override;

        int GetExportWorkCount() const;
        bool WriteTextures(const QString& path, const QString& namingConvention, int index, int format);

        typedef std::pair<Urho3D::SharedPtr<Urho3D::Material>, Urho3D::SharedPtr<Urho3D::Material>> PreviewMaterial;

        PreviewMaterial& GetRoughMat() const { return roughMetalPreviewMaterial_; }
        PreviewMaterial& GetGlossMat() const { return glossPreviewMaterial_; }

        void SetEnableHeightShader(bool state);

        void SetGlossMode(bool gloss);
        bool IsGlossMode() const { return glossMode_; }

    protected:
        PreviewMaterial& GetPreviewMat() const { return glossMode_ ? glossPreviewMaterial_ : roughMetalPreviewMaterial_; }

        virtual void ConnectionFormed(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to) override;
        virtual void ConnectionDestroyed(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to) override;

    private:
        void FillList(QGraphicsView*, std::vector<QAction*>& actionsList, const char* forItems);
        void FillMenu(QMenu* menu, const char* forItems, QPointF pos);
        void AddOutputMenu(QMenu* menu, const char* name, QPointF pos, SprueEngine::TexGenOutputFormat fmt, SprueEngine::TexGenOutputType type);
        void QueuePreviewUpdate(SprueEngine::GraphNode* node);

        /// The material used.
        mutable PreviewMaterial glossPreviewMaterial_;
        mutable PreviewMaterial roughMetalPreviewMaterial_;
        bool showHeight_ = false;
        bool glossMode_ = false;
        /// Underlying Urho3D scene for the view
        Urho3D::SharedPtr<Urho3D::Scene> urhoScene_;
        /// The index of texture document this is, used for constructing unique resource names
        int documentIndex_;
        TextureDocumentShelf* shelfWidget_;
        QMenu* contextMenu_;
        static std::vector<std::string> shadingTypeShaders_;
        std::vector<PreviewMaterial> materials_;

        static int nextDocumentIndex_;
    };

}