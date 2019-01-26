#include "UndoListWidget.h"

UndoListWidget::UndoListWidget()
{
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    connect(this, &QListWidget::itemSelectionChanged, this, &UndoListWidget::SelectionChanged);
}

UndoListWidget::~UndoListWidget()
{

}

void UndoListWidget::SetUndoStack(SmartUndoStack* newStack)
{
    if (newStack == stack_)
        return;

    if (stack_)
    {
        disconnect(stack_, &SmartUndoStack::ActionRedone, this, &UndoListWidget::ActionUndone);
        disconnect(stack_, &SmartUndoStack::ActionRedone, this, &UndoListWidget::ActionRedone);
        disconnect(stack_, &SmartUndoStack::ActionBounced, this,&UndoListWidget::ActionBounced);
        disconnect(stack_, &SmartUndoStack::ActionMerged, this, &UndoListWidget::ActionMerged);
        disconnect(stack_, &SmartUndoStack::ActionAdded, this,  &UndoListWidget::ActionAdded);
    }
    stack_ = newStack;
    FillControl();
    if (stack_)
    {
        connect(stack_, &SmartUndoStack::ActionUndone, this, &UndoListWidget::ActionUndone);
        connect(stack_, &SmartUndoStack::ActionRedone, this, &UndoListWidget::ActionRedone);
        connect(stack_, &SmartUndoStack::ActionBounced, this,&UndoListWidget::ActionBounced);
        connect(stack_, &SmartUndoStack::ActionMerged, this, &UndoListWidget::ActionMerged);
        connect(stack_, &SmartUndoStack::ActionAdded, this,  &UndoListWidget::ActionAdded);

        // Make sure we're on the correct row
        blockSignals(true);
        setCurrentRow(stack_->GetCurrent() + 1); // need +1 to account for "< empty >" item.
        blockSignals(false);
    }

    emit UndoStackChanged(stack_);
}

void UndoListWidget::ActionAdded(SmartCommand* cmd)
{
    blockSignals(true);
    FillControl();
    blockSignals(false);
}

void UndoListWidget::ActionBounced(SmartCommand* cmd)
{
    blockSignals(true);
    for (int row = 0; row < count(); row++)
    {
        QListWidgetItem* cur = item(row);
        SmartCommand* thisCmd = ((SmartCommand*)cur->data(Qt::UserRole).value<void*>());
        if (thisCmd == cmd)
            delete cur;
    }
    ActionUndone();
    blockSignals(false);
}

void UndoListWidget::ActionMerged(SmartCommand* cmd)
{
    blockSignals(true);
    for (int row = 0; row < count(); row++)
    {
        QListWidgetItem* cur = item(row);
        SmartCommand* thisCmd = ((SmartCommand*)cur->data(Qt::UserRole).value<void*>());
        if (thisCmd == cmd)
        {
            cur->setText(thisCmd->GetText());
            break;
        }
    }
    ActionUndone();
    blockSignals(false);
}

void UndoListWidget::IndexChanged(int index)
{
    blockSignals(true);

    blockSignals(false);
}

void UndoListWidget::ActionUndone()
{
    if (!stack_)
        return;
    blockSignals(true);
    if (QListWidgetItem* it = item(stack_->GetCurrent() + 1))
        it->setSelected(true);
    else
        item(0)->setSelected(true);
    blockSignals(false);
}

void UndoListWidget::ActionRedone()
{
    ActionUndone();
}

void UndoListWidget::FillControl()
{
    blockSignals(true);
    clear();
    blockSignals(false);

    if (stack_)
    {
        QListWidgetItem* emptyItem = new QListWidgetItem(tr("< empty >"));
        addItem(emptyItem);
        for (int i = 0; i < stack_->GetCount(); ++i)
        {
            SmartCommand* cmd = stack_->GetCommand(i);
            QListWidgetItem* item = new QListWidgetItem(cmd->GetText());
            QVariant data;
            data.setValue<void*>(cmd);
            item->setData(Qt::UserRole, data);
            addItem(item);

            if (stack_->GetCurrent() == i)
                item->setSelected(true);
        }
    }
}

void UndoListWidget::SelectionChanged()
{
    auto indices = selectedIndexes();
    if (indices.size())
    {
        lastSelected_ = item(indices[0].row());
        stack_->SetCurrent(indices[0].row() - 1);
    }
    else
        lastSelected_->setSelected(true);
}