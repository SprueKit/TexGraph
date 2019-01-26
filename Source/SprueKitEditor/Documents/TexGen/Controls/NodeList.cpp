#include "NodeList.h"

#include "../../Data/TexGenData.h"

#include <SprueEngine/StringHash.h>

#include <qdrag.h>
#include <qmimedata.h>

#include <string>
#include <map>

using namespace SprueEngine;

namespace SprueEditor
{

    class NodeListItem : public QTreeWidgetItem
    {
    public:
        NodeListItem(const SprueEngine::StringHash& typeHash, const std::string& name, QTreeWidgetItem* parent) : QTreeWidgetItem(parent)
        {
            displayName_ = name;
            creationHash_ = typeHash;
            setItemData();
        }

        std::string displayName_;
        QIcon displayIcon_;
        SprueEngine::StringHash creationHash_;
        std::string savedData_; // for prefabs|favorites

        void setItemData()
        {
            setText(0, displayName_.c_str());
        }
    };

#define DECL_NODE(VAR, NAME) QTreeWidgetItem* VAR = new QTreeWidgetItem();  VAR->setText(0, #NAME); addTopLevelItem(VAR)

    NodeList::NodeList()
    {
        DECL_NODE(mathNode, Math);
        DECL_NODE(colorNode, Color);
        DECL_NODE(valueNode, Value);
        DECL_NODE(generatorsNode, Generators);
        DECL_NODE(filterNode, Filters);
        DECL_NODE(normalsNode, Normal Maps);
        prefabsItem_ = new QTreeWidgetItem();
        prefabsItem_->setText(0, "Custom Prefabs");
        addTopLevelItem(prefabsItem_);

        FillByGroup(mathNode, "Math");
        FillByGroup(colorNode, "Color");
        FillByGroup(normalsNode, "Normal Maps");
        FillByGroup(filterNode, "Filter");
        FillByGroup(generatorsNode, "Generator");
        FillByGroup(valueNode, "Value");

        headerItem()->setHidden(true);
    }

    NodeList::~NodeList()
    {

    }

    void NodeList::mouseMoveEvent(QMouseEvent* event)
    {
        if (!(event->buttons() & Qt::LeftButton))
            return;
        // if the left button is down
        if (event->buttons() & Qt::LeftButton)
        {
            if (NodeListItem* item = dynamic_cast<NodeListItem*>(itemAt(event->pos())))
            {
                QDrag* drag = new QDrag(this);
                QMimeData* mimedata = new QMimeData();
                drag->setMimeData(mimedata);
                mimedata->setObjectName("TEXTURE_GRAPH_NODE");
                mimedata->setText(item->displayName_.c_str());
                Qt::DropAction dropAction = drag->exec(Qt::DropAction::CopyAction);
            }
        }
    }

    void NodeList::PrefabAdded(const std::string& prefabFile)
    {

    }

    void NodeList::FillByGroup(QTreeWidgetItem* parent, const char* name)
    {
        std::vector<TexGenData::NodeNameRecord> records = TexGenData::GetSortedNodeRecords(name);
        for (auto item : records)
        {
            NodeListItem* treeItem = new NodeListItem(item.first, item.second.first, parent);
            treeItem->setItemData();
            QIcon icon = TexGenData::GetIconFor(item.second.typeName_);
            treeItem->setIcon(0, icon);
        }
    }
}