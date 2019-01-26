#pragma once

#include <EditorLib/DataSource.h>
#include <EditorLib/Controls/DataGridWidget.h>

#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/UI/UIElement.h>

#define SERIALIZABLE_DATA_SOURCE_ID 3
#define NODE_DATA_SOURCE_ID 4
#define COMPONENT_DATA_SOURCE_ID 5
#define UI_DATA_SOURCE_ID 6

namespace UrhoEditor
{

    class SerializableDataSource : public DataSource
    {
    public:
        //??? just putting this here??
        static std::map<Urho3D::StringHash, const char*> UrhoIcons;

        SerializableDataSource(Urho3D::Serializable* serial);

        SerializableDataSource(Urho3D::Node* node) { 
            serial_ = node;
        }

        SerializableDataSource(Urho3D::Component* comp) { 
            serial_ = comp;
        }

        virtual int GetID() const override { return 3; }
        virtual void Destroy() override;

        Urho3D::SharedPtr<Urho3D::Serializable> GetObject() { return serial_; }

        virtual QString GetName() const override;
        virtual QString GetTypeName() const override;

        virtual std::vector<QString> EnumerateFields() override;
        virtual bool CanShow(const QString& column) override;
        virtual bool CanSetText(const QString& column) override;
        virtual void FromText(const QString& column, const QString& text) override;
        virtual QString ToText(const QString& column) override;
        virtual QVariant ToVariant(const QString& column) override;
        virtual void FromVariant(const QString& column, const QVariant& var) override;

    protected:
        virtual bool equals(const DataSource* rhs) const override;

        Urho3D::SharedPtr<Urho3D::Serializable> serial_;
    };

    class NodeDataSource : public SerializableDataSource
    {
    public:
        NodeDataSource(Urho3D::Node* node) : SerializableDataSource(node)
        {
        }

        virtual int GetID() const override { return 4; }
        
        Urho3D::Node* GetNode() const { return (Urho3D::Node*)serial_.Get(); }

        virtual QString GetName() const override;
    };

    class ComponentDataSource : public SerializableDataSource
    {
    public:
        ComponentDataSource(Urho3D::Component* component) : SerializableDataSource(component)
        {
        }

        virtual int GetID() const override { return 5; }

        Urho3D::Component* GetComponent() const { return (Urho3D::Component*)serial_.Get(); }

        virtual QString GetName() const override;
    };

    class UIDataSource : public SerializableDataSource
    {
    public:
        UIDataSource(Urho3D::UIElement* component) : SerializableDataSource(component)
        {
        }

        virtual int GetID() const override { return 6; }

        Urho3D::UIElement* GetElement() const { return (Urho3D::UIElement*)serial_.Get(); }

        virtual QString GetName() const override;
    };
}