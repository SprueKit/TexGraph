#include "ApplicationCore.h"

#include "DocumentManager.h"
#include "Commands/RegisteredAction.h"

#include <QTimer>

ApplicationCore* ApplicationCore::instance_ = 0x0;

ApplicationCore::ApplicationCore(QMainWindow* mainWindow) 
{
    instance_ = this;
    mainWindow_ = mainWindow;
    documentManager_ = new DocumentManager();

    // Run a CCmdUI analog
    cCmdUITimer_ = new QTimer();
    cCmdUITimer_->setSingleShot(false);
    cCmdUITimer_->connect(cCmdUITimer_, &QTimer::timeout, []() {
        RegisteredAction::DoCmdUIChecks();
    });
    // Non-intuitive, 0 means it will update with every single application level notify
    // Basically the only way to setup polling in QT
    cCmdUITimer_->start(0);
}

void ApplicationCore::SetStatusBarMessage(const QString& msg)
{

}

Selectron* ApplicationCore::GetObjectSelectron()
{
    return ApplicationCore::GetInstance()->documentManager_->GetSelectron();
}