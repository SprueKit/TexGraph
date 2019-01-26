#pragma once

#include <QBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QWidget>

#include <map>


class PathPickerWidget;

/// Widget for editing an arbitrary number of paths.
class PathListWidget : public QWidget
{
    Q_OBJECT;
public:
    PathListWidget(bool isFolderMode = true);
    virtual ~PathListWidget();

    void SetPaths(const QStringList& list);
    QStringList GetPaths();

signals:
    void PathsChanged();

    private slots:
    void AddItem(bool);

private:
    bool isFolderMode_;
    QVBoxLayout* pickersTable_;
};
