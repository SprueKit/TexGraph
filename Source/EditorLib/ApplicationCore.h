#pragma once

#include <EditorLib/editorlib_global.h>

#include <QMainWindow>
#include <EditorLib/DocumentManager.h>

class QTimer;

class Ribbon;
class Selectron;

class EDITORLIB_EXPORT ApplicationCore
{
public:
    ApplicationCore(QMainWindow* mainWindow);

    static ApplicationCore* GetInstance() { return instance_; }
    static QMainWindow* GetMainWindow() { return GetInstance()->mainWindow_; }
    static DocumentManager* GetDocumentManager() { return GetInstance()->documentManager_; }
    static Ribbon* GetRibbon() { return GetInstance()->ribbon_; }
    static void SetRibbon(Ribbon* rib) { GetInstance()->ribbon_ = rib; }
    static void SetStatusBarMessage(const QString& msg);
    static Selectron* GetObjectSelectron();

private:
    static ApplicationCore* instance_;
    QMainWindow* mainWindow_ = 0x0;
    DocumentManager* documentManager_ = 0x0;
    Ribbon* ribbon_ = 0x0;
    // We can't actually do a proper CCmdUI in QT, although a custom event could be sent MFC's CCmdUI has special behaviour
    QTimer* cCmdUITimer_ = 0x0;
};