#include "LayoutMenuBuilder.h"

#include "../../SprueKitEditor.h"
#include "../../QtHelpers.h"

#include <EditorLib/Commands/RegisteredAction.h>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include <SprueEngine/FString.h>

namespace SprueEditor
{

void LayoutMenuBuilder::Build(SprueKitEditor* editor)
{

    QMenu* menu = editor->GetOrCreateMenu("LAYOUT_MENU", "&Layout");

    QAction* saveLayout = new RegisteredAction("User Interface", "Save Layout");
    saveLayout->setIcon(QIcon(":/Images/shared/SaveLayout.png"));
    connect(saveLayout, &QAction::triggered, [=](bool) {
        QInputDialog dlg;
        dlg.setWindowTitle("Save Layout");
        dlg.setLabelText("Name");
        dlg.setFixedWidth(640);
        dlg.setMinimumWidth(640);
        dlg.setModal(true);
        if (dlg.exec())
        {
            QString name = dlg.textValue();
            if (!name.isEmpty())
                editor->SaveLayout(name.toStdString(), FString("%1.layout", name.toStdString()).str());
        }
    });

    QAction* restoreLayout = new RegisteredAction("User Interface", "Restore Layout");
    restoreLayout->setIcon(QIcon(":/Images/shared/RestoreLayout.png"));
    connect(restoreLayout, &QAction::triggered, [=](bool) {
        QString path = QFileDialog::getOpenFileName(0x0, "Load Layout", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation), "UI Layouts (*.layout)");
        if (!path.isEmpty())
            editor->RestoreLayout(path.toStdString());
    });

    menu->addAction(saveLayout);
    menu->addAction(restoreLayout);
    menu->addSeparator();

    QMenu* layouts = new QMenu("Layouts");
    menu->addMenu(layouts);

    connect(layouts, &QMenu::aboutToShow, [=]() {
        layouts->clear();

        QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        if (dir.exists())
        {
            for (auto entry : dir.entryInfoList())
            {
                if (entry.isFile())
                {
                    if (entry.completeSuffix().compare("layout") == 0)
                    {
                        if (entry.baseName().compare("PREVIOUS_LAYOUT") == 0)
                            continue;

                        QAction* act = new QAction(entry.baseName());
                        connect(act, &QAction::triggered, [=](bool) {
                            editor->RestoreLayout(entry.path().toStdString());
                        });
                        layouts->addAction(act);
                    }
                }
            }
        }
    });


    QMenu* dockCornersMenu = new QMenu("Docking Corners");
    menu->addMenu(dockCornersMenu);

    QAction* bottomLeftCornerMenu = new RegisteredAction("User Interface", "Toggle Dock Bottom Left Corner");
    bottomLeftCornerMenu->setIcon(QIcon(":/Images/shared/ToggleDockBottomLeft.png"));
    dockCornersMenu->addAction(bottomLeftCornerMenu);
    connect(bottomLeftCornerMenu, &QAction::triggered, [=](bool) {
        if (editor->GetDockingWindow()->corner(Qt::BottomLeftCorner) == Qt::LeftDockWidgetArea)
            editor->GetDockingWindow()->setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
        else
            editor->GetDockingWindow()->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    });

    QAction* bottomRightCornerMenu = new RegisteredAction("User Interface", "Toggle Dock Bottom Right Corner");
    bottomRightCornerMenu->setIcon(QIcon(":/Images/shared/ToggleDockBottomRight.png"));
    dockCornersMenu->addAction(bottomRightCornerMenu);
    connect(bottomRightCornerMenu, &QAction::triggered, [=](bool) {
        if (editor->GetDockingWindow()->corner(Qt::BottomRightCorner) == Qt::RightDockWidgetArea)
            editor->GetDockingWindow()->setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
        else
            editor->GetDockingWindow()->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    });

    QAction* topLeftCornerMenu = new RegisteredAction("User Interface", "Toggle Dock Top Left Corner");
    topLeftCornerMenu->setIcon(QIcon(":/Images/shared/ToggleDockTopLeft.png"));
    topLeftCornerMenu->setChecked(editor->corner(Qt::TopLeftCorner) == Qt::LeftDockWidgetArea);
    dockCornersMenu->addAction(topLeftCornerMenu);
    connect(topLeftCornerMenu, &QAction::triggered, [=](bool) {
        if (editor->GetDockingWindow()->corner(Qt::TopLeftCorner) == Qt::LeftDockWidgetArea)
            editor->GetDockingWindow()->setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
        else
            editor->GetDockingWindow()->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    });

    QAction* topRightCornerMenu = new RegisteredAction("User Interface", "Toggle Dock Top Right Corner");
    topRightCornerMenu->setIcon(QIcon(":/Images/shared/ToggleDockTopRight.png"));
    dockCornersMenu->addAction(topRightCornerMenu);
    connect(topRightCornerMenu, &QAction::triggered, [=](bool) {
        if (editor->GetDockingWindow()->corner(Qt::TopRightCorner) == Qt::RightDockWidgetArea)
            editor->GetDockingWindow()->setCorner(Qt::TopRightCorner, Qt::TopDockWidgetArea);
        else
            editor->GetDockingWindow()->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    });
}

}