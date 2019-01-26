#pragma once

#include <EditorLib/Controls/ISignificantControl.h>

#include <QTreeWidget>
#include <qevent.h>

namespace SprueEditor
{
    ///TODO: Contains a categorized tree list of nodes
    class NodeList : public QTreeWidget, public ISignificantControl
    {
        Q_OBJECT
    public:
        NodeList();
        virtual ~NodeList();

    public slots:
        void PrefabAdded(const std::string& prefabFile);

    protected:
        virtual void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;

    private:
        void FillByGroup(QTreeWidgetItem* parentItem, const char* group);
        /// The tree widget item used for prefabs
        QTreeWidgetItem* prefabsItem_;
    };

}