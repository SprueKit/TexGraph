#pragma once

#include "PropertyEditorHandler.h"

#include <EditorLib/DataSource.h>

#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>

#include <memory>

namespace SprueEditor
{
    

    class BasePropertyEditor : public QObject
    {
        Q_OBJECT;
    public:
        BasePropertyEditor(std::shared_ptr<PropertyEditorHandler> handler, bool vertical);

        virtual ~BasePropertyEditor() { }

        QBoxLayout* GetLayout() { return layout_; }

        std::shared_ptr<PropertyEditorHandler> GetHandler() { return handler_; }
        std::shared_ptr<DataSource> GetEditing() { return editing_; }
        void SetEditing(std::shared_ptr<DataSource> dataSource);

        void ChangePropertyEditorHandler(std::shared_ptr<PropertyEditorHandler> handler);

        void SetId(const QString& id) { identifier_ = id; }
        QString GetId() const { return identifier_; }

        void SetTag(unsigned tag) { tag_ = tag; }
        unsigned GetTag() const { return tag_; }

    public slots:
        virtual void PropertyUpdated(std::shared_ptr<DataSource> editable) = 0;

    signals:
        void DataChanged();

    protected:
        QString identifier_;
        unsigned tag_ = 0;
        QBoxLayout* layout_;
        std::shared_ptr<DataSource> editing_;
        std::shared_ptr<PropertyEditorHandler> handler_;
    };

    class IntPropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        IntPropertyEditor(std::shared_ptr<PropertyEditorHandler>);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>) override;

        QSpinBox* spinner_;

    public slots:
        void SpinHandler(int value);
    };

    class BoolPropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        BoolPropertyEditor(std::shared_ptr<PropertyEditorHandler>);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>) override;

        QCheckBox* checkbox_;
    public slots:
        void CheckChanged(int state);
    };

    class UIntPropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        UIntPropertyEditor(std::shared_ptr<PropertyEditorHandler>);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>) override;

        QSpinBox* spinner_;

    public slots:
        void SpinChanged(int state);
    };

    class FloatPropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        FloatPropertyEditor(std::shared_ptr<PropertyEditorHandler>);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>) override;

        QDoubleSpinBox* spinner_;

    public slots:
        void SpinHandler(double value);
    };

    class StringPropertyEditor : public BasePropertyEditor
    {
        Q_OBJECT;
    public:
        StringPropertyEditor(std::shared_ptr<PropertyEditorHandler>);

        virtual void PropertyUpdated(std::shared_ptr<DataSource>) override;

        QLineEdit* editor_;

    public slots:
        void TextChanged(const QString& txt);
    };
}