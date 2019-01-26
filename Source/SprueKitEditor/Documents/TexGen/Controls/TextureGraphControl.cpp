#include "TextureGraphControl.h"

#include "../Dialogs/SaveImageDialog.h"
#include "../../SprueKitEditor.h"
#include "../../Documents/TexGen/TextureDocument.h"
#include "../../GlobalAccess.h"
#include "../../Documents/TexGen/Tasks/TextureGenTask.h"

#include "../../Data/TexGenData.h"

#include "../../ThirdParty/NodeEditor/qneblock.h"
#include "../../ThirdParty/NodeEditor/qneconnection.h"
#include "../../ThirdParty/NodeEditor/qneport.h"
#include "../../ThirdParty/NodeEditor/qnodeseditor.h"

#include <EditorLib/TaskProcessor.h>

#include <QAction>
#include <qevent.h>
#include <QMenu>
#include <QMimeData>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Loaders/BasicImageLoader.h>

#include <memory>

using namespace SprueEngine;

namespace SprueEditor
{
    //TODO: this probably belongs somewhere else, quite likely to be needed elsewhere as well
    static const std::set<QString> TextureGraphSupportedTextures = {
        "bmp",
        "tga",
        "png",
        "jpg",
        "jpeg"
    };
    static const std::set<QString> TextureGraphSupportedModelsForBake = {
        "obj",
        "fbx"
    };

    class TextureGraphUpdateVisitor : public Graph::NodeVisitor
    {
    public:
        TextureGraphUpdateVisitor(TextureGraphControl* panel) : panel_(panel)
        {

        }

        virtual bool Visit(GraphNode* node) override
        {
            panel_->QueuePreviewUpdate(node);
            return true;
        }

        TextureGraphControl* panel_;
    };

#define DO_PREVIEW_UPDATE(NODE) if (graph_) { TextureGraphUpdateVisitor visitor(this); graph_->VisitDownStream(&visitor, NODE); }


    TextureGraphControl::TextureGraphControl()
    {
        setAcceptDrops(true);
        connect(Global_DocumentManager(), &DocumentManager::ActiveDocumentChanged, [=](DocumentBase* newDoc, DocumentBase* oldDoc)
        {
            if (TextureDocument* doc = dynamic_cast<TextureDocument*>(oldDoc))
            {
                doc->SetViewPosition(GetViewPosition());
                if (graph_ == doc->GetGraph())
                    UpdateGraphNodePositions();
            }
            if (TextureDocument* doc = dynamic_cast<TextureDocument*>(newDoc))
            {
                view_->setScene(doc->GetScene());
                SetGraphDocument(doc, false);
                SetViewPosition(doc->GetViewPosition());
            }
            else
            {
                view_->setScene(defaultScene_);
                SetGraphDocument(0x0, false);
            }
        });

        connect(Global_DocumentManager(), &DocumentManager::DocumentOpened, [=](DocumentBase* newDoc) {
            if (TextureDocument* doc = dynamic_cast<TextureDocument*>(newDoc))
            {
                doc->SetScene(CreateNewScene());
            }
        });

        connect(Global_DocumentManager(), &DocumentManager::DocumentClosed, [=](DocumentBase* closingDoc) {
            // If the document containing our graph is closed then clear the graph
            if (TextureDocument* doc = dynamic_cast<TextureDocument*>(closingDoc))
            {
                if (doc->GetGraph() == graph_)
                {
                    SetGraphDocument(0x0, false);
                    view_->setScene(defaultScene_); // default scene
                }
            }
        });
    }

    TextureGraphControl::~TextureGraphControl()
    {

    }

    void TextureGraphControl::PreviewGenerated(TextureGenTask* task)
    {
        if (!task || !task->GetNode())
            return;
        if (graph_ == 0x0)
            return;
        GraphNode* node = graph_->GetNodeByInstanceID(task->GetNode()->GetSourceID());
        if (node)
        {
            auto found = document_->GetNodeToBlockTable().find(node);
            if (found != document_->GetNodeToBlockTable().end())
            {
                found->second->SetPreviewImage(task->GetGeneratedImage());
                view_->repaint();
            }
        }
    }

    void TextureGraphControl::AllowConnect(bool& allowed, QNEPort* lhs, QNEPort* rhs)
    {
        BaseGraphControl::AllowConnect(allowed, lhs, rhs);
    }

    void TextureGraphControl::ConnectionDeleted(QNEPort* lhs, QNEPort* rhs)
    {
        BaseGraphControl::ConnectionDeleted(lhs, rhs);
    }

    void TextureGraphControl::dragMoveEvent(QDragMoveEvent* event)
    {
        if (!graph_)
            return;
        const auto mimeData = event->mimeData();
        if (mimeData->objectName().compare("TEXTURE_GRAPH_NODE") == 0)
            event->acceptProposedAction();
        else if (mimeData->hasText())
        {
            QString text = mimeData->text();
            for (auto record : TexGenData::NodeNames)
            {
                if (record.second.first == text)
                {
                    event->acceptProposedAction();
                    break;
                }
            }
        }
        else if (mimeData->hasUrls())
        {
            auto urls = mimeData->urls();
            for (auto url : urls)
            {
                QFileInfo info(url.fileName());
                if (TextureGraphSupportedTextures.find(info.suffix()) == TextureGraphSupportedTextures.end() && TextureGraphSupportedModelsForBake.find(info.suffix()) == TextureGraphSupportedModelsForBake.end())
                    return;
            }
            event->acceptProposedAction();
        }
    }

    void TextureGraphControl::dragEnterEvent(QDragEnterEvent* event)
    {
        if (!graph_)
            return;
        const auto mimeData = event->mimeData();
        std::string dataName = mimeData->objectName().toStdString();
        if (mimeData->objectName().compare("TEXTURE_GRAPH_NODE") == 0)
        {
            event->acceptProposedAction();
            return;
        }
        if (mimeData->hasText())
        {
            QString text = mimeData->text();
            for (auto record : TexGenData::NodeNames)
            {
                if (record.second.first == text)
                {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
        if (mimeData->hasUrls())
        {
            auto urls = mimeData->urls();
            for (auto url : urls)
            {
                QFileInfo info(url.fileName());
                if (TextureGraphSupportedTextures.find(info.suffix()) != TextureGraphSupportedTextures.end() || TextureGraphSupportedModelsForBake.find(info.suffix()) != TextureGraphSupportedModelsForBake.end())
                {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }

    void TextureGraphControl::dropEvent(QDropEvent* event)
    {
        if (!graph_)
            return;

        const auto mimeData = event->mimeData();
        QPointF pt = view_->mapToScene(event->pos());
        if (mimeData->hasUrls())
        {
            auto urls = mimeData->urls();
            ///TODO support multiple files
            if (urls.size() > 0)
            {
                /// only take the first URL
                auto url = urls[0];
                QFileInfo info(url.toLocalFile());
                if (TextureGraphSupportedTextures.find(info.suffix()) != TextureGraphSupportedTextures.end())
                {
                    if (GraphNode* graphNode = Context::GetInstance()->Create<GraphNode>("BitmapGenerator"))
                    {
                        graphNode->Construct();
                        graphNode->name = info.fileName().toStdString();
                        ResourceHandle ref("Image", info.filePath().toStdString());
                        graphNode->SetProperty(StringHash("Image"), ref);
                        graphNode->XPos = pt.x();
                        graphNode->YPos = pt.y();
                        graph_->AddNode(graphNode, false);
                        //document_->CreateBlockFor(graphNode)->setPos(pt);
                    }
                }
                else if (TextureGraphSupportedModelsForBake.find(info.suffix()) != TextureGraphSupportedModelsForBake.end())
                {
                    QMenu* pickBaker = new QMenu();
                    std::vector<TexGenData::NodeNameRecord> found = TexGenData::GetSortedNodeRecords("Bakers");
                    for (auto record : found)
                    {
                        QAction* action = new QAction(record.second.first);
                        pickBaker->addAction(action);
                        connect(action, &QAction::triggered, [this, record, info, pt]() {
                            if (GraphNode* graphNode = Context::GetInstance()->Create<GraphNode>(record.first))
                            {
                                graphNode->Construct();
                                graphNode->name = info.fileName().toStdString();
                                graphNode->XPos = pt.x();
                                graphNode->YPos = pt.y();
                                graph_->AddNode(graphNode, false);
                                ResourceHandle ref("Mesh", info.filePath().toStdString());
                                graphNode->SetProperty(StringHash("Mesh"), ref);
                                // Handled now by event handling
                                //document_->CreateBlockFor(graphNode)->setPos(pt);
                            }
                        });
                    }
                    pickBaker->exec(mapToGlobal(event->pos()));
                }
            }
        }
        else
        {
            std::string name = mimeData->text().toStdString();
            for (auto texGenRecord : TexGenData::NodeNames)
            {
                if (name.compare(texGenRecord.second.first) == 0)
                {
                    if (GraphNode* graphNode = Context::GetInstance()->Create<GraphNode>(texGenRecord.first))
                    {
                        graphNode->Construct();
                        graphNode->name = texGenRecord.second.first;
                        graphNode->XPos = pt.x();
                        graphNode->YPos = pt.y();
                        graph_->AddNode(graphNode, false);
                        //document_->CreateBlockFor(graphNode)->setPos(pt);
                    }
                    break;
                }
            }
        }
    }

    void TextureGraphControl::GraphNodeExternallyUdpdated(SprueEngine::GraphNode* node)
    {
        if (node)
            DO_PREVIEW_UPDATE(node);
    }

    void TextureGraphControl::QueuePreviewUpdate(SprueEngine::GraphNode* node)
    {
        if (node && node->CanPreview())
        {
            std::shared_ptr<Task> task = std::make_shared<TextureGenTask>(graph_, node, Global_DocumentManager()->GetActiveDocument());
            if (DocumentBase* doc = Global_DocumentManager()->GetActiveDocument())
                doc->AddDependentTask(task);
            Global_MainTaskProcessor()->AddTask(task);
        }
    }
}