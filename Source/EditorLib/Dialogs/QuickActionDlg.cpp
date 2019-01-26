#include "QuickActionDlg.h"

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/DocumentManager.h>
#include <EditorLib/Commands/RegisteredAction.h>
#include <EditorLib/Commands/DocumentRegisteredAction.h>

#include <QApplication>

QuickActionDlg* QuickActionDlg::instance_ = 0x0;

void QuickActionDlg::Activate(IQuickActionSource* source)
{
    if (instance_ == 0x0)
        instance_ = new QuickActionDlg();
    QWidget* focused = QApplication::focusWidget();
    instance_->qaSource_ = dynamic_cast<IQuickActionSource*>(focused);
    instance_->Prepare();
    instance_->setWindowTitle(tr("Quick Action"));
    instance_->setModal(true);
    instance_->show();
    instance_->search_->clear();
    instance_->search_->setFocus();
    //for (int row(0); row < instance_->list_->count(); row++)
    //{
    //    QListWidgetItem* i = instance_->list_->item(row);
    //    i->setHidden(false);
    //    i->setSelected(false);
    //}
}

void QuickActionDlg::Fill(QStringList& list)
{
    if (qaSource_)
    {
        auto extra = qaSource_->GetActions();
        for (auto item : extra)
        {
            actions_[item->text()] = item;
            list.push_back(item->text());
        }
    }

    auto curDoc = ApplicationCore::GetDocumentManager()->GetActiveDocument();
    for (auto item : RegisteredAction::GetList())
    {
        if (curDoc && !item->IsDocumentRelevant(curDoc))
            continue;
        actions_[item->text()] = item;
        list.push_back(item->text());
    }
}

void QuickActionDlg::Selected(const QString& text)
{
    auto found = actions_.find(text);
    if (found != actions_.end())
        found->second->trigger();
}