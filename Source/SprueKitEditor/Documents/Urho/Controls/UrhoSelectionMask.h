#pragma once

#include <QComboBox>
#include <QStandardItemModel>

namespace UrhoEditor
{
    class UrhoSelectionMask : public QComboBox
    {
    public:
        UrhoSelectionMask();

        unsigned GetMask() const;
        void SetMask(unsigned mask);
        bool GetPickPhysics() const;
        bool GetPickUI() const;

    private:
        QStandardItemModel* itemModel_;
    };

}