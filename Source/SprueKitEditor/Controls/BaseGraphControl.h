#pragma once

#include "../ThirdParty/NodeEditor/qneblock.h"

#include <EditorLib/Search/ISearchable.h>
#include <EditorLib/ScrollAreaWidget.h>
#include <EditorLib/Selectron.h>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <qevent.h>

#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/Graph/GraphNode.h>
#include <SprueEngine/Graph/GraphSocket.h>

#include <vector>

class QMenu;
class QNodesEditor;
class QNEPort;
class QNEBlock;

namespace SprueEditor
{
    class GraphDocument;

    /// Will eventually render subclassing of the BaseGraphControl obsolete through this interface.
    class GraphHandlingStrategy
    {
    public:
        virtual bool RelevantForDocument(GraphDocument* doc) = 0;
        virtual void contextMenuEvent(QContextMenuEvent*) = 0;
        virtual void dragMoveEvent(QDragMoveEvent*) = 0;
        virtual void dragEnterEvent(QDragEnterEvent*) = 0;
        virtual void dropEvent(QDropEvent*) = 0;
    };

    /// Baseclass to be used for implementing Graph based editors.
    /// Most functionality for graphs lives here (in overridable form)
    class BaseGraphControl : public ScrollAreaWidget, public SelectronLinked, public ISearchable
    {
        friend class GraphGraphicsView;
        friend class GraphNodeSearchResult;
        Q_OBJECT
    public:
        BaseGraphControl();
        virtual ~BaseGraphControl();

        /// Sets the graph to display in the viewport
        virtual void SetGraphDocument(GraphDocument* graph, bool build);
        /// Adjusts the zoom level (zoom values are fixed and this is the index in the zoom value list
        virtual void SetZoom(int zoomLevel);
        /// Increments zoom N steps down or up
        virtual void IncrementZoom(int delta);
        
        virtual void Link(Selectron* selectron);

        struct ViewPosition
        {
            QTransform transform_;
            int zoomLevel_;
            int horizontalPosition_;
            int verticalPosition_;
            bool valid_ = false;
        };

        static void RebuildConnectors(GraphDocument* doc, SprueEngine::Graph* graph);

    public slots:
        virtual void SelectionChanged();

        /// Determines whether a connection/block is allowed to be deleted
        virtual void AllowDelete(bool&, QGraphicsItem*);
        /// Determines whether two ports may have a connection between them (does any consequence cleanup)
        virtual void AllowConnect(bool&, QNEPort*, QNEPort*);
        /// Called whenever a connection is deleted between two ports
        virtual void ConnectionDeleted(QNEPort* lhs, QNEPort* rhs);
        /// Called whenever a block/node is deleted
        virtual void NodeDeleted(QNEBlock* block);

        /// Invoked by the copy action
        virtual void OnCopy();
        /// Invoked by the paste action
        virtual void OnPaste();
        /// Invoked by the cut action
        virtual void OnCut();

    protected: // Overridable core methods
        /// Respond to any key commands (zooming, etc), should invoke this base method first if overriding
        virtual void keyReleaseEvent(QKeyEvent*) Q_DECL_OVERRIDE;
        virtual void contextMenuEvent(QContextMenuEvent*) Q_DECL_OVERRIDE;
        virtual void dragMoveEvent(QDragMoveEvent*) Q_DECL_OVERRIDE;
        virtual void dragEnterEvent(QDragEnterEvent*) Q_DECL_OVERRIDE;
        virtual void dropEvent(QDropEvent*) Q_DECL_OVERRIDE;

        virtual void CollectSearchResults(const QStringList& searchTerms, SearchResultVector& results, bool caseSensitive = false, bool exactMatch = false) const override;

        /// Override for support the "Quick Action" dialog
        virtual std::vector<QAction*> CreateNodeActions();

        /// Override to perform any extra work (such as updating a preview) when a graph node has been externally chagned
        virtual void GraphNodeExternallyUdpdated(SprueEngine::GraphNode*) { }
        /// Must be invoked before saving to store the visual positions of the nodes
        virtual void UpdateGraphNodePositions();
        /// Deletes all selected nodes/connections
        virtual void DeleteSelected();
        /// Clears and refreshes all connections in the graphics scene to match those in the Graph
        virtual void RebuildConnectors();

        ViewPosition GetViewPosition();
        void SetViewPosition(const ViewPosition& pos);

        /// Performs initial scene construction and assures single connectivity of signals
        virtual QGraphicsScene* CreateNewScene() {
            QGraphicsScene* scene = new QGraphicsScene();
            scene->setFont(QFont("Arial", 10));
            connect(scene, &QGraphicsScene::selectionChanged, this, &BaseGraphControl::SelectionChanged);
            return scene;
        }

    protected:
        GraphDocument* document_ = 0x0;
        QNodesEditor* nodesEditor_ = 0x0;
        QGraphicsView* view_ = 0x0;
        QGraphicsScene* defaultScene_ = 0x0;
        int currentZoom_;
        SprueEngine::Graph* graph_ = 0x0;

        QAction* copyAction_ = 0x0;
        QAction* pasteAction_ = 0x0;
        QAction* cutAction_ = 0x0;
        QAction* testAction_ = 0x0;
        std::vector<QAction*> standardActions_;
        QPoint lastMouse_;
        bool dragging_;

        // Used to defer memory free
        struct DeletionDenial {
            DeletionDenial() {
                Denied = false;
                StackCt = 1;
            }
            bool Denied;
            int StackCt;
        };
        std::map<SprueEngine::GraphNode*, DeletionDenial> deletionDenials_;
        std::vector<GraphHandlingStrategy*> strategies_;
    };

}