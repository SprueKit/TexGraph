#include "SettingsBitNames.h"



SettingsBitNames::SettingsBitNames(const QString& name, bool enumValues, unsigned bitCount) :
    name_(name),
    enumerationValues_(enumValues)
{
    for (unsigned i = 0; i < bitCount; ++i)
        push_back("");
}

void SettingsBitNames::Save(QXmlStreamWriter* writer)
{
    writer->writeStartElement("bitnames");
    writer->writeAttribute("key", name_);
    writer->writeAttribute("size", QString(size()));
    writer->writeAttribute("enum", enumerationValues_ ? "true" : "false");

    for (unsigned i = 0; i < size(); ++i)
    {
        writer->writeStartElement("flag");
        writer->writeAttribute("bit", QString::number(i));
        writer->writeAttribute("text", (*this)[i]);
        writer->writeEndElement();
    }

    writer->writeEndElement();
}

QString SettingsBitNames::GetBitName(int index)
{
    QString ret = at(index);
    if (ret.isEmpty())
        return QString("Bit %1").arg(index + 1);
    return ret;
}

void SettingsBitNames::Restore(QDomElement* parentElement)
{
    if (parentElement && !parentElement->isNull())
    {
        QDomElement child = parentElement->firstChildElement("flag");
        unsigned idx = 0;
        while (!child.isNull())
        {
            unsigned thisIdx = child.attribute("bit").toInt();
            QString text = child.attribute("text");
            (*this)[thisIdx] = text;

            child = child.nextSiblingElement("flag");
            ++idx;
        }
    }
}

