#include "UrhoSceneTree.h"

#include "../BaseUrhoDocument.h"
#include "../../../GlobalAccess.h"
#include "../../../Data/UrhoDataSources.h"
#include "../../../Commands/Urho/UrhoSceneCmds.h"

#include <QMenu>
#include <QAction>

#include <EditorLib/DocumentManager.h>

#include <Urho3D/Scene/Component.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIElement.h>

#define NODE_ID 1
#define COMPONENT_ID 2
#define UI_ELEMENT_ID 3

using namespace Urho3D;

namespace UrhoEditor
{

    UrhoSceneTree::UrhoSceneTree()
    {
        connect(this, &Repeater::SelectionChanged, this, &UrhoSceneTree::OnLocalSelectionChange);
    }

    UrhoSceneTree::~UrhoSceneTree()
    {

    }

    void UrhoSceneTree::GenerateUI()
    {
        if (auto activeDocument = Global_DocumentManager()->GetActiveDoc<BaseUrhoDocument>())
        {
            if (auto scene = activeDocument->GetScene())
            {
                PrintObject(scene);
                if (auto uiRoot = scene->GetContext()->GetSubsystem<UI>()->GetRoot())
                    PrintElement(uiRoot);
            }
        }
    }

    void UrhoSceneTree::OnForeignSelectionChange(void* src, Selectron* sel)
    {
        if (src != this)
        {
            ClearSelected(false);
            for (unsigned i = 0; i < sel->GetSelectedCount(); ++i)
            {
                if (auto ds = GetSelectron()->GetSelected<SerializableDataSource>(i))
                {
                    if (ds->GetID() == NODE_DATA_SOURCE_ID)
                        SetSelected({ NODE_ID, ds->GetObject() }, true, false);
                    else if (ds->GetID() == COMPONENT_DATA_SOURCE_ID)
                        SetSelected({ COMPONENT_ID, ds->GetObject() }, true, false);
                    else if (ds->GetID() == UI_DATA_SOURCE_ID)
                        SetSelected({ UI_ELEMENT_ID, ds->GetObject() }, true, false);
                }
            }
            DoUpdate();
        }
    }

    void UrhoSceneTree::OnLocalSelectionChange(std::vector<IMItemID> items)
    {
        // Don't fire notifications constantly
        GetSelectron()->blockSignals(true);
        GetSelectron()->ClearSelection(this);
        for (int i = 0; i < items.size(); ++i)
        {
            auto& item = items[i];
            if (item.first == NODE_ID)
                GetSelectron()->AddSelected(this, std::make_shared<NodeDataSource>((Node*)items[i].second));
            else if (item.first == COMPONENT_ID)
                GetSelectron()->AddSelected(this, std::make_shared<ComponentDataSource>((Component*)items[i].second));
            else if (item.first == UI_ELEMENT_ID)
                GetSelectron()->AddSelected(this, std::make_shared<UIDataSource>((UIElement*)items[i].second));

        }
        GetSelectron()->blockSignals(false);
        GetSelectron()->NotifySelectionChanged(this);
    }

    void UrhoSceneTree::Link(Selectron* sel)
    {
        connect(sel, &Selectron::SelectionChanged, this, &UrhoSceneTree::OnForeignSelectionChange);
    }

    void UrhoSceneTree::PrintObject(Urho3D::Node* node)
    {
        QString name = node->GetName().CString() ? node->GetName().CString() : "<unnamed>";
        int id = node->GetID();

        auto components = node->GetComponents();
        auto children = node->GetChildren();
        int nonTempChildCt = 0;
        for (auto child : children)
            if (child->IsTemporary())
                ++nonTempChildCt;

        const bool hasChildren = components.Size() || nonTempChildCt;

        // TODO: verify the expense of doing this.
        QIcon icon;
        QPixmap pix;
        auto foundIcon = SerializableDataSource::UrhoIcons.find(node->GetType());
        if (foundIcon != SerializableDataSource::UrhoIcons.end())
        {
            icon = QIcon(foundIcon->second);
            pix = icon.pixmap(16, 16);
        }

        QColor textColor = node->IsEnabled() ? Qt::white : Qt::red;

        QString textName = QString("%1 - %2 [%3]").arg(name).arg(QString(node->GetTypeName().CString())).arg(id);
        if (TreeNode({ NODE_ID, node }, textName.toStdString().c_str(), &pix, !hasChildren, textColor))
        {
            for (auto comp : components)
            {
                QPixmap img;
                foundIcon = SerializableDataSource::UrhoIcons.find(comp->GetType());
                if (foundIcon != SerializableDataSource::UrhoIcons.end())
                {
                    img = QIcon(foundIcon->second).pixmap(16, 16);
                }

                textColor = comp->IsEnabled() ? Qt::green : Qt::red;
                TreeNode({ COMPONENT_ID, comp.Get() }, comp->GetTypeName().CString(), &img, true, textColor);
            }
            for (auto child : children)
            {
                if (!child->IsTemporary())
                    PrintObject(child);
            }

            TreePop();
        }
    }

    void UrhoSceneTree::PrintElement(Urho3D::UIElement* ui)
    {
        QString name = ui->GetName().CString() ? ui->GetName().CString() : "<unnamed>";
        QString typeName = ui->GetTypeName().CString();

        auto children = ui->GetChildren();
        int nonTempCt = 0;
        for (auto child : children)
        {
            if (!child->IsTemporary())
                ++nonTempCt;
        }

        const bool hasChildren = nonTempCt;
        
        QIcon icon;
        QPixmap pix;
        auto foundIcon = SerializableDataSource::UrhoIcons.find(ui->GetType());
        if (foundIcon != SerializableDataSource::UrhoIcons.end())
        {
            icon = QIcon(foundIcon->second);
            pix = icon.pixmap(16, 16);
        }

        QColor textColor = ui->IsVisible() ? Qt::cyan : Qt::red;
        QString textName = QString("%1 - %2").arg(name, typeName);
        if (TreeNode({ UI_ELEMENT_ID, ui }, textName.toStdString().c_str(), &pix, !hasChildren, textColor))
        {
            for (auto child : children)
            {
                if (!child->IsTemporary())
                    PrintElement(child);
            }

            TreePop();
        }
    }

    static inline bool RootCheck(const IMItemID& item)
    {
        if (item.first == NODE_ID)
            return ((Node*)item.second)->GetParent() == 0x0;
        else if (item.first == UI_ELEMENT_ID)
            return ((UIElement*)item.second)->GetParent() == 0x0;
        return false;
    }

    bool UrhoSceneTree::DeleteItem(IMItemID item)
    {
        return !RootCheck(item);
    }

    bool UrhoSceneTree::CanDragItem(IMItemID item)
    {
        return !RootCheck(item);
    }

    bool UrhoSceneTree::ContextMenu(const std::vector<IMItemID>& items, const QPoint& pt)
    {
        bool anyRoots = false;
        for (auto& item : items)
            anyRoots |= RootCheck(item);

        QMenu* menu = new QMenu(this);

        if (!anyRoots)
        {
            QAction* delAction = new QAction("Delete");
            menu->addAction(delAction);
        }

        if (items.size() == 1)
        {
            if (items[0].first == NODE_ID)
            {
                Node* node = (Node*)items[0].second;

                if (!anyRoots)
                    menu->addSeparator();

                QAction* addReplicatedChild = new QAction(QIcon(":/Images/urho/ReplicatedNode.png"), "Add Replicated Child");
                QAction* addStaticChild = new QAction(QIcon(":/Images/urho/StaticNode.png"), "Add Static Child");
                menu->addAction(addReplicatedChild);
                menu->addAction(addStaticChild);
                menu->addSeparator();

                connect(addReplicatedChild, &QAction::triggered, [=](bool) {
                    Node* created = node->CreateChild();
                    PUSH_DOCUMENT_UNDO(new SceneCommands::AddNodeCmd(node, std::shared_ptr<NodeDataSource>(new NodeDataSource(created))));
                });

                connect(addStaticChild, &QAction::triggered, [=](bool) {
                    Node* created = node->CreateChild("", Urho3D::LOCAL);
                    PUSH_DOCUMENT_UNDO(new SceneCommands::AddNodeCmd(node, std::shared_ptr<NodeDataSource>(new NodeDataSource(created))));
                });

                auto context = node->GetContext();
                auto categories = context->GetObjectCategories();
                for (auto category : categories)
                {
                    if (category.first_.Compare("UI") == 0)
                        continue;

                    std::vector<QAction*> actions;
                    QMenu* subMenu = new QMenu(category.first_.CString());
                    for (auto categoryItem : category.second_)
                    {
                        auto foundIcon = SerializableDataSource::UrhoIcons.find(categoryItem);
                        QAction* addComp = 0x0;

                        if (foundIcon != SerializableDataSource::UrhoIcons.end())
                            addComp = new QAction(QIcon(foundIcon->second), context->GetTypeName(categoryItem).CString());
                        else
                            addComp = new QAction(context->GetTypeName(categoryItem).CString());

                        connect(addComp, &QAction::triggered, [=](bool) {
                            Component* comp = node->CreateComponent(categoryItem);
                            PUSH_DOCUMENT_UNDO(new SceneCommands::AddComponentCmd(node, std::shared_ptr<ComponentDataSource>(new ComponentDataSource(comp))));
                        });
                        actions.push_back(addComp);
                    }

                    std::sort(actions.begin(), actions.end(), [=](QAction* lhs, QAction* rhs) { return lhs->text().compare(rhs->text(), Qt::CaseInsensitive) < 0; });
                    for (auto action : actions)
                        subMenu->addAction(action);
                    menu->addMenu(subMenu);
                }

            }
            else if (items[0].first == UI_ELEMENT_ID)
            {
                UIElement* elem = (UIElement*)items[0].second;
                if (!anyRoots)
                    menu->addSeparator();

                auto context = elem->GetContext();
                auto categories = context->GetObjectCategories();
                for (auto category : categories)
                {
                    if (category.first_.Compare("UI") != 0)
                        continue;

                    std::vector<QAction*> actions;
                    QMenu* subMenu = new QMenu(category.first_.CString());
                    for (auto categoryItem : category.second_)
                    {
                        auto foundIcon = SerializableDataSource::UrhoIcons.find(categoryItem);
                        QAction* addComp = 0x0;

                        if (foundIcon != SerializableDataSource::UrhoIcons.end())
                            addComp = new QAction(QIcon(foundIcon->second), context->GetTypeName(categoryItem).CString());
                        else
                            addComp = new QAction(context->GetTypeName(categoryItem).CString());

                        connect(addComp, &QAction::triggered, [=](bool) {
                            UIElement* comp = elem->CreateChild(categoryItem);
                            PUSH_DOCUMENT_UNDO(new SceneCommands::AddUIElementCmd(elem, std::make_shared<UIDataSource>(comp)));
                        });
                        actions.push_back(addComp);
                    }

                    std::sort(actions.begin(), actions.end(), [=](QAction* lhs, QAction* rhs) { return lhs->text().compare(rhs->text(), Qt::CaseInsensitive) < 0; });
                    for (auto action : actions)
                        subMenu->addAction(action);
                    menu->addMenu(subMenu);
                }
            }
        }

        if (menu->actions().size())
        {
            menu->exec(pt);
            return true;
        }

        return false;
    }

    bool UrhoSceneTree::CanDropOn(IMItemID who, IMItemID onto)
    {
        if (who.first == NODE_ID)
            return onto.first == NODE_ID;
        else if (who.first == COMPONENT_ID)
            return onto.first == NODE_ID;
        else if (who.first == UI_ELEMENT_ID)
            return onto.first == UI_ELEMENT_ID;
        return false;
    }

    bool UrhoSceneTree::CanDropBefore(IMItemID who, IMItemID before)
    {
        // can never drop before a root
        if (RootCheck(before))
            return false;

        return who.first == before.first;
    }

    bool UrhoSceneTree::CanDropAfter(IMItemID who, IMItemID after)
    {
        // can never drop after a root
        if (RootCheck(after))
            return false;
        return who.first == after.first;
    }

    void UrhoSceneTree::DoDropOn(IMItemID who, IMItemID onto)
    {
        if (who.first == NODE_ID)
        {
            if (onto.first == NODE_ID)
                PUSH_DOCUMENT_UNDO(new SceneCommands::MoveNodeCmd(((Node*)who.second)->GetParent(), (Node*)onto.second, std::make_shared<NodeDataSource>((Node*)who.second)));
        }
        else if (who.first == COMPONENT_ID)
        {
            if (onto.first == NODE_ID)
            {
                Component* me = (Component*)who.second;
                PUSH_DOCUMENT_UNDO(new SceneCommands::MoveComponentOwnerCmd(me->GetNode(), (Node*)onto.second, std::make_shared<ComponentDataSource>(me)));
            }
        }
        else if (who.first == UI_ELEMENT_ID)
        {
            if (onto.first == UI_ELEMENT_ID)
            {
                UIElement* me = (UIElement*)who.second;
                PUSH_DOCUMENT_UNDO(new SceneCommands::MoveUIElementCmd(me->GetParent(), (UIElement*)onto.second, std::make_shared<UIDataSource>(me)));
            }
        }
    }

    void UrhoSceneTree::DoDropBefore(IMItemID who, IMItemID before)
    {
        if (who.first == NODE_ID)
        {
            auto children = ((Node*)before.second)->GetParent()->GetChildren();
            int idx = 0;
            for (; idx < children.Size(); ++idx)
                if (children[idx] == before.second)
                    break;

            PUSH_DOCUMENT_UNDO(new SceneCommands::MoveNodeCmd(((Node*)who.second)->GetParent(), ((Node*)before.second)->GetParent(), std::make_shared<NodeDataSource>((Node*)who.second), idx));
        }
        else if (who.first == UI_ELEMENT_ID)
        {
            auto children = ((UIElement*)before.second)->GetParent()->GetChildren();
            int idx = 0;
            for (; idx < children.Size(); ++idx)
                if (children[idx] == before.second)
                    break;

            PUSH_DOCUMENT_UNDO(new SceneCommands::MoveUIElementCmd(((UIElement*)who.second)->GetParent(), ((UIElement*)before.second)->GetParent(), std::make_shared<UIDataSource>((UIElement*)who.second), idx));
        }
    }

    void UrhoSceneTree::DoDropAfter(IMItemID who, IMItemID after)
    {
        if (who.first == NODE_ID)
        {
            auto children = ((Node*)after.second)->GetParent()->GetChildren();
            int idx = 0;
            for (; idx < children.Size(); ++idx)
                if (children[idx] == after.second)
                    break;

            PUSH_DOCUMENT_UNDO(new SceneCommands::MoveNodeCmd(((Node*)who.second)->GetParent(), ((Node*)after.second)->GetParent(), std::make_shared<NodeDataSource>((Node*)who.second), idx + 1));
        }
        else if (who.first == UI_ELEMENT_ID)
        {
            auto children = ((UIElement*)after.second)->GetParent()->GetChildren();
            int idx = 0;
            for (; idx < children.Size(); ++idx)
                if (children[idx] == after.second)
                    break;

            PUSH_DOCUMENT_UNDO(new SceneCommands::MoveUIElementCmd(((UIElement*)who.second)->GetParent(), ((UIElement*)after.second)->GetParent(), std::make_shared<UIDataSource>((UIElement*)who.second), idx + 1));
        }
    }
}
