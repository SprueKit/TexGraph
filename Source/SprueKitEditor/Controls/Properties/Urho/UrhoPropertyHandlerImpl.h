#pragma once

#include "../PropertyEditorHandler.h"

#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/Math/StringHash.h>

namespace UrhoEditor
{
    struct UrhoBasePropertyHandler : public SprueEditor::PropertyEditorHandler
    {
        Urho3D::String name_;
        Urho3D::StringHash hash_;
        virtual bool IsPropertyDirty(std::shared_ptr<DataSource> dataSource) override;

        const Urho3D::AttributeInfo* Attribute(Urho3D::Serializable* serialziable);
    };


    struct UrhoBoolPropertyHandler : public UrhoBasePropertyHandler
    {
        virtual void ConfigureEditor(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override { }
        virtual void ShowEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void CommitEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
    };

    struct UrhoColorPropertyHandler : public UrhoBasePropertyHandler
    {
        virtual void ConfigureEditor(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void ShowEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void CommitEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
    };

    struct UrhoVec2PropertyHandler : public UrhoBasePropertyHandler
    {
        virtual void ConfigureEditor(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void ShowEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void CommitEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
    };

    struct UrhoVec3PropertyHandler : public UrhoBasePropertyHandler
    {
        virtual void ConfigureEditor(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void ShowEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void CommitEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
    };

    struct UrhoQuatPropertyHandler : public UrhoBasePropertyHandler
    {
        virtual void ConfigureEditor(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void ShowEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void CommitEditValue(SprueEditor::BasePropertyEditor*, std::shared_ptr<DataSource>) override;
    };
}