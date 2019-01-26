#pragma once

#include <EditorLib/editorlib_global.h>

#include <QObject>
#include <QVariant>

enum SettingType
{
    ST_VARIANT_TYPE = 0,    // Will use exactly what the QVariant type is
    ST_PATH = 1,            // String value is treated as a path
    ST_FILE = 2,            // String value is treated as a file
    ST_ENUM = 3,            // Enum value is treated as a file, options_ value is a StringList of values
    ST_PATH_LIST = 4,       // Property is a list of paths which (QStringList)
};

class EDITORLIB_EXPORT SettingValue : public QObject
{
    Q_OBJECT;
public:
    SettingValue() : QObject(0x0) { }
    SettingValue(SettingType t, QString name, QString tip, QVariant defVal, QVariant val, QVariant opts) : 
        QObject(0x0),
        type_(t),
        name_(name),
        tip_(tip),
        defaultValue_(defVal),
        value_(val),
        options_(opts)
    {

    }

    SettingType type_;
    QString name_;
    QString tip_;
    /// If the setting is not found in the XML file, this value will be used.
    QVariant defaultValue_;
    QVariant value_;
    QVariant options_;

    void Notify() {
        emit Changed(value_);
    }

signals:
    void Changed(const QVariant& newValue);
};