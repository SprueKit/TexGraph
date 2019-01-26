#include "GraphDocument.h"

#include "../../GlobalAccess.h"
#include "../../SprueKitEditor.h"
#include "../Commands/SprueKit/GraphCmds.h"

#include "../ThirdParty/NodeEditor/qneblock.h"
#include "../ThirdParty/NodeEditor/qneconnection.h"
#include "../ThirdParty/NodeEditor/qneport.h"

#include <SprueEngine/Core/Context.h>

namespace SprueEditor
{
    bool GraphDocument::callbacksAdded_ = false;
    std::vector<GraphDocument*> GraphDocument::documents_ = std::vector<GraphDocument*>();

    void QNEBlocKMovedCallback(QNEBlock* block)
    {
        if (SprueEngine::GraphNode* node = (SprueEngine::GraphNode*)block->getUserData())
        {
            node->XPos = block->scenePos().x();
            node->YPos = block->scenePos().y();
        }
    }

    GraphDocument::GraphDocument(DocumentHandler* handler, SprueEngine::Graph* graph) :
        DocumentBase(handler),
        graph_(graph)
    {
        documents_.push_back(this);
        if (!callbacksAdded_)
        {
            SprueEngine::Context::GetInstance()->GetCallbacks().AddGraphNodeAdded(GraphNodeAdded);
            SprueEngine::Context::GetInstance()->GetCallbacks().AddGraphNodeRemoved(GraphNodeRemoved);
            SprueEngine::Context::GetInstance()->GetCallbacks().AddGraphConnnectionAdded(ConnectionAdded);
            SprueEngine::Context::GetInstance()->GetCallbacks().AddGraphConnectionRemoved(ConnectionRemoved);
            SprueEngine::Context::GetInstance()->GetCallbacks().AddGraphNodeSocketsChanged(SocketsChanged);
            callbacksAdded_ = true;
        }
    }

    GraphDocument::~GraphDocument()
    {
        auto found = std::find(documents_.begin(), documents_.end(), this);
        if (found != documents_.end())
            documents_.erase(found);
    }


    QNEBlock* GraphDocument::CreateBlockFor(SprueEngine::GraphNode* node)
    {
        QNEBlock* block = new QNEBlock(QNEBlocKMovedCallback);
        scene_->addItem(block);
        block->addPort(node->name.c_str(), false, QNEPort::NamePort);

        for (auto socket : node->inputSockets)
        {
            if (!socket->secret)
                block->addPort(socket->name.c_str(), false, 0, socket, GetPortStrategy(socket));
        }

        for (auto socket : node->outputSockets)
            if (!socket->secret)
                block->addPort(socket->name.c_str(), true, 0, socket, GetPortStrategy(socket));

        block->setPos(node->XPos, node->YPos);
        nodeToBlock_[node] = block;
        block->setUserData(node);

        return block;
    }

    void GraphDocument::Connect(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to)
    {
        GraphCommands::ConnectionCreateCmd* cmd = new GraphCommands::ConnectionCreateCmd(graph_, std::make_shared<GraphNodeDataSource>(from->node), std::make_shared<GraphNodeDataSource>(to->node), from->node->GetSocketFlatIndex(from), to->node->GetSocketFlatIndex(to));
        GetUndoStack()->Push(cmd);
    }

    void GraphDocument::Disconnect(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to)
    {
        auto sceneItems = scene_->items();
        for (auto item : sceneItems)
        {
            if (QNEConnection* conn = dynamic_cast<QNEConnection*>(item))
            {
                if ((conn->port1()->ptr() == from && conn->port2()->ptr() == to) || (conn->port2()->ptr() == from && conn->port1()->ptr() == from))
                {
                    scene_->removeItem(conn);
                    delete conn;
                    scene_->update();
                }
            }
        }
    }

    void GraphDocument::GraphNodeAdded(SprueEngine::GraphNode* node)
    {
        for (auto doc : documents_)
        {
            if (doc->GetGraph() == node->graph)
            {
                QNEBlock* block = doc->CreateBlockFor(node);
                doc->nodeToBlock_.insert(std::make_pair(node, block));
                doc->SetDirty(true);
                return;
            }
        }
    }

    void GraphDocument::GraphNodeRemoved(SprueEngine::GraphNode* node)
    {
        for (auto doc : documents_)
        {
            if (doc->GetGraph() == node->graph)
            {
                auto found = doc->nodeToBlock_.find(node);
                if (found != doc->nodeToBlock_.end())
                {
                    auto block = found->second;
                    doc->scene_->removeItem(block);
                    doc->nodeToBlock_.erase(found);
                    doc->SetDirty(true);
                    return;
                }
            }
        }
    }

    void GraphDocument::ConnectionAdded(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to)
    {
        for (auto doc : documents_)
        {
            if (doc->GetGraph() == from->node->graph)
            {
                auto fromNode = doc->nodeToBlock_.find(from->node);
                auto toNode = doc->nodeToBlock_.find(to->node);

                if (fromNode->second && toNode->second)
                {
                    auto fromPort = fromNode->second->findPort(from);
                    auto toPort = toNode->second->findPort(to);

                    if (fromPort && toPort)
                    {
                        QNEConnection* conn = new QNEConnection();
                        conn->setPort1(toPort);
                        conn->setPort2(fromPort);
                        conn->updatePosFromPorts();
                        conn->updatePath();
                        doc->scene_->addItem(conn);
                        doc->ConnectionFormed(fromPort->isOutput() ? from : to, fromPort->isOutput() ? to : from);
                        doc->SetDirty(true);
                        return;
                    }
                }
            }
        }
    }

    void GraphDocument::ConnectionRemoved(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to)
    {
        for (auto doc : documents_)
        {
            if (doc->GetGraph() == from->node->graph)
            {
                if (QNEConnection* conn = QNEConnection::GetConnection(doc->scene_, from, to))
                {
                    doc->scene_->removeItem(conn);
                    delete conn;
                    doc->ConnectionDestroyed(from, to);
                    doc->SetDirty(true);
                    return;
                }
            }
        }
    }

    void GraphDocument::SocketsChanged(SprueEngine::GraphNode* node)
    {
        for (auto doc : documents_)
        {
            if (doc->GetGraph() == node->graph)
            {
                auto found = doc->GetNodeToBlockTable().find(node);
                if (found != doc->GetNodeToBlockTable().end())
                {
                    found->second->clearOutputPorts();
                    for (auto socket : node->outputSockets)
                        if (!socket->secret)
                            found->second->addPort(socket->name.c_str(), true, 0, socket, doc->GetPortStrategy(socket));

                    found->second->scene()->invalidate(found->second->sceneBoundingRect());
                }
                return;
            }
        }
    }

    void GraphDocument::DataChanged(void* source, Selectron* selectron, unsigned propertyHash)
    {
        if (this == Global_DocumentManager()->GetActiveDocument())
            SetDirty(true);
    }
}