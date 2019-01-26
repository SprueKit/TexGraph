#pragma once

#include "../PropertyEditorHandler.h"

#include <SprueEngine/StringHash.h>

namespace SprueEditor
{
    struct SprueBasePropertyHandler : public PropertyEditorHandler
    {
        SprueBasePropertyHandler(const char* propName) {
            hash_ = SprueEngine::StringHash(propName);
        }

        SprueEngine::StringHash hash_;
        virtual bool IsPropertyDirty(std::shared_ptr<DataSource> dataSource) override;
    };


    struct SprueBoolPropertyHandler : public SprueBasePropertyHandler
    {
        SprueBoolPropertyHandler(const char* propName) : SprueBasePropertyHandler(propName) { }
        virtual void ConfigureEditor(BasePropertyEditor*, std::shared_ptr<DataSource>) override { }
        virtual void ShowEditValue(BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void CommitEditValue(BasePropertyEditor*, std::shared_ptr<DataSource>) override;
    };

    struct SprueVec3PropertyHandler : public SprueBasePropertyHandler
    {
        SprueVec3PropertyHandler(const char* propName) : SprueBasePropertyHandler(propName) { }
        virtual void ConfigureEditor(BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void ShowEditValue(BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void CommitEditValue(BasePropertyEditor*, std::shared_ptr<DataSource>) override;
    };

    struct SprueQuatPropertyHandler : public SprueBasePropertyHandler
    {
        SprueQuatPropertyHandler(const char* propName) : SprueBasePropertyHandler(propName) { }
        virtual void ConfigureEditor(BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void ShowEditValue(BasePropertyEditor*, std::shared_ptr<DataSource>) override;
        virtual void CommitEditValue(BasePropertyEditor*, std::shared_ptr<DataSource>) override;
    };
}