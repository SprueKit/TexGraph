#include "GraphCmds.h"

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/FString.h>

#include "../GuiBuilder/GraphDocument.h"
#include "../Controls/BaseGraphControl.h"

#include <QRect>

namespace SprueEditor
{

namespace GraphCommands
{

    CreateCmd::CreateCmd(SprueEngine::Graph* graph, std::shared_ptr<GraphNodeDataSource> newNode, bool isEntry) :
        graph_(graph),
        node_(newNode),
        isEntryPoint_(isEntry)
    {
        SetText(FString("Create Node %1", newNode->GetNode()->name).c_str());
    }

    CreateCmd::~CreateCmd()
    {
        if (lastActionWasUndo_)
            node_->Destroy();
    }

    void CreateCmd::Undo()
    {
        graph_->RemoveNode(node_->GetNode());
        SimpleSmartCommand::Undo();
    }

    void CreateCmd::Redo()
    {
        if (!IsFirstRedo())
            graph_->AddNode(node_->GetNode(), isEntryPoint_);
        SimpleSmartCommand::Redo();
    }

    DeleteCmd::DeleteCmd(SprueEngine::Graph* graph, std::shared_ptr<GraphNodeDataSource> deleting, bool isEntry) :
        graph_(graph),
        node_(deleting),
        isEntryPoint_(isEntry)
    {
        SetText(FString("Delete node %1", deleting->GetNode()->name).c_str());
        sockets_ = deleting->GetNode()->GetConnections();
    }

    DeleteCmd::~DeleteCmd()
    {
        if (!lastActionWasUndo_)
            node_->Destroy();
    }

    void DeleteCmd::Undo()
    {
        graph_->AddNode(node_->GetNode(), isEntryPoint_);
        node_->GetNode()->RestoreConnections(sockets_);
        SimpleSmartCommand::Undo();
    }

    void DeleteCmd::Redo()
    {
        graph_->RemoveNode(node_->GetNode());
        node_->GetNode()->graph = 0x0;
        SimpleSmartCommand::Redo();
    }

    ConnectionCreateCmd::ConnectionCreateCmd(SprueEngine::Graph* graph, std::shared_ptr<GraphNodeDataSource> from, std::shared_ptr<GraphNodeDataSource> to, unsigned fromSocket, unsigned toSocket) :
        graph_(graph),
        fromNode_(from),
        toNode_(to),
        fromSocket_(fromSocket),
        toSocket_(toSocket)
    {
        SetText(FString("Create connection between %1 and %2", from->GetNode()->name, to->GetNode()->name).c_str());
    }

    ConnectionCreateCmd::~ConnectionCreateCmd()
    {

    }

    void ConnectionCreateCmd::Undo()
    {
        if (auto fromNode = fromNode_->GetNode())
        {
            if (auto toNode = toNode_->GetNode())
            {
                auto fromSocket = fromNode->GetSocketByFlatIndex(fromSocket_);
                auto toSocket = toNode->GetSocketByFlatIndex(toSocket_);
                if (fromSocket && toSocket)
                    graph_->Disconnect(fromSocket, toSocket);
            }
        }
        SimpleSmartCommand::Undo();
    }
    void ConnectionCreateCmd::Redo()
    {
        if (!IsFirstRedo())
        {
            if (auto fromNode = fromNode_->GetNode())
            {
                if (auto toNode = toNode_->GetNode())
                {
                    auto fromSocket = fromNode->GetSocketByFlatIndex(fromSocket_);
                    auto toSocket = toNode->GetSocketByFlatIndex(toSocket_);
                    if (fromSocket && toSocket)
                        graph_->Connect(fromSocket, toSocket);
                }
            }
        }
        SimpleSmartCommand::Redo();
    }

    ConnectionDeleteCmd::ConnectionDeleteCmd(SprueEngine::Graph* graph, std::shared_ptr<GraphNodeDataSource> from, std::shared_ptr<GraphNodeDataSource> to, unsigned fromSocket, unsigned toSocket) :
        graph_(graph),
        fromNode_(from),
        toNode_(to),
        fromSocket_(fromSocket),
        toSocket_(toSocket)
    {
        SetText(FString("Delete connection between %1 and %2", from->GetNode()->name, to->GetNode()->name).c_str());
    }

    ConnectionDeleteCmd::~ConnectionDeleteCmd()
    {

    }

    void ConnectionDeleteCmd::Undo()
    {
        if (auto fromNode = fromNode_->GetNode())
        {
            if (auto toNode = toNode_->GetNode())
            {
                auto fromSocket = fromNode->GetSocketByFlatIndex(fromSocket_);
                auto toSocket = toNode->GetSocketByFlatIndex(toSocket_);
                if (fromSocket && toSocket)
                    graph_->Connect(fromSocket, toSocket);
            }
        }
        SimpleSmartCommand::Undo();
    }

    void ConnectionDeleteCmd::Redo()
    {
        if (auto fromNode = fromNode_->GetNode())
        {
            if (auto toNode = toNode_->GetNode())
            {
                auto fromSocket = fromNode->GetSocketByFlatIndex(fromSocket_);
                auto toSocket = toNode->GetSocketByFlatIndex(toSocket_);
                if (fromSocket && toSocket)
                    graph_->Disconnect(fromSocket, toSocket);
            }
        }
        SimpleSmartCommand::Redo();
    }

    PasteCmd::PasteCmd(GraphDocument* document, SprueEngine::Graph* graph, const SprueEngine::VectorBuffer& buffer, QPointF point, bool usePoint) :
        document_(document),
        graph_(graph),
        pasteBuffer_(buffer),
        point_(point),
        usePoint_(usePoint)
    {
        SetText("Paste graph nodes");
    }

    PasteCmd::~PasteCmd()
    {
        if (LastActionWasUndo())
        {
            for (auto editable : editables_)
                delete editable;
        }
    }

    void PasteCmd::Undo()
    {
        for (auto editable : editables_)
            graph_->RemoveNode(editable);
        SimpleSmartCommand::Undo();
    }

    void PasteCmd::Redo()
    {
        if (IsFirstRedo())
        {
            pasteBuffer_.Seek(0);
            SprueEngine::SerializationContext ctx;
            ctx.isClone_ = true;
            unsigned ct = pasteBuffer_.ReadUInt();
            while (ct)
            {
                if (SprueEngine::GraphNode* deserialized = SprueEngine::Context::GetInstance()->Deserialize<SprueEngine::GraphNode>(&pasteBuffer_, ctx))
                {
                    editables_.push_back(deserialized);
                    // Paste is a NEW instance, so the object's need to be made unique or they'll map erroneously
                    deserialized->MakeUnique();
                }
                --ct;
            }

            graph_->ReconstructLocalConnectivity(editables_, &pasteBuffer_);

            float minX = FLT_MAX;
            float maxX = FLT_MIN;
            float minY = FLT_MAX;
            float maxY = FLT_MIN;
            if (editables_.size() > 0)
            {
                for (auto editable : editables_)
                {
                    minX = std::min(minX, editable->XPos);
                    maxX = std::max(maxX, editable->XPos);
                    minY = std::min(minY, editable->YPos);
                    maxY = std::max(maxY, editable->YPos);
                }
                QRectF rect(minX, minY, maxX - minX, maxY - minY);
                QPointF centroid = rect.center();
                QPointF relPoint = rect.topLeft();

                for (auto editable : editables_)
                {
                    QPointF naturalPos(editable->XPos, editable->YPos);
                    if (usePoint_ && editables_.size() > 1)
                        naturalPos = (naturalPos - relPoint) + point_;
                    else if (usePoint_ && editables_.size() == 1)
                        naturalPos = point_;
                    else
                        naturalPos += QPointF(10, 10);

                    editable->XPos = naturalPos.x();
                    editable->YPos = naturalPos.y();
                    graph_->AddNode(editable, false);
                }
            }

            for (auto editable : editables_)
                sockets_.push_back(editable->GetConnections());
            BaseGraphControl::RebuildConnectors(document_, graph_);
        }
        else
        {
            for (auto editable : editables_)
                graph_->AddNode(editable, false);

            for (unsigned i = 0; i < sockets_.size(); ++i)
            {
                auto& conns = sockets_[i];
                auto node = editables_[i];
                if (!conns.empty())
                    node->RestoreConnections(conns);
            }
        }
        SimpleSmartCommand::Redo();
    }
}

}