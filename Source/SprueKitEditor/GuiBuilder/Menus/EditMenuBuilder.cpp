#include "EditMenuBuilder.h"

#include "../../GlobalAccess.h"
#include "../../SprueKitEditor.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Commands/RegisteredAction.h>

#include <QAction>
#include <QMenu>
#include <QShortcut>
#include <QKeySequence>
#include <QString>
#include <QWidget>
#include <QSpiNBox>

namespace SprueEditor
{

    void EditMenuBuilder::Build(SprueKitEditor* editor)
    {
        QMenu* menu = editor->GetOrCreateMenu("EDIT_MENU", "&Edit");

        RegisteredAction* undoAct = new RegisteredAction("Edit", "Undo");
        undoAct->SetShortcut(QKeySequence::Undo);
        connect(undoAct, &QAction::triggered, [=](bool) {
            if (DocumentBase* opened = Global_DocumentManager()->GetActiveDocument())
                if (opened->GetUndoStack()->CanUndo())
                    opened->GetUndoStack()->Undo();
        });

        RegisteredAction* redoAct = new RegisteredAction("Edit", "Redo");
        redoAct->SetShortcut(QKeySequence::Redo);
        connect(redoAct, &QAction::triggered, [=](bool) {
            if (DocumentBase* opened = Global_DocumentManager()->GetActiveDocument())
                if (opened->GetUndoStack()->CanRedo())
                    opened->GetUndoStack()->Redo();
        });

        

        RegisteredAction* cutAct = new RegisteredAction("Edit", "Cut", editor);
        cutAct->setIcon(QIcon(":/Images/shared/Cut.png"));
        cutAct->SetShortcut(QKeySequence::Cut);
        RegisteredAction* copyAct = new RegisteredAction("Edit", "Copy", editor);
        copyAct->setIcon(QIcon(":/Images/shared/Copy.png"));
        copyAct->SetShortcut(QKeySequence::Copy);
        RegisteredAction* pasteAct = new RegisteredAction("Edit", "Paste", editor);
        pasteAct->setIcon(QIcon(":/Images/shared/Paste.png"));
        pasteAct->SetShortcut(QKeySequence::Paste);

        RegisteredAction* selectNoneAct = new RegisteredAction("Edit", "Clear Selection");
        selectNoneAct->SetShortcut(QKeySequence("CTRL+D"));

        connect(cutAct, &QAction::triggered, [=](bool state) {
            if (editor->focusWidget())
            {
                if (auto child = editor->focusWidget()->findChild<QLineEdit*>())
                    child->cut();
                else if (editor->focusWidget()->metaObject()->indexOfMethod("cut()") >= 0)
                    const_cast<QMetaObject*>(editor->focusWidget()->metaObject())->invokeMethod(editor->focusWidget(), "cut");
            }
        });
        connect(copyAct, &QAction::triggered, [=](bool state) {
            if (editor->focusWidget())
            {
                if (auto child = editor->focusWidget()->findChild<QLineEdit*>())
                    child->copy();
                else if (editor->focusWidget()->metaObject()->indexOfMethod("copy()") >= 0)
                    const_cast<QMetaObject*>(editor->focusWidget()->metaObject())->invokeMethod(editor->focusWidget(), "copy");
            }
        });
        connect(pasteAct, &QAction::triggered, [=](bool state) {
            if (editor->focusWidget())
            {
                if (auto child = editor->focusWidget()->findChild<QLineEdit*>())
                    child->paste();
                else if (editor->focusWidget()->metaObject()->indexOfMethod("paste()") >= 0)
                    const_cast<QMetaObject*>(editor->focusWidget()->metaObject())->invokeMethod(editor->focusWidget(), "paste");
            }
        });

        connect(selectNoneAct, &QAction::triggered, [=](bool state) {
            if (editor && editor->GetObjectSelectron())
                editor->GetObjectSelectron()->ClearSelection(0x0);
        });


        menu->addAction(undoAct);
        menu->addAction(redoAct);
        menu->addSeparator();
        menu->addAction(cutAct);
        menu->addAction(copyAct);
        menu->addAction(pasteAct);

        connect(menu, &QMenu::aboutToShow, [=]() {
            if (DocumentBase* opened = Global_DocumentManager()->GetActiveDocument())
            {
                if (opened->GetUndoStack()->CanUndo())
                {
                    undoAct->setEnabled(true);
                    undoAct->setText(QString("Undo '%1'").arg(opened->GetUndoStack()->GetUndoText()));
                }
                else
                {
                    undoAct->setEnabled(false);
                    undoAct->setText(tr("Undo"));
                }

                if (opened->GetUndoStack()->CanRedo())
                {
                    redoAct->setEnabled(true);
                    redoAct->setText(QString("Redo '%1'").arg(opened->GetUndoStack()->GetRedoText()));
                }
                else
                {
                    redoAct->setEnabled(false);
                    redoAct->setText(tr("Redo"));
                }
            }
            else
            {
                undoAct->setEnabled(false);
                redoAct->setEnabled(false);
            }
        });
    }

}