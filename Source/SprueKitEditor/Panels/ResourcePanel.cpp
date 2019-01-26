#include "ResourcePanel.h"

#include <EditorLib/Controls/FlippableSplitter.h>
#include <EditorLib/Platform/Thumbnails.h>

#include <qevent.h>
#include <QFont>
#include <QSplitter>
#include <QTreeView>
#include <QBoxLayout>
#include <QListView>
#include <QFileSystemModel>
#include <qheaderview.h>
#include <qitemdelegate.h>
#include <qpainter.h>
#include <qtooltip.h>

#include "../QtHelpers.h"

namespace SprueEditor
{

    class ResourceItemDelegate : public QItemDelegate
    {
    public:
        Thumbnails* thumbs_;

        ResourceItemDelegate(QFileSystemModel* view) :
            thumbs_(new Thumbnails())
        {
            view_ = view;
            
        }
        virtual ~ResourceItemDelegate() 
        {
            delete thumbs_;
        }
        
        //virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
        //{
        //    QMap<int, QVariant> data = view_->itemData(index);
        //
        //    QString label = data[0].toString();
        //    int height = 64;
        //    QRect rect = metrics.boundingRect(0, 0, 128, INT_MAX, Qt::TextWordWrap, label);
        //    
        //    height += rect.height();
        //
        //    return QSize(150, height += 8);
        //}

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& opts, const QModelIndex& index) const
        {
            if (index.column() == 1)
            {
                return QItemDelegate::paint(painter, opts, index);
            }

            QStyleOptionViewItem viewOpt(opts);
            QColor foreColor = index.data(Qt::ForegroundRole).value<QColor>();

            QString name = index.data(Qt::ItemDataRole::DisplayRole).value<QString>();
            auto dataMap = view_->itemData(index);
            QRectF rectF = painter->boundingRect(viewOpt.rect, Qt::AlignLeft, name);
            //painter->drawText(rectF, name);

            QPixmap map = thumbs_->GetPixmap(name);
            painter->drawPixmap(QPoint(0, 0), map);
        }

        QFontMetrics metrics = QFontMetrics(QFont("Arial", 12));
        QFileSystemModel* view_;
    };

    //class ListView : public QListView
    //{
    //public:
    //    ListView() : QListView()
    //    {
    //        setMouseTracking(true);
    //    }
    //
    //    virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE
    //    {
    //        QModelIndex index = indexAt(mapFromGlobal(event->globalPos()));
    //        if (index.isValid())
    //        {
    //            QMap<int, QVariant> data = model()->itemData(index);
    //            QString text = data[0].toString();
    //            QRect rect = metrics.boundingRect(0, 0, 128, INT_MAX, Qt::TextWordWrap, text);
    //            // If we're too small then forcibly show it
    //            if (rect.width() >= 150)
    //            {
    //                QToolTip::showText(QPoint(event->globalX(), event->globalY() - 12), text, 0x0);
    //            }
    //            //setToolTip(text);
    //        }
    //        QListView::mouseMoveEvent(event);
    //    }
    //
    //    QFontMetrics metrics = QFontMetrics(QFont("Arial", 12));
    //};

ResourcePanel::ResourcePanel()
{
    setMouseTracking(true);
    treeModel_ = new QFileSystemModel();
    treeModel_->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    treeModel_->setRootPath("C:\\");

    listModel_ = new QFileSystemModel();
    listModel_->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    listModel_->setRootPath("C:\\");
    listModel_->setIconProvider(new Thumbnails());

    tree_ = new QTreeView();
    tree_->setModel(treeModel_);
    tree_->setSelectionMode(QAbstractItemView::SingleSelection);
    tree_->header()->setVisible(false);
    tree_->setColumnHidden(1, true);
    tree_->setColumnHidden(2, true);
    tree_->setColumnHidden(3, true);
    tree_->setColumnHidden(4, true);

    //ResourceItemDelegate* del = new ResourceItemDelegate(listModel_);

    list_ = new QListView();
    list_->setModel(listModel_);
    list_->setSelectionMode(QAbstractItemView::SingleSelection);
    list_->setIconSize(QSize(64, 64));
    list_->setResizeMode(QListView::Adjust);
    list_->setWordWrap(true);
    list_->setTextElideMode(Qt::TextElideMode::ElideNone);
    list_->setWrapping(true);
    list_->setSpacing(10);
    list_->setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);

    FlippableSplitter* splitter = new FlippableSplitter();
    splitter->setObjectName("RESOURCE_PANEL_SPLITTER");
    splitter->addWidget(tree_);
    splitter->addWidget(list_);
    splitter->setOrientation(Qt::Horizontal);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(splitter);
    //QHBoxLayout* layout = new QHBoxLayout();
    //vBoxLayout_->addLayout(layout);
    //layout->addWidget(tree_);
    //layout->addWidget(list_, Qt::Horizontal);

    connect(tree_, &QAbstractItemView::clicked, [=](const QModelIndex &index) {
        QString path = treeModel_->fileInfo(index).absoluteFilePath();
        list_->setRootIndex(listModel_->setRootPath(path));
    });

    connect(tree_, &QTreeView::expanded, [=](const QModelIndex&) { tree_->resizeColumnToContents(0); });
    connect(tree_, &QTreeView::collapsed, [=](const QModelIndex&) { tree_->resizeColumnToContents(0); });

    connect(tree_->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection & selected, const QItemSelection & deselected) {
        if (!selected.isEmpty() && !selected.first().isEmpty())
        {
            if (!selected.first().indexes().isEmpty())
            {
                QString path = treeModel_->fileInfo(selected.first().indexes()[0]).absoluteFilePath();
                list_->setRootIndex(listModel_->setRootPath(path));
            }
        }
    });
}

ResourcePanel::~ResourcePanel()
{

}

void ResourcePanel::mouseMoveEvent(QMouseEvent* event)
{
    QModelIndex index = list_->indexAt(list_->mapFromGlobal(event->globalPos()));
    if (index.isValid())
    {
        QMap<int, QVariant> data = listModel_->itemData(index);
        QString text = data[0].toString();
        QToolTip::showText(event->pos(), text, 0x0);
    }
}



}