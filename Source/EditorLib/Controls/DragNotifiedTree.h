#pragma once

#include <EditorLib/Controls/ISignificantControl.h>
#include <EditorLIb/Search/ISearchable.h>

#include <QTreeWidget>

#include <map>

/// QTreeWidget implementation that handles drag and drop itself
class DragNotifiedTree : public QTreeWidget, public ISignificantControl, public ISearchable
{
    Q_OBJECT;
public:
    DragNotifiedTree();
    virtual ~DragNotifiedTree() { }

    using QTreeWidget::itemFromIndex;

    /// Implements ISearchable for searching through items. "Accessible name" will be used for the "source" field of the search result.
    virtual void CollectSearchResults(const QStringList& searchTerms, SearchResultVector& results, bool caseSensitive = false, bool exactMatch = false) const override;

    /// Helper function for collection expansion status of all QTreeWidgetItems contained mapped by the stored user data type.
    /// T parameter should be the typed stored in column 0 for Qt::UserRole.
    template<class T>
    void CollectExpansionStates(std::map<T*, bool>& states)
    {
        QTreeWidgetItemIterator it(self);
        while (*it)
            states.insert((*it)->data(0, Qt::UserRole).value<T*>(), (*it)->isExpanded());
    }

    /// Helper function for restoration of expansion status of all QTreeWidgetItems contained mapped by the stored user data type.
    /// T parameter should be the typed stored in column 0 for Qt::UserRole.
    template<class T>
    void RestoreExpansionStates(const std::map<T*, bool>& states)
    {
        QTreeWidgetItemIterator it(self);
        while (*it)
        {
            auto found = states.find((*it)->data(0, Qt::UserRole).value<T*>());
            if (found != states.end())
                (*it)->setExpanded(found->second);
        }
    }

protected:
    virtual Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE { return Qt::MoveAction | Qt::CopyAction; }
    virtual void dragEnterEvent(QDragEnterEvent*) Q_DECL_OVERRIDE;
    virtual void dragMoveEvent(QDragMoveEvent*) Q_DECL_OVERRIDE;
    virtual void dropEvent(QDropEvent*) Q_DECL_OVERRIDE;

    /// Implement this method to determine what items can drop onto what parent items
    virtual bool CanDropOn(QTreeWidgetItem* parent, QTreeWidgetItem* child) = 0;
    /// Implement this method to handle dropping an item onto another
    virtual void ItemDroppedOn(QTreeWidgetItem* parent, QTreeWidgetItem* child) = 0;
    /// Implement this method to handle dropping an item at a specified index in a parent
    virtual void ItemDroppedAt(QTreeWidgetItem* parent, int index, QTreeWidgetItem* child) = 0;
    /// Whenever items are dragged and dropped this method will be called.
    virtual void SortChildren(QTreeWidgetItem* item) { }
};