#include "FastCommandDlg.h"

#include <EditorLib/Commands/RegisteredAction.h>

#include <qapplication.h>
#include <QBoxLayout>
#include <QCompleter>
#include <qcompleter.h>
#include <QEvent>
#include <qevent.h>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>

#include <string>

class FastCommandDlg::SearchLineEdit : public QLineEdit
{
public:
    SearchLineEdit() : QLineEdit() { }

    void focusInEvent(QFocusEvent* event) { selectAll(); QLineEdit::focusInEvent(event); }
};

class FastCommandDlg::FastCommandListWidget : public QListWidget
{
public:
    FastCommandListWidget() : QListWidget()
    {
        setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    }

    void Filter(QString text)
    {
        if (text.isEmpty())
        {
            for (int row(0); row < count(); row++)
            {
                QListWidgetItem* i = item(row);
                i->setHidden(false);
            }
            return;
        }

        QListWidgetItem* lastSelItem = selectedItems().size() ? selectedItems().first() : 0x0;
        for (int row = 0; row < count(); row++)
        {
            auto listItem = item(row);
            listItem->setHidden(true);
        }

        QList<QListWidgetItem*> matches(findItems(text, Qt::MatchFlag::MatchContains));
        bool first = true;
        for (QListWidgetItem* item : matches)
        {
            item->setHidden(false);
            if (item == lastSelItem)
            {
                item->setSelected(true);
                setItemSelected(item, true);
            }
        }
    }

    virtual bool event(QEvent* event) Q_DECL_OVERRIDE
    {
        return QListWidget::event(event);
    }
};

FastCommandDlg::FastCommandDlg() :
    QDialog(0, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
    setWindowTitle(tr("Quick Action"));
    setMinimumSize(320, 320);
    QVBoxLayout* masterLayout = new QVBoxLayout(this);

    search_ = new SearchLineEdit();
    search_->installEventFilter(this);
    masterLayout->addWidget(search_, Qt::Horizontal);

    list_ = new FastCommandListWidget();
    list_->setSelectionMode(QAbstractItemView::SingleSelection);
    list_->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
    masterLayout->addWidget(list_, Qt::Vertical | Qt::Horizontal);

    connect(search_, &QLineEdit::selectionChanged, [=]() {
        QString baseTxt = search_->text();
        QString txt;
        int selStart = search_->selectionStart();
        if (search_->hasSelectedText())
            txt = baseTxt.toStdString().substr(0, search_->selectionStart()).c_str();
        if (search_->selectionStart() != lastSelStart_)
        {
            lastSelStart_ = search_->selectionStart();
            if (txt.isEmpty() && !baseTxt.isEmpty())
            {

            }
            else
                ((FastCommandListWidget*)list_)->Filter(txt);
        }
    });
    connect(search_, &QLineEdit::textChanged, [=](const QString& newText) {

    });

    connect(list_, &QListWidget::itemClicked, [=](QListWidgetItem* item) {
        search_->setText("");
    });

    connect(list_, &QListWidget::itemDoubleClicked, [=](QListWidgetItem *item) {
        if (item)
        {
            this->Selected(item->text());
            hide();
        }
    });
}

void FastCommandDlg::Prepare()
{
    if (completer_)
        delete completer_;

    QStringList itemsList;
    Fill(itemsList);

    list_->clear();
    list_->addItems(itemsList);

    completer_ = new QCompleter(itemsList);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);
    completer_->setCompletionMode(QCompleter::InlineCompletion);
    search_->setCompleter(completer_);
    search_->setFocus();
}

FastCommandDlg::~FastCommandDlg()
{
    delete list_;
}

void FastCommandDlg::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key::Key_Escape)
        hide();
    else if (event->key() == Qt::Key::Key_Enter || event->key() == Qt::Key::Key_Return)
    {
        // do the command if any
        QString text;
        if (list_->selectedItems().size())
        {
            for (auto item : list_->selectedItems())
            {
                if (item->isHidden())
                    continue;
                text = item->text();
                search_->blockSignals(true);
                search_->setText(text);
                search_->blockSignals(false);
            }
        }
        else
            text = search_->text();

        if (!text.isEmpty())
            Selected(text);
        event->accept();
        hide();
    }
    else if (event->key() == Qt::Key::Key_Up)
    {
        search_->blockSignals(true);
        if (!list_->hasFocus() && !list_->selectedItems().isEmpty())
        {
            QListWidgetItem* selected = list_->selectedItems()[0];
            QListWidgetItem* lastvisible = 0x0;
            bool found = false;
            for (int row(0); row < list_->count(); row++)
            {
                QListWidgetItem* i = list_->item(row);
                if (i->isHidden())
                    continue;
                if (lastvisible && i == selected)
                {
                    if (selected)
                        list_->setItemSelected(selected, false);
                    list_->setItemSelected(lastvisible, true);
                    event->accept();
                    break;
                }
                lastvisible = i;
            }
        }
        search_->blockSignals(false);
    }
    else if (event->key() == Qt::Key::Key_Down)
    {
        search_->blockSignals(true);
        if (!list_->hasFocus() && !list_->selectedItems().isEmpty())
        {
            QListWidgetItem* selected = list_->selectedItems()[0];
            QListWidgetItem* previousItem = 0x0;

            bool hitSelected = false;
            bool found = false;
            for (int row(0); row < list_->count(); row++)
            {
                QListWidgetItem* i = list_->item(row);
                if (i->isHidden())
                    continue;

                if (previousItem == selected)
                {
                    if (selected)
                        list_->setItemSelected(selected, false);
                    list_->setItemSelected(i, true);
                    event->accept();
                    break;
                }
                previousItem = i;
            }
        }
        else if (!list_->hasFocus() && list_->selectedItems().size() == 0)
        {
            for (int row(0); row < list_->count(); row++)
            {
                QListWidgetItem* i = list_->item(row);
                if (i->isHidden())
                    continue;
                i->setSelected(true);
                event->accept();
                break;
            }
        }
        search_->blockSignals(false);
    }
}

bool FastCommandDlg::eventFilter(QObject* src, QEvent* event)
{
    if (src == search_)
    {
        if (event->type() == QEvent::Type::KeyPress || event->type() == QEvent::Type::KeyRelease)
        {
            QKeyEvent* keyEvent = (QKeyEvent*)event;
            if (keyEvent->key() == Qt::Key::Key_Up || keyEvent->key() == Qt::Key_Down)
            {
                if (event->type() == QEvent::Type::KeyRelease)
                    keyReleaseEvent(keyEvent);
                return true;
            }
        }
    }
    return false;
}