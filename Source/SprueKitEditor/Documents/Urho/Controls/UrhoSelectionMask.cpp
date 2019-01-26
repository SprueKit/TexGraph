#include "UrhoSelectionMask.h"

#include <Urho3D/Graphics/Drawable.h>

namespace UrhoEditor
{
    
    UrhoSelectionMask::UrhoSelectionMask()
    {
        itemModel_ = new QStandardItemModel();

        int itemIndex = 0;
        QStandardItem* drawablesItem = new QStandardItem(tr("Drawables"));
        itemModel_->setItem(itemIndex++, drawablesItem);
        drawablesItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        drawablesItem->setData(Qt::Checked, Qt::CheckStateRole);

        QStandardItem* zoneItem = new QStandardItem(tr("Zones"));
        itemModel_->setItem(itemIndex++, zoneItem);
        zoneItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        zoneItem->setData(Qt::Unchecked, Qt::CheckStateRole);

        QStandardItem* lightsItem = new QStandardItem(tr("Lights"));
        itemModel_->setItem(itemIndex++, lightsItem);
        lightsItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        lightsItem->setData(Qt::Unchecked, Qt::CheckStateRole);

        QStandardItem* physicsItem = new QStandardItem(tr("Physics"));
        itemModel_->setItem(itemIndex++, physicsItem);
        physicsItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        physicsItem->setData(Qt::Unchecked, Qt::CheckStateRole);

        QStandardItem* uiItem = new QStandardItem(tr("UI"));
        itemModel_->setItem(itemIndex++, uiItem);
        uiItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        uiItem->setData(Qt::Unchecked, Qt::CheckStateRole);


        ++itemIndex;
        setModel(itemModel_);
        setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
        setMinimumWidth(300);
    }

    unsigned UrhoSelectionMask::GetMask() const
    {
        unsigned mask = 0;
        if (itemModel_->item(0)->data(Qt::CheckStateRole).value<bool>())
            mask |= (Urho3D::DRAWABLE_GEOMETRY | Urho3D::DRAWABLE_GEOMETRY2D);
        if (itemModel_->item(1)->data(Qt::CheckStateRole).value<bool>())
            mask |= Urho3D::DRAWABLE_LIGHT;
        if (itemModel_->item(2)->data(Qt::CheckStateRole).value<bool>())
            mask |= Urho3D::DRAWABLE_ZONE;
        return mask;
    }

    void UrhoSelectionMask::SetMask(unsigned mask)
    {
        if (mask & Urho3D::DRAWABLE_GEOMETRY)
            itemModel_->item(0)->setData(Qt::Checked, Qt::CheckStateRole);
        else
            itemModel_->item(0)->setData(Qt::Unchecked, Qt::CheckStateRole);

        if (mask & Urho3D::DRAWABLE_LIGHT)
            itemModel_->item(1)->setData(Qt::Checked, Qt::CheckStateRole);
        else
            itemModel_->item(1)->setData(Qt::Unchecked, Qt::CheckStateRole);

        if (mask & Urho3D::DRAWABLE_ZONE)
            itemModel_->item(2)->setData(Qt::Checked, Qt::CheckStateRole);
        else
            itemModel_->item(2)->setData(Qt::Unchecked, Qt::CheckStateRole);
    }

    bool UrhoSelectionMask::GetPickPhysics() const
    {
        return itemModel_->item(3)->data(Qt::CheckStateRole).value<bool>();
    }

    bool UrhoSelectionMask::GetPickUI() const
    {
        return itemModel_->item(4)->data(Qt::CheckStateRole).value<bool>();
    }

}