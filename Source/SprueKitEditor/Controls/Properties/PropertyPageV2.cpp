#include "PropertyPageV2.h"

#include "BasePropertyEditor.h"

#include "../../QtHelpers.h"

namespace SprueEditor
{

    PropertyPageV2::PropertyPageV2()
    {
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setMargin(2);
        layout->setSpacing(2);
        setLayout(layout);
        Rebuild();
    }

    PropertyPageV2::~PropertyPageV2()
    {

    }

    void PropertyPageV2::SetEditing(std::shared_ptr<DataSource> editing)
    {
        if (editing_ = editing)
        {
            Rebuild();
            //for (unsigned i = 0; i < properties_.size(); ++i)
            //    properties_[i].editor_->SetEditing(editing_);
            //UpdateLabels();
        }
    }

    void PropertyPageV2::SelectionChanged(void* src, Selectron* sel)
    {
        // Don't respond to change we made or if we're not visible
        if (src == this || !isVisible())
            return;

        SetEditing(sel->MostRecentSelected());
    }

    void PropertyPageV2::SelectronDataChanged(void* src, Selectron* sel, unsigned property)
    {
        if (isVisible())
            UpdateLabels();
    }

    void PropertyPageV2::AddPropertyEnumerator(std::shared_ptr<PropertyEnumerator> enumerator)
    {
        enumerators_.push_back(enumerator);
        std::sort(enumerators_.begin(), enumerators_.end(), [=](const std::shared_ptr<PropertyEnumerator>& lhs, const std::shared_ptr<PropertyEnumerator>& rhs) {
            return lhs->GetSortPriority() < rhs->GetSortPriority();
        });
    }

    void PropertyPageV2::Rebuild()
    {
        QtHelpers::ClearLayout(layout());

        properties_.clear();
        //for (unsigned i = 0; i < properties_.size(); ++i)
        //    properties_[i].Destroy();

        if (editing_)
        {
            for (auto enumerator : enumerators_)
            {
                if (enumerator->ValidFor(editing_))
                {
                    const unsigned count = enumerator->BeginPropertyEnumeration(editing_);
                    for (unsigned i = 0; i < count; ++i)
                    {
                        EnumeratedProperty property = enumerator->EnumerateProperty(editing_, i);
                        if (property.editor_)
                        {
                            property.label_ = new QLabel(property.propertyName_.c_str());
                            QHBoxLayout* labelLayout = new QHBoxLayout();
                            labelLayout->addWidget(property.label_);
                            layout()->addItem(labelLayout);
                            layout()->addItem(property.editor_->GetLayout());
                            if (auto handler = property.editor_->GetHandler())
                            {
                                handler->ConfigureEditor(property.editor_, editing_);
                                handler->ShowEditValue(property.editor_, editing_);
                            }
                        }
                    }
                    UpdateLabels();
                    break;
                }
            }
        }
    }

    void PropertyPageV2::UpdateLabels()
    {
        if (!editing_)
            return;
        for (unsigned i = 0; i < properties_.size(); ++i)
        {
            if (auto handler = properties_[i].editor_->GetHandler())
            {
                if (handler->IsPropertyDirty(editing_))
                    properties_[i].label_->setText(QString("<font color='#88EE88'>%1</font>").arg(properties_[i].propertyName_.c_str()));
                else
                    properties_[i].label_->setText(properties_[i].propertyName_.c_str());
            }
        }
    }
}