#include "RegisteredAction.h"

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/DataSource.h>
#include <EditorLib/DocumentBase.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Selectron.h>

#include <QApplication>
#include <QDomDocument>
#include <QFile>
#include <QMenu>
#include <QStandardPaths>
#include <QWidget>
#include <QWindow>
#include <QXmlStreamWriter>

#include <map>

std::vector<RegisteredAction*> RegisteredAction::actions_ = std::vector<RegisteredAction*>();

RegisteredAction* RegisteredAction::GetAction(const QString& originalName)
{
    auto found = std::find_if(GetList().begin(), GetList().end(), [originalName](const RegisteredAction* action) { return action->GetOriginalName().compare(originalName) == 0; });
    if (found != GetList().end())
        return *found;
    return 0x0;
}

RegisteredAction::RegisteredAction(const QString& actionClass, const QString& text, QObject* object) :
    QAction(text, object == 0x0 ? ApplicationCore::GetMainWindow() : object),
    actionClass_(actionClass),
    originalName_(text)
{
    ApplicationCore::GetMainWindow()->addAction(this);
    actions_.push_back(this);
    setShortcutContext(Qt::ApplicationShortcut);
}

RegisteredAction::RegisteredAction(const QString& actionClass, const QIcon& icon, const QString& text, QObject* object) :
    QAction(icon, text, object == 0x0 ? ApplicationCore::GetMainWindow() : object),
    actionClass_(actionClass),
    originalName_(text)
{
    ApplicationCore::GetMainWindow()->addAction(this);
    actions_.push_back(this);
    setShortcutContext(Qt::ApplicationShortcut);
}

RegisteredAction::~RegisteredAction()
{
    auto found = std::find(actions_.begin(), actions_.end(), this);
    if (found != actions_.end())
        actions_.erase(found);
}

void RegisteredAction::Save()
{
    QXmlStreamWriter writer;
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    basePath += "/accelerators.xml";
    QFile file(basePath);

    if (!file.open(QIODevice::WriteOnly))
        return;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("accelerators");

    auto actions = GetList();
    for (auto action : actions)
    {
        QKeySequence shortCut = action->GetShortCut();
        if (shortCut.count())
        {
            writer.writeStartElement("accel");
            writer.writeAttribute("cmd", action->GetOriginalName());
            writer.writeAttribute("seq", shortCut.toString());
            writer.writeEndElement();
        }
    }

    writer.writeEndElement();

    writer.writeEndDocument();
}

void RegisteredAction::Restore()
{
    QXmlStreamReader reader;
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    basePath += "/accelerators.xml";
    if (!QFile::exists(basePath))
        return;

    QFile file(basePath);
    QDomDocument doc;
    if (!doc.setContent(&file))
        return;

    QDomElement docElem = doc.documentElement();
    if (docElem.tagName() != "accelerators")
        return;

    std::map<QString, QString> values;

    QDomElement element = docElem.firstChildElement("accel");
    while (!element.isNull())
    {
        values[element.attribute("cmd")] = element.attribute("seq");
        element = element.nextSiblingElement("accel");
    }

    auto actions = GetList();
    for (auto action : actions)
    {
        auto found = values.find(action->GetOriginalName());
        if (found != values.end())
            action->SetShortcut(QKeySequence(found->second));
    }
}

#define CHECK_STATE(STATE) if (STATE != CUIS_None) {  \
    setEnabled(STATE & CUIS_Enabled);  \
    if (isCheckable()) setChecked(STATE & CUIS_Checked); \
    if (STATE & CUIS_Visible && !isVisible()) setVisible(true); \
    else if (STATE & CUIS_Invisible && isVisible()) setVisible(false); \
    return; }

void RegisteredAction::CheckCmdUI()
{
    if (constantCmdUI_)
    {
        auto state = constantCmdUI_(this);
        CHECK_STATE(state);
    }
    if (anyCmdUI_.size())
    {
        bool enabled = false;
        for (auto ui : anyCmdUI_)
        {
            auto state = ui(this);
            CHECK_STATE(state);
        }
    }

    if (auto ds = ApplicationCore::GetObjectSelectron()->MostRecentSelected())
    {
        if (auto cmdUI = dynamic_cast<ICommandUI*>(ds.get()))
        {
            auto state = cmdUI->CommandUI(this);
            CHECK_STATE(state);
        }
    }

    if (auto doc = ApplicationCore::GetDocumentManager()->GetActiveDocument())
    {
        if (auto cmdUI = dynamic_cast<ICommandUI*>(doc))
        {
            auto state = cmdUI->CommandUI(this);
            CHECK_STATE(state);
        }
    }

    if (auto focusWidget = dynamic_cast<ICommandUI*>(QApplication::focusWidget()))
    {
        auto state = focusWidget->CommandUI(this);
        CHECK_STATE(state);
    }

    if (auto focusWindow = dynamic_cast<ICommandUI*>(QApplication::focusWindow()))
    {
        auto state = focusWindow->CommandUI(this);
        CHECK_STATE(state);
    }
}

void RegisteredAction::DoCmdUIChecks()
{
    for (auto act : actions_)
        act->CheckCmdUI();
}

QMenu* RegisteredAction::CreateMenu(const std::vector<QString>& actions)
{
    QMenu* menu = new QMenu();
    Populate(menu, actions);
    return menu;
}

void RegisteredAction::Populate(QMenu* target, const std::vector<QString>& actions)
{
    for (auto act : actions)
    {
        if (auto found = RegisteredAction::GetAction(act))
        {
            target->addAction(found);
        }
    }
}

RegisteredWrapperAction::RegisteredWrapperAction(const QString& actionClass, QAction* wrapping) :
    RegisteredAction(actionClass, wrapping->icon(), wrapping->text()),
    action_(wrapping)
{
    connect(this, &QAction::triggered, this, &RegisteredWrapperAction::Trigger);
}

void RegisteredWrapperAction::Trigger()
{
    if (action_)
        action_->trigger();
}