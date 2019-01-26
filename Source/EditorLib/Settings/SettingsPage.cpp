
#include "SettingsPage.h"
#include "Settings.h"


SettingsPage::SettingsPage(const QString& pageName, const QString& pageTip, Settings* owner) :
    owner_(owner),
    pageName_(pageName),
    pageTip_(pageTip)
{

}

void SettingsPage::InitializeSetting(SettingValue* setting)
{
    push_back(setting);
}

SettingValue* SettingsPage::Get(const QString& key)
{
    for (auto setting : *this)
        if (setting->name_.compare(key) == 0)
            return setting;
    return 0x0;
}

void SettingsPage::SetValue(const QString& key, QVariant value)
{
    auto val = Get(key);
    val->value_ = value;
    val->Notify();
}

void SettingsPage::Save(QXmlStreamWriter* writer)
{
    writer->writeStartElement("page");
    writer->writeAttribute("name", GetName());
    for (auto property : *this)
    {
        writer->writeStartElement("property");
        writer->writeAttribute("name", property->name_);
        writer->writeStartElement("value");
        // Apparently Qt doesn't do anything sensible like delimiting with standard illegal characters, even though ASCII control characters exist for this purpose.
        if (property->defaultValue_.type() == QVariant::StringList)
        {
            for (auto str : property->value_.toStringList())
            {
                writer->writeStartElement("val");
                writer->writeCharacters(str);
                writer->writeEndElement();
            }
        }
        else
            writer->writeCharacters(property->value_.toString());
        writer->writeEndElement();
        writer->writeEndElement();
    }

    writer->writeEndElement();
}

void SettingsPage::Restore(QDomElement* parentElement)
{
    if (parentElement && !parentElement->isNull())
    {
        QDomElement child = parentElement->firstChildElement("property");
        while (!child.isNull())
        {
            QString propertyName = child.attribute("name");
            if (auto foundProperty = Get(propertyName))
            {
                QDomElement value = child.firstChildElement("value");
                if (!value.isNull())
                {
                    if (foundProperty->defaultValue_.type() == QVariant::StringList)
                    {
                        QDomElement row = value.firstChildElement("val");
                        QStringList vals;
                        while (!row.isNull())
                        {
                            vals.push_back(row.text());
                            row = row.nextSiblingElement("val");
                        }
                        foundProperty->value_ = QVariant(vals);
                        foundProperty->Notify();
                    }
                    else
                    {
                        foundProperty->value_ = QVariant(value.text());
                        foundProperty->Notify();
                    }
                }
                value = child.firstChildElement("options");
                if (!value.isNull())
                    foundProperty->options_ = QVariant(value.text());
            }

            child = child.nextSiblingElement("property");
        }
    }
}
