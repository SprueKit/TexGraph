#pragma once

#include <EditorLib/editorlib_global.h>
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

/// Baseclass for a collection of property editors that work with QVariant types.
class EDITORLIB_EXPORT BasePropertyEditor : public QObject
{
    Q_OBJECT;
public:
    BasePropertyEditor(bool vertical);

    virtual ~BasePropertyEditor() { }

    QBoxLayout* GetLayout() { return layout_; }

    void SetId(const QString& id) { identifier_ = id; }
    QString GetId() const { return identifier_; }

    void SetTag(unsigned tag) { tag_ = tag; }
    unsigned GetTag() const { return tag_; }

    virtual void SetValue(const QVariant& value) = 0;
    virtual QVariant GetValue() const = 0;

signals:
    void DataChanged();
    void ValueChanged(const QVariant& value);

protected:
    QString identifier_;
    unsigned tag_ = 0;
    QBoxLayout* layout_;
};

class EDITORLIB_EXPORT IntPropertyEditor : public BasePropertyEditor
{
    Q_OBJECT;
public:
    IntPropertyEditor();

    QSpinBox* spinner_;

    virtual void SetValue(const QVariant& value) override;
    virtual QVariant GetValue() const override;

public slots:
    void SpinHandler(int value);
};

class EDITORLIB_EXPORT BoolPropertyEditor : public BasePropertyEditor
{
    Q_OBJECT;
public:
    BoolPropertyEditor();

    virtual void SetValue(const QVariant& value) override;
    virtual QVariant GetValue() const override;

    QCheckBox* checkbox_;

public slots:
    void CheckChanged(int state);
};

class EDITORLIB_EXPORT UIntPropertyEditor : public BasePropertyEditor
{
    Q_OBJECT;
public:
    UIntPropertyEditor();

    virtual void SetValue(const QVariant& value) override;
    virtual QVariant GetValue() const override;

    QSpinBox* spinner_;

public slots:
    void SpinChanged(int state);
};

class EDITORLIB_EXPORT FloatPropertyEditor : public BasePropertyEditor
{
    Q_OBJECT;
public:
    FloatPropertyEditor();

    virtual void SetValue(const QVariant& value) override;
    virtual QVariant GetValue() const override;

    QDoubleSpinBox* spinner_;

public slots:
    void SpinHandler(double value);
};

class EDITORLIB_EXPORT StringPropertyEditor : public BasePropertyEditor
{
    Q_OBJECT;
public:
    StringPropertyEditor();

    virtual void SetValue(const QVariant& value) override;
    virtual QVariant GetValue() const override;

    QLineEdit* editor_;

public slots:
    void TextChanged(const QString& txt);
};