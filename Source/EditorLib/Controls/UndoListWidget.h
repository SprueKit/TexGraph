#pragma once

#include <QListWidget>

#include <EditorLib/Commands/SmartUndoStack.h>


class UndoListWidget : public QListWidget
{
    Q_OBJECT
public:
    UndoListWidget();
    virtual ~UndoListWidget();

    void SetUndoStack(SmartUndoStack* newStack);

signals:
    void UndoStackChanged(SmartUndoStack*);

private slots:
    void ActionAdded(SmartCommand*);
    void ActionBounced(SmartCommand*);
    void ActionMerged(SmartCommand*);
    void IndexChanged(int index);
    void ActionUndone();
    void ActionRedone();

    void SelectionChanged();

private:
    void FillControl();

    SmartUndoStack* stack_ = 0x0;
    QListWidgetItem* lastSelected_ = 0x0;
};