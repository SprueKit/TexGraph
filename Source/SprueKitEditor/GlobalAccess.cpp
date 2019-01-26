#include "GlobalAccess.h"

#include "SprueKitEditor.h"

#include <EditorLib/ApplicationCore.h>

#include <QMainWindow>
#include <QMessageBox>

void Global_SetStatusBarMessage(const QString& message, int duration)
{
    SprueEditor::SprueKitEditor::GetInstance()->statusBar()->showMessage(message, duration);
}

SprueEditor::SprueKitEditor* Global_GetEditor()
{
    return SprueEditor::SprueKitEditor::GetInstance();
}

TaskProcessor* Global_MainTaskProcessor()
{
    return SprueEditor::SprueKitEditor::GetInstance()->GetTaskProcessor();
}

TaskProcessor* Global_SecondaryTaskProcessor()
{
    return SprueEditor::SprueKitEditor::GetInstance()->GetSecondaryTaskProcessor();
}

DocumentManager* Global_DocumentManager()
{
    return ApplicationCore::GetDocumentManager();
}

SprueEditor::RenderWidget* Global_RenderWidget()
{
    return SprueEditor::SprueKitEditor::GetInstance()->GetRenderer();
}

SprueEditor::ViewManager* Global_ViewManager()
{
    return SprueEditor::SprueKitEditor::GetInstance()->GetViewManager();
}

Selectron* Global_ObjectSectron()
{
    return ApplicationCore::GetObjectSelectron();//SprueEditor::SprueKitEditor::GetInstance()->GetObjectSelectron();
}

QMainWindow* Global_MainWindow()
{
    return SprueEditor::SprueKitEditor::GetInstance();
}

void Global_ShowErrorMessage(const QString& title, const QString& message)
{
    QMessageBox::critical(0x0, title, message);
}