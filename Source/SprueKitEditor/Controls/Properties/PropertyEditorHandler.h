#pragma once
#ifndef _PROPERTY_EDITOR_HANDLER_
#define _PROPERTY_EDITOR_HANDLER_

#include <EditorLib/DataSource.h>

#include <memory>

namespace SprueEditor
{

class BasePropertyEditor;

struct PropertyEditorHandler
{
    /// Test whether the property in the given object is dirty.
    virtual bool IsPropertyDirty(std::shared_ptr<DataSource> dataSource) = 0;
    /// Called to configure changes to the editor's properties (like spinner step size)
    virtual void ConfigureEditor(BasePropertyEditor*, std::shared_ptr<DataSource>) = 0;
    /// Called to fetch the value from the object and show it in the editor
    /// Object -> Editor
    virtual void ShowEditValue(BasePropertyEditor*, std::shared_ptr<DataSource>) = 0;
    /// Apply the contents of the control to the object being edited
    /// Editor -> Object
    virtual void CommitEditValue(BasePropertyEditor*, std::shared_ptr<DataSource>) = 0;
};

}

#endif