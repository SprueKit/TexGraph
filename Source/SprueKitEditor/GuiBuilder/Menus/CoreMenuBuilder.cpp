#include "CoreMenuBuilder.h"

#include "../../GlobalAccess.h"
#include "../../SprueKitEditor.h"

#include <EditorLib/DocumentBase.h>
#include <EditorLib/Dialogs/QuickActionDlg.h>
#include <EditorLib/Commands/RegisteredAction.h>

#include <SprueEngine/FString.h>

#include <QFileDialog>
#include <qmenubar.h>
#include <qtoolbar.h>

namespace SprueEditor
{

    void CoreMenuBuilder::Build(SprueKitEditor* editor)
    {
        auto quickAction_ = new QAction("Quick Action", SprueKitEditor::GetInstance());
        quickAction_->setShortcut(QKeySequence::fromString("Ctrl+Space"));
        quickAction_->setShortcutContext(Qt::ShortcutContext::WindowShortcut);
        quickAction_->setPriority(QAction::Priority::HighPriority);
        connect(quickAction_, &QAction::triggered, [=](bool) {
            QuickActionDlg::Activate();
        });

        auto closeActiveDoc = new RegisteredAction("File", QIcon(":/Images/Close.png"), tr("Close Document"), this);
        connect(closeActiveDoc, &QAction::triggered, [=](bool) {
            Global_DocumentManager()->CloseActiveDocument(true);
        });

        auto saveAction_ = new RegisteredAction("File", QIcon(":/Images/Save.png"), tr("Save ..."));
        saveAction_->setShortcut(QKeySequence::fromString("Ctrl+S"));
        connect(saveAction_, &QAction::triggered, [=](bool) {
            Global_DocumentManager()->SaveActiveDocument();
        });

        auto saveAsAction_ = new RegisteredAction("File", QIcon(":/Images/SaveAs.png"), tr("Save As ..."), this);
        saveAsAction_->setShortcut(QKeySequence::fromString("Ctrl+Shift+S"));
        connect(saveAsAction_, &QAction::triggered, [=](bool) {
            Global_DocumentManager()->SaveActiveDocumentAs();
        });

        auto exitAction_ = new RegisteredAction("File", tr("Exit"));
        exitAction_->setShortcut(QKeySequence::fromString("Alt+F4"));
        connect(exitAction_, &QAction::triggered, [=](bool) {
            editor->close();
        });

        QAction* newAction_ = 0x0;
        QAction* openAction_ = 0x0;

        QMenu* menu = editor->GetOrCreateMenu("file", "&File");
        editor->addAction(quickAction_);
        std::vector<DocumentHandler*> handlers = editor->GetDocumentHandlers();
        // Program only handles one type of document
        if (handlers.size() == 1)
        {
            newAction_ = new RegisteredAction("File", QIcon(":/Images/New.png"), tr("New"), this);
            newAction_->setShortcut(QKeySequence::fromString("Ctrl+N"));
            
            openAction_ = new RegisteredAction("File", QIcon(":/Images/Open.png"), tr("Open ..."), this);
            openAction_->setShortcut(QKeySequence::fromString("Ctrl+O"));
            
            menu->addAction(newAction_);
            menu->addAction(openAction_);

            connect(newAction_, &QAction::triggered, [=](bool) {
                Global_DocumentManager()->CreateNewDocument(handlers[0]);
            });

            connect(openAction_, &QAction::triggered, [=](bool) {
                Global_DocumentManager()->OpenDocument(handlers[0]);
            });
        }
        else // program handles multiple
        {
            //connect(newAction_, &QAction::triggered, [=](bool) {
            //});
            QMenu* newMenu = menu->addMenu(QIcon(":/Images/New.png"), "&New");
            QMenu* openMenu = menu->addMenu(QIcon(":/Images/Open.png"), "&Open");
            for (auto handler : handlers)
            {
                QAction* newAct = new RegisteredAction("File", QString("New %1").arg(handler->DocumentTypeName()));
                QAction* openAct = new RegisteredAction("File", QString("Open %1").arg(handler->DocumentTypeName()));
                newMenu->addAction(newAct);
                openMenu->addAction(openAct);

                connect(newAct, &QAction::triggered, [=](bool) {
                    Global_DocumentManager()->CreateNewDocument(handler);
                });

                connect(openAct, &QAction::triggered, [=](bool) {
                    Global_DocumentManager()->OpenDocument(handler);
                });
            }
        }

        menu->addAction(closeActiveDoc);
        menu->addSeparator();
        menu->addAction(saveAction_);
        menu->addAction(saveAsAction_);
        menu->addSeparator();
        QMenu* recentFiles = menu->addMenu("Recent Files");
        //TODO
        menu->addSeparator();
        menu->addAction(exitAction_);


        QToolBar* toolBar = editor->GetOrCreateToolBar("file", "File Commands");
        // Only add as toolbar commands if single document application
        if (handlers.size() == 1)
        {
            toolBar->addAction(newAction_);
            toolBar->addAction(openAction_);
        }
        toolBar->addAction(saveAction_);
        toolBar->addAction(saveAsAction_);
        toolBar->addAction(exitAction_);
    }

}