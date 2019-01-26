#include "PathListWidget.h"

#include "PathPickerWidget.h"

#include <QBoxLayout>
#include <QHeaderView>

#include "../QtHelpers.h"


PathListWidget::PathListWidget(bool isFolderMode) :
    isFolderMode_(isFolderMode)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    buttonsLayout->setAlignment(Qt::AlignTop);
    QPushButton* addButton = new QPushButton(QIcon(":/Images/godot/icon_add.png"), "");

    buttonsLayout->addWidget(addButton);

    pickersTable_ = new QVBoxLayout();
    layout->addLayout(pickersTable_);
    layout->addLayout(buttonsLayout);

    connect(addButton, &QPushButton::clicked, this, &PathListWidget::AddItem);
}

PathListWidget::~PathListWidget()
{

}

void PathListWidget::SetPaths(const QStringList& list)
{
    QtHelpers::ClearLayout(pickersTable_);

    for (int i = 0; i < list.size(); ++i)
    {
        PathPickerWidget* widget = new PathPickerWidget(isFolderMode_, true);
        connect(widget, &PathPickerWidget::PathChanged, [=](const PathPickerWidget* widget, const std::string& newPath) {
            emit PathsChanged();
        });
        connect(widget, &PathPickerWidget::Deleted, [=](const PathPickerWidget*) {
            pickersTable_->removeWidget(widget);
            widget->deleteLater();
            emit PathsChanged();
        });
        widget->SetPath(list[i].toStdString());
        pickersTable_->addWidget(widget);
    }
}

QStringList PathListWidget::GetPaths()
{
    QStringList ret;

    for (int i = 0; i < pickersTable_->count(); ++i)
        ret.push_back(((PathPickerWidget*)pickersTable_->itemAt(i)->widget())->GetPath().c_str());

    return ret;
}

void PathListWidget::AddItem(bool)
{
    PathPickerWidget* widget = new PathPickerWidget(isFolderMode_, true);
    pickersTable_->addWidget(widget);
    connect(widget, &PathPickerWidget::PathChanged, [=](const PathPickerWidget* widget, const std::string& newPath) {
        emit PathsChanged();
    });
    connect(widget, &PathPickerWidget::Deleted, [=](const PathPickerWidget*) {
        pickersTable_->removeWidget(widget);
        widget->deleteLater();
        emit PathsChanged();
    });
    emit PathsChanged();
}