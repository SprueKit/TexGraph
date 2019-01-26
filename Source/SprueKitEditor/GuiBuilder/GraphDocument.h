#pragma once

#include "../Controls/BaseGraphControl.h"

#include <EditorLib/DocumentBase.h>

#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/Graph/GraphNode.h>
#include <SprueEngine/Graph/GraphSocket.h>

#include <QGraphicsScene>

class QNEBlock;
class QNEConnection;
class QNEPort;

namespace SprueEditor
{
    typedef void(*GRAPH_SOCKET_BRUSH_STRATEGY)(QNEPort*);

    /// Baseclass for documents that should implement node graphs. Provides methods used by the graphing controls for constructing blocks and examining the scene.
    class GraphDocument : public DocumentBase
    {
        Q_OBJECT
    public:
        GraphDocument(DocumentHandler* handler, SprueEngine::Graph* graph);
        virtual ~GraphDocument();

        SprueEngine::Graph* GetGraph() { return graph_; }

        std::map<SprueEngine::GraphNode*, QNEBlock*>& GetNodeToBlockTable() { return nodeToBlock_; }

        QGraphicsScene* GetScene() { return scene_; }
        void SetScene(QGraphicsScene* scene) { scene_ = scene; }
        BaseGraphControl::ViewPosition GetViewPosition() { return viewPosition_; }
        void SetViewPosition(const BaseGraphControl::ViewPosition& pos) { viewPosition_ = pos; }

        /// Constructs a node block.
        virtual QNEBlock* CreateBlockFor(SprueEngine::GraphNode*);
        virtual void Connect(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to);
        virtual void Disconnect(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to);
        virtual GRAPH_SOCKET_BRUSH_STRATEGY GetPortStrategy(SprueEngine::GraphSocket* forSocket) = 0;
        virtual QString GetGraphClipboardID() const = 0;
        virtual QMenu* GetContextMenu(QGraphicsView*, QGraphicsItem* item, QPointF pos, std::vector<QAction*>& standardActions) = 0;
        virtual std::vector<QAction*> CreateNodeActions(QGraphicsView*) = 0;

    protected slots:
        void DataChanged(void* source, Selectron* selectron, unsigned propertyHash);

    protected:
        /// Called by ConnectionAdded() when a connection was appropriately formed, override to deal with special needs like preview.
        virtual void ConnectionFormed(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to) { }
        /// Called by ConnectionRemoved() when a connection was appropriately formed, override to deal with special needs like preview.
        virtual void ConnectionDestroyed(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to) { }

        BaseGraphControl::ViewPosition viewPosition_;
        /// The graph that is being edited
        SprueEngine::Graph* graph_ = 0x0;
        /// The graphics scene for the graph (eliminates having to fully rebuild the graph on document switch, which can be very slow)
        QGraphicsScene* scene_ = 0x0;

        std::map<SprueEngine::GraphNode*, QNEBlock*> nodeToBlock_;

        static void GraphNodeAdded(SprueEngine::GraphNode* node);
        static void GraphNodeRemoved(SprueEngine::GraphNode* node);
        static void ConnectionAdded(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to);
        static void ConnectionRemoved(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to);
        static void SocketsChanged(SprueEngine::GraphNode* node);
        static bool callbacksAdded_;
        static std::vector<GraphDocument*> documents_;
    };

}