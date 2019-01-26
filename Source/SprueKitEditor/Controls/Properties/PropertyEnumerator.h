#pragma once

#include <EditorLib/DataSource.h>

#include <QLabel>
#include <QToolButton>

#include <string>

namespace SprueEditor
{

    class BasePropertyEditor;
    class PropertyTypeHandler;

    struct EnumeratedProperty
    {
        std::string propertyName_;
        std::string propertyTip_;
        BasePropertyEditor* editor_;
        QLabel* label_ = 0x0;
        QToolButton* tipButton_ = 0x0;

        void Destroy();
    };

    struct PropertyEnumerator
    {
        /// Sort priority allows changing the order in which enumerators are evaluated so that specialized controls can intercept handling.
        virtual unsigned GetSortPriority() const { return 255; }
        /// Return true if there's a need for a complete rebuild.
        virtual bool NeedsRebuild(std::shared_ptr<DataSource>) const { return false; }
        /// Return true if this enumerator is valid for the given datasource.
        virtual bool ValidFor(std::shared_ptr<DataSource>) = 0;
        /// Perform any required prep work and return the number of properties.
        virtual unsigned BeginPropertyEnumeration(std::shared_ptr<DataSource>) = 0;
        /// Construct a property
        virtual EnumeratedProperty EnumerateProperty(std::shared_ptr<DataSource>, unsigned index) = 0;
    };

}