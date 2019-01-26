#pragma once

#include <EditorLib/editorlib_global.h>

#include <QAction>
#include <QIcon>
#include <QString>

#include <functional>
#include <vector>

class QMenu;

class DocumentBase;
class RegisteredAction;

enum CommandUIState
{
    CUIS_None = 0,
    CUIS_Enabled = 1,
    CUIS_Disabled = 1 << 2,
    CUIS_Checked = 1 << 3,
    CUIS_UnChecked = 1 << 4,
    CUIS_Visible = 1 << 5,
    CUIS_Invisible = 1 << 6
};

typedef CommandUIState(*CCMD_UI)(RegisteredAction*);
typedef std::function<CommandUIState(RegisteredAction*)> CmdUIFunction;

/// DataSources, Documents, and Widgets may implement ICommandUI to deal with command UI changes.
struct EDITORLIB_EXPORT ICommandUI
{
    virtual CommandUIState CommandUI(RegisteredAction*) = 0;
};

/// Preferable to using QAction. All existing instances can be found in a single "AutoList" 
/// which is primarily used for populating the action customization GUI and quick action search
///
/// If QT's menus and toolbars did not depend so immensely on QAction for basically everything 
/// it wouldn't even be used. RegisteredAction wraps around it and is used to make QAction
/// suck a whole lot less.
///
/// Long-term objective: Abandon everything that works with QAction ... everything
class EDITORLIB_EXPORT RegisteredAction : public QAction
{
public:
    /// Construct. Action class is used for grouping commands
    RegisteredAction(const QString& actionClass, const QString& text, QObject* owner = 0x0);
    /// Construct. Action class is used for grouping commands
    RegisteredAction(const QString& actionClass, const QIcon& icon, const QString& text, QObject* owner = 0x0);
    virtual ~RegisteredAction();

    QString GetActionClass() const { return actionClass_; }

    virtual void SetShortcut(QKeySequence sequence) { QAction::setShortcut(sequence); }

    virtual QKeySequence GetShortCut() { return QAction::shortcut(); }

    virtual void Trigger() { QAction::trigger(); }

    static std::vector< RegisteredAction* >& GetList() { return actions_; }
    static RegisteredAction* GetAction(const QString& originalName);

    static void Save();
    static void Restore();

    virtual bool IsDocumentRelevant(DocumentBase* document) { return true; }
    virtual QString GetDocumentType() const { return QString(); }

    QString GetOriginalName() const { return originalName_; }

    virtual bool IsAvailable() const { return isEnabled(); }

    void SetConstantUI(CmdUIFunction cmdUI) { constantCmdUI_ = cmdUI; }
    void AddAnyUI(CmdUIFunction cmdUI) { anyCmdUI_.push_back(cmdUI); }

    /// Does the horrible disambiguation of resolving current UI state without coupling to anything.
    virtual void CheckCmdUI();

    /// Runs all of cmd ui.
    static void DoCmdUIChecks();

    /// Construct a QMenu for display with the contained actions.
    static QMenu* CreateMenu(const std::vector<QString>& actions);
    /// Populate an existing QMenu with the given actions.
    static void Populate(QMenu* target, const std::vector<QString>& actions);

private:
    /// If exists then this is the constant check.
    CmdUIFunction constantCmdUI_;
    /// If any of these return true than the action is enabled.
    std::vector<CmdUIFunction> anyCmdUI_;
    /// Name this action had initially.
    QString originalName_;
    /// Classification of this action into a group.
    QString actionClass_;
    /// List of all actions that exist.
    static std::vector< RegisteredAction* > actions_;
};

class EDITORLIB_EXPORT RegisteredWrapperAction : public RegisteredAction
{
    Q_OBJECT
public:
    RegisteredWrapperAction(const QString& actionClass, QAction* wrapping);
    virtual ~RegisteredWrapperAction() { }

    virtual void SetShortcut(QKeySequence sequence) override {
        if (action_)
            action_->setShortcut(sequence);
    }

    QAction* GetAction() const { return action_; }

    virtual QKeySequence GetShortCut() override { return action_ ? action_->shortcut() : 0x0; }

    virtual void Trigger();

private:
    QAction* action_;
};