#pragma once

#include <EditorLib/Controls/ISignificantControl.h>

#include <QWidget>

class QFileSystemModel;
class QListView;
class QTreeView;

namespace SprueEditor
{

class ResourcePanel : public QWidget, public ISignificantControl
{
    //Q_OBJECT
public:
    ResourcePanel();
    virtual ~ResourcePanel();

    virtual void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;

private:
    QTreeView* tree_;
    QListView* list_;
    QFileSystemModel* treeModel_;
    QFileSystemModel* listModel_;
};

}