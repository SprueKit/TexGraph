#pragma once

#include <EditorLib/editorlib_global.h>

#include <QDomDocument>
#include <QStringList>
#include <QXmlStreamWriter>

/// List of values for a set of bits
class EDITORLIB_EXPORT SettingsBitNames : public QStringList
{
public:
    /// Construct for a given number of bits.
    SettingsBitNames(const QString& name, bool enumerationValues, unsigned bitCount);

    QString GetName() { return name_; }

    QString GetBitName(int index);

    /// If true this isn't a set of bit names but a set of names for an enumeration property.
    bool IsEnumValues() const { return enumerationValues_; }

    void Save(QXmlStreamWriter* parentElement);
    void Restore(QDomElement* parentElement);

private:
    QString name_;
    bool enumerationValues_;
};

