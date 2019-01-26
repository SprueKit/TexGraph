#include "BaseGraphControl.h"

#include "../GlobalAccess.h"
#include "../Data/SprueDataSources.h"
#include "../Commands/SprueKit/GraphCmds.h"
#include "../InternalNames.h"

#include "../GuiBuilder/GraphDocument.h"
#include "../SprueKitEditor.h"
#include "../Localization/LocalizedWidgets.h"

#include "../ThirdParty/NodeEditor/qneblock.h"
#include "../ThirdParty/NodeEditor/qneconnection.h"
#include "../ThirdParty/NodeEditor/qneport.h"
#include "../ThirdParty/NodeEditor/qnodeseditor.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Dialogs/QuickActionDlg.h>
#include <EditorLib/Localization/Localizer.h>
#include <EditorLib/Selectron.h>

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QDockWidget>
#include <QMenu>
#include <QMimeData>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/VectorBuffer.h>

using namespace SprueEngine;

namespace SprueEditor
{

    class GraphNodeSearchResult : public SearchResult
    {
    public:
        GraphNodeSearchResult(BaseGraphControl* focusWidget, std::shared_ptr<GraphNodeDataSource> dataSource, QString text, int hitCt) :
            dataSource_(dataSource),
            widget_(focusWidget)
        {
            text_ = text;
            source_ = Localizer::Translate("Graph");
            hitCount_ = hitCt;
        }

        virtual void GoTo()
        {
            if (!widget_ || !widget_->document_)
                return;

            if (auto dock = SprueKitEditor::GetInstance()->GetDock(TEXGRAPH_PANEL))
                if (!dock->isVisible())
                    dock->setVisible(true);

            if (!widget_->hasFocus())
                widget_->setFocus();
            
            auto selectedItems = widget_->document_->GetScene()->selectedItems();
            for (auto selItem : selectedItems)
                selItem->setSelected(false);

            if (QNEBlock* block = QNEBlock::FindBlock(widget_->document_->GetScene(), dataSource_->GetNode()))
            {
                block->setSelected(true);
                widget_->view_->centerOn(block);
            }
        }

        std::shared_ptr<GraphNodeDataSource> dataSource_;
        BaseGraphControl* widget_;
    };

    static float GraphZoomLevels[] = {
        0.25f,
        0.5f,
        0.75f,
        1.0f,
        1.25f,
        1.5f,
        1.75f,
        2.0f,
        2.25f,
        2.5f,
        2.75f,
        3.0f,
        3.5f,
        4.0f,
        4.5f,
        5.0f,
    };

    static const char* GraphZoomLevelsText[] = {
        "25",
        "50",
        "75",
        "100",
        "125",
        "150",
        "175",
        "200",
        "225",
        "250",
        "275",
        "300",
        "350",
        "400",
        "450",
        "500",
    };

    static const unsigned graphZoomLevels = 16;

    class GraphGraphicsView : public QGraphicsView, public IQuickActionSource
    {
    public:
        GraphGraphicsView(BaseGraphControl* owner) :
            QGraphicsView(),
            owner_(owner)
        {
            setRubberBandSelectionMode(Qt::ItemSelectionMode::IntersectsItemBoundingRect);
        }

        virtual void mouseMoveEvent(QMouseEvent* evt)
        {
            // Support middle mouse drag scrolling
            if (evt->buttons() & Qt::MouseButton::MiddleButton)
            {
                QPoint delta = evt->pos() - lastMouse_;
                int vPos = verticalScrollBar()->value();
                int hPos = horizontalScrollBar()->value();
                verticalScrollBar()->setValue(vPos - delta.y() * 2);
                horizontalScrollBar()->setValue(hPos - delta.x() * 2);
                evt->accept();
                lastMouse_ = evt->pos();
                return;
            }
            lastMouse_ = evt->pos();

            QGraphicsView::mouseMoveEvent(evt);
        }

        virtual void wheelEvent(QWheelEvent* evt)
        {
            if (QApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier)
            {
                owner_->IncrementZoom(evt->delta() > 0 ? 1 : -1);
                return;
            }
            QGraphicsView::wheelEvent(evt);
        }

        // Draw the gridlines in the graph view
        virtual void drawBackground(QPainter *painter, const QRectF &rect) override
        {
            const int gridSize = 50;

            qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
            qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

            QVarLengthArray<QLineF, 100> lines;

            for (qreal x = left; x < rect.right(); x += gridSize)
                lines.append(QLineF(x, rect.top(), x, rect.bottom()));
            for (qreal y = top; y < rect.bottom(); y += gridSize)
                lines.append(QLineF(rect.left(), y, rect.right(), y));

            painter->setPen(QPen(QColor(60, 60, 60)));
            painter->drawLines(lines.data(), lines.size());
        }

        virtual void dragMoveEvent(QDragMoveEvent* event) override
        {
            owner_->dragMoveEvent(event);
        }

        virtual void dragEnterEvent(QDragEnterEvent* event) override
        {
            owner_->dragEnterEvent(event);
        }

        virtual void dropEvent(QDropEvent* event) override
        {
            owner_->dropEvent(event);
        }

        // implementation of IQuickActionSource
        virtual std::vector<QAction*> GetActions() const override
        {
            return owner_->CreateNodeActions();
        }

        BaseGraphControl::ViewPosition GetPosition()
        {
            BaseGraphControl::ViewPosition ret;
            ret.transform_ = transform();
            ret.verticalPosition_ = verticalScrollBar()->value();
            ret.horizontalPosition_ = horizontalScrollBar()->value();
            return ret;
        }

        void SetPosition(const BaseGraphControl::ViewPosition& pos)
        {
            setTransform(pos.transform_);
            verticalScrollBar()->setValue(pos.verticalPosition_);
            horizontalScrollBar()->setValue(pos.horizontalPosition_);
        }

        BaseGraphControl* owner_;
        QPoint lastMouse_;
    };

    BaseGraphControl::BaseGraphControl() :
        currentZoom_(3),
        dragging_(false),
        graph_(0x0),
        nodesEditor_(0x0)
    {
        setAcceptDrops(true);
        setMouseTracking(true);
        vBoxLayout_->setMargin(0);
        vBoxLayout_->setSpacing(0);

        QScroller::grabGesture(this, QScroller::TouchGesture);

        defaultScene_ = new QGraphicsScene();
        view_ = new GraphGraphicsView(this);
        vBoxLayout_->addWidget(view_);
        view_->setScene(defaultScene_);
        //view_->setRenderHint(QPainter::Antialiasing, true);
        defaultScene_->setFont(QFont("Arial", 10));

        SetGraphDocument(0x0, false);

        copyAction_ = new QAction("Copy");
        copyAction_->setShortcutContext(Qt::ShortcutContext::WindowShortcut);
        copyAction_->setShortcut(QKeySequence("CTRL+C"));
        addAction(copyAction_);
        connect(copyAction_, SIGNAL(triggered()), this, SLOT(OnCopy()));

        pasteAction_ = new QAction("Paste");
        pasteAction_->setShortcutContext(Qt::ShortcutContext::WindowShortcut);
        pasteAction_->setShortcut(QKeySequence("CTRL+V"));
        addAction(pasteAction_);
        connect(pasteAction_, SIGNAL(triggered()), this, SLOT(OnPaste()));

        cutAction_ = new QAction("Cut");
        cutAction_->setShortcut(QKeySequence("CTRL+X"));
        addAction(cutAction_);
        connect(cutAction_, SIGNAL(triggered()), this, SLOT(OnCut()));

        standardActions_.push_back(cutAction_);
        standardActions_.push_back(copyAction_);
        standardActions_.push_back(pasteAction_);
    }

    BaseGraphControl::~BaseGraphControl()
    {
        delete view_;
        delete defaultScene_;
    }

    void BaseGraphControl::SetGraphDocument(GraphDocument* doc, bool build)
    {
        document_ = doc;
        if (document_)
            graph_ = document_->GetGraph();
        else
            graph_ = 0x0;

        if (nodesEditor_)
            delete nodesEditor_;

        nodesEditor_ = new QNodesEditor(this);
        connect(nodesEditor_, &QNodesEditor::AllowDelete, this, &BaseGraphControl::AllowDelete);
        connect(nodesEditor_, &QNodesEditor::AllowConnect, this, &BaseGraphControl::AllowConnect);
        connect(nodesEditor_, &QNodesEditor::ConnectionDeleted, this, &BaseGraphControl::ConnectionDeleted);
        connect(nodesEditor_, &QNodesEditor::NodeDeleted, this, &BaseGraphControl::NodeDeleted);
        nodesEditor_->install(view_->scene(), view_);

        // If the scene is empty then reconstruct
        if (graph_ && view_->scene()->items().size() == 0)
        {
            for (auto node : graph_->GetNodes())
            {
                QNEBlock* block = document_->CreateBlockFor(node);
                block->setPos(node->XPos, node->YPos);
            }

            RebuildConnectors();
        }
    }

    void BaseGraphControl::SetZoom(int zoomLevel)
    {
        currentZoom_ = zoomLevel;
        currentZoom_ = CLAMP(currentZoom_, 0, graphZoomLevels - 1);

        QTransform trans;
        trans.scale(GraphZoomLevels[currentZoom_], GraphZoomLevels[currentZoom_]);
        view_->setTransform(trans, false);
    }

    void BaseGraphControl::IncrementZoom(int delta)
    {
        SetZoom(currentZoom_ + delta);
    }

    void BaseGraphControl::Link(Selectron* sel)
    {
        connect(sel, &Selectron::SelectionChanged, [=](void* src, Selectron* who) {
            if (src == this)
                return;
            if (!document_)
                return;

            view_->scene()->blockSignals(true);

            view_->scene()->clearSelection();
            if (auto editable = who->GetSelected<SprueEditor::GraphNodeDataSource>())
            {
                auto found = document_->GetNodeToBlockTable().find(editable->GetNode());
                if (found != document_->GetNodeToBlockTable().end())
                    found->second->setSelected(true);
            }
            view_->repaint();

            view_->scene()->blockSignals(false);
        });

        connect(sel, &Selectron::DataChanged, [=](void* src, Selectron* who, unsigned property) {
            if (src == this)
                return;
            if (!document_)
                return;
            if (auto editable = who->GetSelected<SprueEditor::GraphNodeDataSource>())
            {
                GraphNode* node = editable->GetNode();
                auto found = document_->GetNodeToBlockTable().find(node);
                if (found != document_->GetNodeToBlockTable().end())
                {
                    for (auto port : found->second->ports())
                    {
                        if (port->portFlags() & QNEPort::NamePort)
                        {
                            port->setName(node->name.c_str());
                            found->second->updatePreview();
                        }
                    }
                }
                GraphNodeExternallyUdpdated(node);
            }
        });
    }

    void BaseGraphControl::SelectionChanged()
    {
        auto selection = view_->scene()->selectedItems();
        std::vector<GraphNode*> selectedNodes;
        for (unsigned i = 0; i < selection.size(); ++i)
        {
            if (QNEBlock* node = dynamic_cast<QNEBlock*>(selection[i]))
                selectedNodes.push_back((GraphNode*)node->getUserData());
        }
        if (selectedNodes.size())
        {
            for (unsigned i = 0; i < selectedNodes.size(); ++i)
            {
                if (i == 0)
                    GetSelectron()->SetSelected(this, std::shared_ptr<DataSource>(new GraphNodeDataSource(selectedNodes[i])));
                else
                    GetSelectron()->AddSelected(this, std::shared_ptr<DataSource>(new GraphNodeDataSource(selectedNodes[i])));
            }
        }
        else
            GetSelectron()->SetSelected(this, 0x0);
    }

    void BaseGraphControl::AllowDelete(bool& allowed, QGraphicsItem* item)
    {
        allowed = true;
    }

    void BaseGraphControl::AllowConnect(bool& allowed, QNEPort* lhs, QNEPort* rhs)
    {
        GraphSocket* left = (GraphSocket*)lhs->ptr();
        GraphSocket* right = (GraphSocket*)rhs->ptr();
        if (graph_ && graph_->CanConnect(left, right))
        {
            allowed = true;

            // Is the Graph class going to remove any connection?
            // NOTE: if conditions become complicated it may be necessary to add some "I deleted this/these reporting"
            GraphSocket* clearSocket = right->input && !right->control ? right : (left->input && !left->control ? left : 0x0);
            if (clearSocket)
            {
                if (clearSocket->HasConnections())
                {
                    for (auto con : rhs->connections())
                    {
                        view_->scene()->removeItem(con);
                        delete con;
                    }
                }
            }

            graph_->Connect(left, right);
            document_->Connect(left, right);
        }
        else
            allowed = false;
    }
        
    void BaseGraphControl::ConnectionDeleted(QNEPort* lhs, QNEPort* rhs)
    {
        if (graph_ && lhs && rhs)
        {
            auto leftSocket = (GraphSocket*)lhs->ptr();
            auto rightSocket = (GraphSocket*)rhs->ptr();
            
            GraphCommands::ConnectionDeleteCmd* cmd = new GraphCommands::ConnectionDeleteCmd(graph_, std::shared_ptr<GraphNodeDataSource>(new GraphNodeDataSource(leftSocket->node)), std::shared_ptr<GraphNodeDataSource>(new GraphNodeDataSource(rightSocket->node)), leftSocket->node->GetSocketFlatIndex(leftSocket), rightSocket->node->GetSocketFlatIndex(rightSocket));
            Global_DocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd);
            //graph_->Disconnect(leftSocket, rightSocket);
        }
    }

    void BaseGraphControl::NodeDeleted(QNEBlock* block)
    {
        if (graph_)
        {
            GraphNode* node = (GraphNode*)block->getUserData();

            GraphCommands::DeleteCmd* cmd = new GraphCommands::DeleteCmd(graph_, std::shared_ptr<GraphNodeDataSource>(new GraphNodeDataSource(node)), graph_->IsEntryNode(node));
            Global_DocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd);

            //graph_->RemoveNode(node);
            //document_->GetNodeToBlockTable().erase(node);
            //if (foundDenial != deletionDenials_.end())
            //    foundDenial->second.Denied = true;
            //else
            //{
            //    for (auto port : block->ports())
            //    {
            //        if (port->isOutput())
            //        {
            //            for (auto con : port->connections())
            //                ConnectionDeleted(con->port1(), con->port2());
            //        }
            //    }
            //    delete node;
            //}
        }
    }

    void BaseGraphControl::OnCopy()
    {
        if (graph_)
        {
            UpdateGraphNodePositions();
            graph_->AssignIDs();
            std::vector<GraphNode*> editables;
            for (auto item : view_->scene()->items())
            {
                if (item->isSelected())
                {
                    if (QNEBlock* block = dynamic_cast<QNEBlock*>(item))
                    {
                        if (block->getUserData())
                            editables.push_back((GraphNode*)block->getUserData());
                    }
                }
            }

            if (editables.size() > 0)
            {
                QClipboard* clip = QApplication::clipboard();
                SprueEngine::VectorBuffer buffer;
                buffer.WriteUInt(editables.size());
                SprueEngine::SerializationContext ctx;
                ctx.isClone_ = true;
                for (auto editable : editables)
                    editable->Serialize(&buffer, ctx);

                graph_->CollectLocalConnectivity(editables, &buffer);

                QMimeData* data = new QMimeData();
                QByteArray array(buffer.GetSize(), Qt::Initialization::Uninitialized);
                memcpy(array.data(), buffer.GetData(), buffer.GetSize());
                data->setData(document_->GetGraphClipboardID(), array);
                clip->setMimeData(data);
            }
        }
    }

    void BaseGraphControl::OnPaste()
    {
        if (!graph_ || !document_)
            return;

        const QPointF pos = view_->mapToScene(mapFromGlobal(QCursor::pos()));
        const bool useMouse = underMouse();

        std::vector<GraphNode*> editables;
        QClipboard* clip = QApplication::clipboard();
        if (const QMimeData* data = clip->mimeData())
        {
            QByteArray array = data->data(document_->GetGraphClipboardID());
            if (array.size() > 0)
            {
                SprueEngine::VectorBuffer buff(array.data(), array.size());
                GraphCommands::PasteCmd* cmd = new GraphCommands::PasteCmd(document_, graph_, buff, pos, useMouse);
                Global_DocumentManager()->GetActiveDocument()->GetUndoStack()->push(cmd);
                //unsigned ct = buff.ReadUInt();
                //while (ct)
                //{
                //    if (GraphNode* deserialized = Context::GetInstance()->Deserialize<GraphNode>(&buff))
                //    {
                //        editables.push_back(deserialized);
                //        // Paste is a NEW instance, so the object's need to be made unique or they'll map erroneously
                //        deserialized->MakeUnique();
                //    }
                //    --ct;
                //}
                //
                //graph_->ReconstructLocalConnectivity(editables, &buff);
            }
        }

        //float minX = FLT_MAX;
        //float maxX = FLT_MIN;
        //float minY = FLT_MAX;
        //float maxY = FLT_MIN;
        //if (editables.size() > 0)
        //{
        //    for (auto editable : editables)
        //    {
        //        minX = std::min(minX, editable->XPos);
        //        maxX = std::max(maxX, editable->XPos);
        //        minY = std::min(minY, editable->YPos);
        //        maxY = std::max(maxY, editable->YPos);
        //    }
        //    QRectF rect(minX, minY, maxX - minX, maxY - minY);
        //    QPointF centroid = rect.center();
        //    QPointF relPoint = rect.topLeft();
        //
        //    for (auto editable : editables)
        //    {
        //        QPointF naturalPos(editable->XPos, editable->YPos);
        //        if (useMouse && editables.size() > 1)
        //            naturalPos = (naturalPos - relPoint) + pos;
        //        else if (useMouse && editables.size() == 1)
        //            naturalPos = pos;
        //        else
        //            naturalPos += QPointF(10, 10);
        //
        //        editable->XPos = naturalPos.x();
        //        editable->YPos = naturalPos.y();
        //        graph_->AddNode(editable, false);
        //    }
        //    RebuildConnectors();
        //}
    }

    void BaseGraphControl::OnCut()
    {
        if (graph_)
        {
            OnCopy();
            DeleteSelected();
        }
    }

    void BaseGraphControl::keyReleaseEvent(QKeyEvent* evt)
    {
        // Check for zoom adjustments
        if (evt->key() == Qt::Key::Key_Plus || evt->key() == Qt::Key::Key_Equal)
        {
            IncrementZoom(1);
            evt->accept();
        }
        else if (evt->key() == Qt::Key::Key_Minus)
        {
            IncrementZoom(-1);
            evt->accept();
        }
        else if (QApplication::keyboardModifiers() & Qt::KeyboardModifier::ShiftModifier)
        {
            if (evt->key() == Qt::Key::Key_Up)
                IncrementZoom(1);
            else if (evt->key() == Qt::Key::Key_Down)
                IncrementZoom(-1);
            evt->accept();
        }

        // Handle deletion
        if (evt->key() == Qt::Key::Key_Delete || evt->key() == Qt::Key::Key_Backspace)
        {
            DeleteSelected();
            evt->accept();
        }

        if (evt->key() == Qt::Key::Key_Space && QApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier)
        {
            QuickActionDlg::Activate((GraphGraphicsView*)view_);
        }
        //ScrollAreaWidget::keyReleaseEvent(evt);
    }

    void BaseGraphControl::contextMenuEvent(QContextMenuEvent* evt)
    {
        if (document_ && graph_)
        {
            nodesEditor_->haltConnection();
            QPointF pt = view_->mapToScene(view_->mapFromGlobal(evt->globalPos()));
            QGraphicsItem* item = nodesEditor_->itemAt(pt);
            if (QMenu* menu = document_->GetContextMenu(view_, item, pt, standardActions_))
                menu->exec(evt->globalPos());
        }
    }

    void BaseGraphControl::dragMoveEvent(QDragMoveEvent* event)
    {
        if (document_)
        {
            for (auto strat : strategies_)
                if (strat->RelevantForDocument(document_))
                    strat->dragMoveEvent(event);
        }
        else
            ScrollAreaWidget::dragMoveEvent(event);
    }

    void BaseGraphControl::dragEnterEvent(QDragEnterEvent* event)
    {
        if (document_)
        {
            for (auto strat : strategies_)
                if (strat->RelevantForDocument(document_))
                    strat->dragEnterEvent(event);
        }
        else
            ScrollAreaWidget::dragEnterEvent(event);
    }

    void BaseGraphControl::dropEvent(QDropEvent* event)
    {
        if (document_)
        {
            for (auto strat : strategies_)
                if (strat->RelevantForDocument(document_))
                    strat->dropEvent(event);
        }
        else
            ScrollAreaWidget::dropEvent(event);
    }

    void BaseGraphControl::CollectSearchResults(const QStringList& searchTerms, SearchResultVector& results, bool caseSensitive, bool exactMatch) const
    {
        auto items = view_->scene()->items();
        for (auto item : items)
        {
            if (item && item->type() == QNEBlock::Type)
            {
                QNEBlock* block = (QNEBlock*)item;
                SprueEngine::GraphNode* node = ((SprueEngine::GraphNode*)block->getUserData());
                int hitCt = 0;
                for (auto term : searchTerms)
                {
                    QStringMatcher matcher(term, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
                    int start = 0;
                    do {
                        start = matcher.indexIn(node->name.c_str(), start);
                        if (start != -1)
                        {
                            ++hitCt;
                            ++start;
                        }
                    } while (start != -1);
                }
                if (hitCt > 0)
                    results.push_back(std::make_shared<GraphNodeSearchResult>(const_cast<BaseGraphControl*>(this), std::make_shared<GraphNodeDataSource>(node), node->name.c_str(), hitCt));
            }
        }
    }

    void BaseGraphControl::UpdateGraphNodePositions()
    {
        for (auto item : view_->scene()->items())
        {
            if (QNEBlock* block = dynamic_cast<QNEBlock*>(item))
            {
                if (block->getUserData())
                {
                    GraphNode* node = (GraphNode*)block->getUserData();
                    node->XPos = block->scenePos().x();
                    node->YPos = block->scenePos().y();
                }
            }
        }
    }

    void BaseGraphControl::DeleteSelected()
    {
        // Remove selected connections first
        auto items = view_->scene()->selectedItems();
        for (auto child : items)
        {
            if (child && child->type() == QNEConnection::Type)
            {
                if (QNEConnection* conn = dynamic_cast<QNEConnection*>(child))
                {
                    if (conn->isSelected())
                    {
                        bool allowed = true;
                        AllowDelete(allowed, conn);
                        if (allowed)
                            ConnectionDeleted(conn->port1(), conn->port2());
                    }
                }
            }
        }
        // Remove selected nodes next
        items = view_->scene()->selectedItems();
        for (auto child : items)
        {
            if (child && child->type() == QNEBlock::Type)
            {
                if (QNEBlock* block = dynamic_cast<QNEBlock*>(child))
                {
                    if (block->isSelected())
                    {
                        bool canDelete = true;
                        AllowDelete(canDelete, block);
                        if (canDelete)
                            NodeDeleted(block);
                    }
                }
            }
        }
    }

    void BaseGraphControl::RebuildConnectors(GraphDocument* doc, SprueEngine::Graph* graph)
    {
        if (!graph || !doc)
            return;

        auto sceneItems = doc->GetScene()->items();
        for (auto item : sceneItems)
        {
            if (QNEConnection* conn = dynamic_cast<QNEConnection*>(item))
            {
                doc->GetScene()->removeItem(conn);
                delete conn;
            }
        }

        for (auto record : graph->GetUpstreamEdges())
        {
            QNEBlock* toBlock = doc->GetNodeToBlockTable()[record.first->node];
            QNEBlock* fromBlock = doc->GetNodeToBlockTable()[record.second->node];

            if (!toBlock || !fromBlock)
                continue;

            QNEPort* toPort = toBlock->findPort(record.first);
            QNEPort* fromPort = fromBlock->findPort(record.second);

            if (!toPort || !fromPort)
                continue;

            QNEConnection* conn = new QNEConnection();
            conn->setPort1(fromPort);
            conn->setPort2(toPort);
            conn->updatePosFromPorts();
            conn->updatePath();
            doc->GetScene()->addItem(conn);
        }
    }

    void BaseGraphControl::RebuildConnectors()
    {
        if (!graph_)
            return;

        auto sceneItems = view_->scene()->items();
        for (auto item : sceneItems)
        {
            if (QNEConnection* conn = dynamic_cast<QNEConnection*>(item))
            {
                view_->scene()->removeItem(conn);
                delete conn;
            }
        }

        for (auto record : graph_->GetUpstreamEdges())
        {
            QNEBlock* toBlock = document_->GetNodeToBlockTable()[record.first->node];
            QNEBlock* fromBlock = document_->GetNodeToBlockTable()[record.second->node];

            if (!toBlock || !fromBlock)
                continue;

            QNEPort* toPort = toBlock->findPort(record.first);
            QNEPort* fromPort = fromBlock->findPort(record.second);

            if (!toPort || !fromPort)
                continue;

            QNEConnection* conn = new QNEConnection();
            conn->setPort1(fromPort);
            conn->setPort2(toPort);
            conn->updatePosFromPorts();
            conn->updatePath();
            view_->scene()->addItem(conn);
        }
        view_->update();
        //view_->repaint();
    }

    BaseGraphControl::ViewPosition BaseGraphControl::GetViewPosition()
    {
        ViewPosition ret = ((GraphGraphicsView*)view_)->GetPosition();
        ret.valid_ = true;
        ret.zoomLevel_ = currentZoom_;
        return ret;
    }

    void BaseGraphControl::SetViewPosition(const BaseGraphControl::ViewPosition& pos)
    {
        if (!pos.valid_)
            return;
        currentZoom_ = pos.zoomLevel_;
        ((GraphGraphicsView*)view_)->SetPosition(pos);
    }

    std::vector<QAction*> BaseGraphControl::CreateNodeActions()
    {
        if (document_)
            return document_->CreateNodeActions(this->view_);
        return std::vector<QAction*>();
    }
}