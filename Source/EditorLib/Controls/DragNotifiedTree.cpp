#include "DragNotifiedTree.h"

#include <QDropEvent>
#include <QScroller>

class DragNotifiedTreeSearchResult : public SearchResult
{
public:
    DragNotifiedTreeSearchResult(QTreeWidget* widget, QTreeWidgetItem* item, QString text, int hitCt) :
        widget_(widget),
        item_(item)
    {
        text_ = text;
        hitCount_ = hitCt;
        source_ = widget->accessibleName();
    }

    virtual void GoTo() {
        if (!widget_->hasFocus())
            widget_->setFocus();
        widget_->setItemSelected(item_, true);
    }

    QTreeWidget* widget_;
    QTreeWidgetItem* item_;
};

DragNotifiedTree::DragNotifiedTree() :
    QTreeWidget()
{
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
    setDropIndicatorShown(true);
    setDefaultDropAction(Qt::DropAction::TargetMoveAction);
    QScroller::grabGesture(this, QScroller::TouchGesture);
}

void DragNotifiedTree::dragEnterEvent(QDragEnterEvent* event)
{
    //QTreeWidget::dragEnterEvent(event);
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) {  // just in case
        event->setDropAction(Qt::IgnoreAction);
        return;
    }
    
    DropIndicatorPosition pos = dropIndicatorPosition();
    if (pos == DropIndicatorPosition::OnItem || pos == DropIndicatorPosition::AboveItem || pos == DropIndicatorPosition::BelowItem)
    {
        event->setDropAction(Qt::DropAction::MoveAction);
        event->acceptProposedAction();
    }
}

void DragNotifiedTree::dragMoveEvent(QDragMoveEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) {  // just in case
        event->setDropAction(Qt::IgnoreAction);
        return;
    }

    bool anyCanDrop = false;
    QList<QTreeWidgetItem*> dragging = ((QTreeWidget*)event->source())->selectedItems();
    QTreeWidgetItem* dropItem = itemFromIndex(index);

    if (dropItem)
    {
        DropIndicatorPosition pos = dropIndicatorPosition();
        if (pos == DropIndicatorPosition::OnItem)
        {
            QRect itemRect = visualItemRect(dropItem);
            for (auto item : dragging)
            {
                if (!CanDropOn(dropItem, item))
                    continue;
                anyCanDrop = true;
            }
        }
        else if (pos == DropIndicatorPosition::AboveItem)
        {
            if (QTreeWidgetItem* parentItem = dropItem->parent())
            {
                int idx = parentItem->indexOfChild(dropItem);
                if (idx >= 0)
                {
                    for (int i = 0, ct = 0; i < dragging.size(); ++i)
                    {
                        QTreeWidgetItem* item = dragging[i];
                        if (!CanDropOn(parentItem, item))
                            continue;
                        anyCanDrop = true;
                    }
                }
            }
        }
        else if (pos == DropIndicatorPosition::BelowItem)
        {
            if (QTreeWidgetItem* parentItem = dropItem->parent())
            {
                int idx = parentItem->indexOfChild(dropItem) + 1;
                if (idx >= 0 && idx < parentItem->childCount() - 1)
                {
                    for (int i = 0, ct = 0; i < dragging.size(); ++i)
                    {
                        QTreeWidgetItem* item = dragging[i];
                        if (!CanDropOn(parentItem, item))
                            continue;
                        anyCanDrop = true;
                    }
                }
                else if (idx >= 0)
                {
                    for (auto item : dragging)
                    {
                        if (!CanDropOn(parentItem, item))
                            continue;
                        anyCanDrop = true;
                    }
                }
            }
        }
    }

    if (!anyCanDrop)
        event->setDropAction(Qt::IgnoreAction);
    else
    {
        event->setDropAction(Qt::MoveAction);
        event->acceptProposedAction();
        //event->setAccepted(true);
    }
}

void DragNotifiedTree::dropEvent(QDropEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) {  // just in case
        event->setDropAction(Qt::IgnoreAction);
        return;
    }
    QList<QTreeWidgetItem*> dragging = ((QTreeWidget*)event->source())->selectedItems();
    QTreeWidgetItem* dropItem = itemFromIndex(index);

    if (dropItem)
    {
        DropIndicatorPosition pos = dropIndicatorPosition();
        if (pos == DropIndicatorPosition::OnItem)
        {
            QRect itemRect = visualItemRect(dropItem);
            for (auto item : dragging)
            {
                if (!CanDropOn(dropItem, item))
                    continue;

                if (item->parent())
                    item->parent()->removeChild(item);
                dropItem->addChild(item);
                ItemDroppedOn(dropItem, item);
                SortChildren(dropItem);
            }
        }
        else if (pos == DropIndicatorPosition::AboveItem)
        {
            if (QTreeWidgetItem* parentItem = dropItem->parent())
            {
                int idx = parentItem->indexOfChild(dropItem);
                if (idx >= 0)
                {
                    for (int i = 0, ct = 0; i < dragging.size(); ++i)
                    {
                        QTreeWidgetItem* item = dragging[i];
                        if (!CanDropOn(parentItem, item))
                            continue;
                        if (QTreeWidgetItem* parent = item->parent())
                            parent->removeChild(item);
                        parentItem->insertChild(idx + ct, item);
                        ItemDroppedAt(parentItem, idx + ct, item);
                        SortChildren(parentItem);
                        ++ct;
                    }
                }
            }
        }
        else if (pos == DropIndicatorPosition::BelowItem)
        {
            if (QTreeWidgetItem* parentItem = dropItem->parent())
            {
                int idx = parentItem->indexOfChild(dropItem) + 1;
                if (idx >= 0 && idx < parentItem->childCount() - 1)
                {
                    for (int i = 0, ct = 0; i < dragging.size(); ++i)
                    {
                        QTreeWidgetItem* item = dragging[i];
                        if (!CanDropOn(parentItem, item))
                            continue;

                        if (item->parent())
                            item->parent()->removeChild(item);
                        parentItem->insertChild(idx + ct, item);
                        ItemDroppedAt(parentItem, idx + ct, item);
                        SortChildren(parentItem);
                        ++ct;
                    }
                }
                else if (idx >= 0)
                {
                    for (auto item : dragging)
                    {
                        if (!CanDropOn(parentItem, item))
                            continue;
                        if (item->parent())
                            item->parent()->removeChild(item);
                        parentItem->addChild(item);
                        ItemDroppedAt(parentItem, idx, item);
                        SortChildren(parentItem);
                    }
                }
            }
        }
    }

    //base::dropEvent(event);
}

void DragNotifiedTree::CollectSearchResults(const QStringList& searchTerms, SearchResultVector& results, bool caseSensitive, bool exactMatch) const
{
    DragNotifiedTree* self = const_cast<DragNotifiedTree*>(this);
    QTreeWidgetItemIterator it(self);
    while (*it)
    {
        QString txt = (*it)->text(0);
        int hitCt = 0;
        for (auto term : searchTerms)
            if (txt.contains(term, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive))
                ++hitCt;

        if (hitCt > 0)
            results.push_back(std::shared_ptr<SearchResult>(new DragNotifiedTreeSearchResult(self, *it, txt, hitCt)));

        ++it;
    }
}