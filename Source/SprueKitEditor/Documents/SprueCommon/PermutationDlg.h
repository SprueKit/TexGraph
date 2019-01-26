#pragma once

#include <QDialog>
#include "Data/SprueDataSources.h"

#include <SprueEngine/IEditable.h>
#include <SprueEngine/Variant.h>

class FlagWidget;
class QVBoxLayout;

namespace SprueEditor
{
    class BaseTypeEditor;

    class PermutationDlg : public QDialog
    {
        Q_OBJECT;
    public:
        PermutationDlg(std::shared_ptr<IEditableDataSource> dataSource, const QString& propertyName, SprueEngine::VariantType type, SprueEngine::Permutations values);
        virtual ~PermutationDlg();

        virtual void reject() Q_DECL_OVERRIDE;

    private:
        SprueEngine::Permutations GetValues();

        void AddControl(QVBoxLayout* holder, SprueEngine::FieldPermutation* perm = 0x0);

        struct FieldData {
            QLineEdit* name_;
            QLineEdit* weight_;
            FlagWidget* flags_;
            BaseTypeEditor* editor_;

            ~FieldData();
        };

        std::shared_ptr<IEditableDataSource> dataSource_;
        QString propertyName_;
        SprueEngine::VariantType type_;
        std::vector<FieldData*> editors_;
        bool dirty_ = false;
    };

}