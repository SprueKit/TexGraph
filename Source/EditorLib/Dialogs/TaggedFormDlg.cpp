#include "TaggedFormDlg.h"

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

TaggedFormDlg::TaggedFormDlg(const QString& title, std::vector<TaggedField>& fields, bool cancelable, QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle(title);
    QVBoxLayout* layout = new QVBoxLayout(this);

    form_ = new TaggedForm(fields, this);
    layout->addWidget(form_);

    QDialogButtonBox* box = new QDialogButtonBox();
    auto okayButton = box->addButton(QDialogButtonBox::Ok);
    auto cancelButton = box->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(box);
}

TaggedFormDlg::~TaggedFormDlg()
{

}

void TaggedFormDlg::ShowModal(const QString& title, std::vector<TaggedField>& fields, std::function<void()> accepted)
{
    TaggedFormDlg* dlg = new TaggedFormDlg(title, fields, false, 0x0);
    dlg->setModal(true);
    if (dlg->exec() == QDialog::Accepted)
        accepted();
}

void TaggedFormDlg::ShowModal(const QString& title, std::vector<TaggedField>& fields, std::function<void()> accepted, const QSize& size)
{
    TaggedFormDlg* dlg = new TaggedFormDlg(title, fields, false, 0x0);
    dlg->setModal(true);
    dlg->setMinimumSize(size);
    if (dlg->exec() == QDialog::Accepted)
        accepted();
}

void TaggedFormDlg::ShowModal(const QString& title, std::vector<TaggedField>& fields, bool cancelable, std::function<void()> accepted)
{
    TaggedFormDlg* dlg = new TaggedFormDlg(title, fields, cancelable, 0x0);
    dlg->setModal(true);
    if (dlg->exec() == QDialog::Accepted)
        accepted();
}

void TaggedFormDlg::ShowModal(const QString& title, std::vector<TaggedField>& fields, bool cancelable, std::function<void()> accepted, const QSize& size)
{
    TaggedFormDlg* dlg = new TaggedFormDlg(title, fields, cancelable, 0x0);
    dlg->setModal(true);
    dlg->setMinimumSize(size);
    if (dlg->exec() == QDialog::Accepted)
        accepted();
}