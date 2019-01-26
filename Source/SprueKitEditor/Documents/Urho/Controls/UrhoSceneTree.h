#pragma once

#include <EditorLib/IM/Repeater.h>

#include <EditorLib/Selectron.h>

namespace Urho3D
{
    class Node;
    class UIElement;
}

namespace UrhoEditor
{

    class UrhoSceneTree : public Repeater, public SelectronLinked
    {
        Q_OBJECT;
    public:
        UrhoSceneTree();
        virtual ~UrhoSceneTree();

        virtual void GenerateUI() override;

    public slots:
        void OnForeignSelectionChange(void* who, Selectron* sel);
        void OnLocalSelectionChange(std::vector<IMItemID> items);

    protected:
        virtual void Link(Selectron*) override;

        void PrintObject(Urho3D::Node* node);
        void PrintElement(Urho3D::UIElement* ui);

        virtual bool DeleteItem(IMItemID item) override;
        virtual bool CanDragItem(IMItemID item) override;
        virtual bool ContextMenu(const std::vector<IMItemID>& items, const QPoint& pt) override;
        virtual bool CanDropOn(IMItemID who, IMItemID onto) override;
        virtual bool CanDropBefore(IMItemID who, IMItemID before) override;
        virtual bool CanDropAfter(IMItemID who, IMItemID before) override;
        virtual void DoDropOn(IMItemID who, IMItemID onto) override;
        virtual void DoDropBefore(IMItemID who, IMItemID before) override;
        virtual void DoDropAfter(IMItemID who, IMItemID before) override;
    };

}