#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/Dialogs/FastCommandDlg.h>

/// Interface for widgets to use to add their own custom actions to appear in the QuickAction menu
class EDITORLIB_EXPORT IQuickActionSource
{
public:
    virtual std::vector<QAction*> GetActions() const = 0;
};

/// Specialized version of the FastCommand dialog that is intended for working with 'RegisteredAction' to provide
/// a "Quick Action" dialog where commands can be entered via keyboard (or list selection)
class EDITORLIB_EXPORT QuickActionDlg : public FastCommandDlg
{
public:
    static void Activate(IQuickActionSource* source = 0x0);
    virtual ~QuickActionDlg() { }
private:
    QuickActionDlg() : FastCommandDlg() { }

protected:
    virtual void Fill(QStringList&) override;
    virtual void Selected(const QString&) override;

private:
    static QuickActionDlg* instance_;
    IQuickActionSource* qaSource_ = 0x0;
    std::map<QString, QAction*> actions_;
};