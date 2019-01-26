#include "RibbonGallery.h"

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/ThirdParty/QSexyToolTip.h>

#include <qevent.h>
#include <QApplication>
#include <QBoxLayout>
#include <QDrag>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPainter>
#include <QScrollBar>
#include <QToolButton>

class RibbonGalleryListWidget : public QListWidget
{
public:
    RibbonGalleryListWidget(QWidget* parent = 0x0) : QListWidget(parent)
    {

    }

    virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    //virtual void startDrag(Qt::DropActions supportedActions) Q_DECL_OVERRIDE
    //{
    //    QModelIndexList indexes = this->selectedIndexes();
    //    if (indexes.count() > 0) {
    //        QMimeData *data = this->model()->mimeData(indexes);
    //        if (!data)
    //            return;
    //        QRect rect;
    //        QPixmap pixmap;// = this->renderToPixmap(indexes, &rect);
    //        rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
    //        QDrag *drag = new QDrag(this);
    //        drag->setPixmap(pixmap);
    //        drag->setMimeData(data);
    //        Qt::DropAction defaultDropAction = Qt::CopyAction;
    //        drag->exec(supportedActions, defaultDropAction);
    //    }
    //}
};

RibbonGallery::RibbonGallery(bool searchable)
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignLeft);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    displayList_ = new RibbonGalleryListWidget();
    displayList_->setMinimumWidth(240);
    displayList_->setMaximumWidth(240);
    mainLayout->addWidget(displayList_);
    displayList_->verticalScrollBar()->setVisible(false);
    displayList_->horizontalScrollBar()->setVisible(false);

    QVBoxLayout* buttonLayout = new QVBoxLayout();
    buttonLayout->setAlignment(Qt::AlignLeft);
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing(0);
    mainLayout->addLayout(buttonLayout);

    QToolButton* scrollUp = new QToolButton();
    scrollUp->setIcon(QIcon(":/qss_icons/rc/up_arrow.png"));
    QToolButton* scrollDown = new QToolButton();
    scrollDown->setIcon(QIcon(":/qss_icons/rc/down_arrow.png"));
    QToolButton* showPopup = new QToolButton();
    showPopup->setIcon(QIcon(":/qss_icons/rc/undock.png"));
    showPopup->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    buttonLayout->addWidget(scrollUp);
    buttonLayout->addWidget(scrollDown);
    buttonLayout->addWidget(showPopup);

    QSexyToolTip* toolTip = new QSexyToolTip(ApplicationCore::GetMainWindow());
    toolTip->setLayout(new QVBoxLayout());

    popupList_ = new RibbonGalleryListWidget(toolTip);
    if (searchable)
    {
        QLineEdit* searchText = new QLineEdit();
        searchText->setPlaceholderText("Search...");// SprueEditor::Localizer::Translate("Search..."));
        toolTip->layout()->addWidget(searchText);
        connect(searchText, &QLineEdit::textChanged, [=](const QString& newText) {
            UpdateSearch(newText);
        });
    }

    toolTip->layout()->addWidget(popupList_);
    popupList_->setMinimumSize(300, 300);
    toolTip->setAutoHide(true);
    toolTip->attach(showPopup);

    connect(scrollUp, &QToolButton::clicked, this, &RibbonGallery::OnScrollUp);
    connect(scrollDown, &QToolButton::clicked, this, &RibbonGallery::OnScrollDown);

    displayList_->setDragEnabled(true);
    displayList_->setDropIndicatorShown(true);
    popupList_->setDragEnabled(true);
    popupList_->setDropIndicatorShown(true);

    displayList_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    popupList_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    displayList_->setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);
    popupList_->setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);
    popupList_->setViewMode(QListView::ViewMode::IconMode);
    popupList_->setIconSize(QSize(64, 64));
    popupList_->setFlow(QListView::Flow::LeftToRight);
    popupList_->setWordWrap(true);
    popupList_->setTextElideMode(Qt::TextElideMode::ElideNone);
    //popupList_->setGridSize(QSize(64 + 32, 64 + 32));
    popupList_->setResizeMode(QListWidget::ResizeMode::Adjust);
}

RibbonGallery::~RibbonGallery()
{

}

void RibbonGallery::AddItem(QListWidgetItem* item)
{
    displayList_->addItem(item);
    auto cloned = item->clone();
    cloned->setSizeHint(QSize(64 + 32, 64 + 32));
    cloned->setTextAlignment(Qt::AlignBottom);
    popupList_->addItem(cloned);
}

void RibbonGallery::UpdateSearch(const QString& searchText)
{
    for (int i = 0; i < popupList_->count(); ++i)
    {
        if (searchText.isEmpty())
            popupList_->item(i)->setHidden(false);
        else
        {
            if (popupList_->item(i)->text().contains(searchText))
                popupList_->item(i)->setHidden(false);
            else
                popupList_->item(i)->setHidden(true);
        }
    }
}

void RibbonGallery::OnScrollUp(bool)
{
    QKeyEvent keyEvent(QEvent::Type::KeyPress, Qt::Key::Key_PageUp, Qt::KeyboardModifier::NoModifier);
    QApplication::sendEvent(displayList_, &keyEvent);
}

void RibbonGallery::OnScrollDown(bool)
{
    QKeyEvent keyEvent(QEvent::Type::KeyPress, Qt::Key::Key_PageDown, Qt::KeyboardModifier::NoModifier);
    QApplication::sendEvent(displayList_, &keyEvent);
}

void RibbonGalleryListWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    // if the left button is down
    if (event->buttons() & Qt::LeftButton)
    {
        if (QListWidgetItem* item = dynamic_cast<QListWidgetItem*>(itemAt(event->pos())))
        {
            QDrag* drag = new QDrag(this);
            QMimeData* mimedata = new QMimeData();
            drag->setMimeData(mimedata);
            mimedata->setObjectName("RIBBON_GALLERY");
            mimedata->setText(item->text());
            QFontMetrics metrics(QFont("Arial", 10));
            const int textWidth = metrics.width(item->text());

            QPixmap pixmap(QSize(32 + textWidth, 32));
            QPainter painter(&pixmap);
            painter.begin(&pixmap);
            painter.setBrush(QColor("#3daee9"));
            painter.drawRect(0, 0, 32 + textWidth, 32);
            painter.drawPixmap(0, 0, 32, 32, item->icon().pixmap(QSize(32, 32)));
            painter.setPen(QPen(Qt::white, 1));
            painter.drawText(32, 32 * 0.75f, item->text());
            painter.end();
            drag->setPixmap(pixmap);
            Qt::DropAction dropAction = drag->exec(Qt::DropAction::CopyAction);
        }
    }
}