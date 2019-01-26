#pragma once

#include "../CommandID.h"
#include "../../Data/SprueDataSources.h"

#include <EditorLib/Commands/SmartUndoStack.h>
#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/VectorBuffer.h>

#include <QPoint>
#include <memory>

namespace SprueEditor
{
    class GraphDocument;

    namespace GraphCommands
    {
        class CreateCmd : public SimpleSmartCommand
        {
        public:
            CreateCmd(SprueEngine::Graph* graph, std::shared_ptr<GraphNodeDataSource> newNode, bool isEntry);
            virtual ~CreateCmd();

            virtual int GetID() const override { return CMD_GRAPH_CREATE; }

            virtual void Undo() override;
            virtual void Redo() override;

            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

        private:
            SprueEngine::Graph* graph_;
            std::shared_ptr<GraphNodeDataSource> node_;
            bool isEntryPoint_;
        };

        class DeleteCmd : public SimpleSmartCommand
        {
        public:
            DeleteCmd(SprueEngine::Graph* graph, std::shared_ptr<GraphNodeDataSource> deleting, bool isEntry);
            virtual ~DeleteCmd();

            virtual int GetID() const override { return CMD_GRAPH_DELETE; }

            virtual void Undo() override;
            virtual void Redo() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            SprueEngine::Graph* graph_;
            std::shared_ptr<GraphNodeDataSource> node_;
            bool isEntryPoint_;
            std::vector< std::pair<SprueEngine::GraphSocket*, SprueEngine::GraphSocket*> > sockets_;
        };

        class ConnectionCreateCmd : public SimpleSmartCommand
        {
        public:
            ConnectionCreateCmd(SprueEngine::Graph* graph, std::shared_ptr<GraphNodeDataSource> from, std::shared_ptr<GraphNodeDataSource> to, unsigned fromSocket, unsigned toSocket);
            virtual ~ConnectionCreateCmd();

            virtual int GetID() const override { return CMD_GRAPH_CONNECT; }

            virtual void Undo() override;
            virtual void Redo() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            SprueEngine::Graph* graph_;
            std::shared_ptr<GraphNodeDataSource> fromNode_;
            std::shared_ptr<GraphNodeDataSource> toNode_;
            unsigned fromSocket_;
            unsigned toSocket_;
        };

        class ConnectionDeleteCmd : public SimpleSmartCommand
        {
        public:
            ConnectionDeleteCmd(SprueEngine::Graph* graph, std::shared_ptr<GraphNodeDataSource> from, std::shared_ptr<GraphNodeDataSource> to, unsigned fromSocket, unsigned toSocket);
            virtual ~ConnectionDeleteCmd();

            virtual int GetID() const override { return CMD_GRAPH_DISCONNECT; }

            virtual void Undo() override;
            virtual void Redo() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            SprueEngine::Graph* graph_;
            std::shared_ptr<GraphNodeDataSource> fromNode_;
            std::shared_ptr<GraphNodeDataSource> toNode_;
            unsigned fromSocket_;
            unsigned toSocket_;
        };

        class PasteCmd : public SimpleSmartCommand
        {
        public:
            PasteCmd(GraphDocument* doc, SprueEngine::Graph* graph, const SprueEngine::VectorBuffer& buffer, QPointF point, bool usePoint);
            virtual ~PasteCmd();

            virtual int GetID() const { return CMD_GRAPH_PASTE; }
            virtual void Undo() override;
            virtual void Redo() override;
            virtual bool CanMergeWith(const SmartCommand* rhs) override { return false; }

            GraphDocument* document_;
            SprueEngine::Graph* graph_;
            SprueEngine::VectorBuffer pasteBuffer_;
            std::vector<SprueEngine::GraphNode*> editables_;
            QPointF point_;
            bool usePoint_;
            typedef std::vector< std::pair<SprueEngine::GraphSocket*, SprueEngine::GraphSocket*> > SocketList;
            std::vector<SocketList> sockets_;
        };
    }
}