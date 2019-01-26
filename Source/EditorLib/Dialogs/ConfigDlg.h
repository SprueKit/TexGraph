#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/Settings/Settings.h>

#include <QDialog>

class EDITORLIB_EXPORT ConfigDlg : public QDialog
{
    Q_OBJECT
protected:
    ConfigDlg(Settings*);

public:
    virtual ~ConfigDlg();

    static void Show(Settings* settings);

    static QWidget* GenerateControls(SettingsPage* page, SettingValue* value);

private:
    static ConfigDlg* instance_;
};