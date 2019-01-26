#pragma once

#include "PropertyEnumerator.h"

#include <EditorLib/ScrollAreaWidget.h>
#include <EditorLib/Selectron.h>
#include <EditorLib/DataSource.h>

#include <QLabel>

namespace SprueEditor
{

    class BasePropertyEditor;
    class PropertyTypeHandler;
    
    class PropertyPageV2 : public QWidget, public SelectronLinked
    {
    public:
        PropertyPageV2();
        virtual ~PropertyPageV2();

        /// Set the currently editing object and rebuild the property page.
        virtual void SetEditing(std::shared_ptr<DataSource> editing);
        /// When the selection changes we probably need to respond.
        virtual void SelectionChanged(void* src, Selectron* sel);
        /// When selectron data changes we potentially need to update our properties.
        virtual void SelectronDataChanged(void* src, Selectron* sel, unsigned property);
        
        /// Adds a property enumerator as an option for page generator, list will be resorted again by priority.
        void AddPropertyEnumerator(std::shared_ptr<PropertyEnumerator>);

    protected:
        /// Reconstruct controls content.
        void Rebuild();
        /// Update the labels contained when edits have happened
        void UpdateLabels();

        std::vector< std::shared_ptr<PropertyEnumerator> > enumerators_;
        std::shared_ptr<DataSource> editing_ = 0x0;
        std::vector<EnumeratedProperty> properties_;
    };
}