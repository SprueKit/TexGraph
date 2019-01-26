#include "Settings.h"

#include "SettingsPage.h"
#include "SettingsBitNames.h"

#include <qstandardpaths.h>
#include <qfile.h>
#include <qxmlstream.h>
#include <QDomDocument>


Settings* Settings::instance_ = 0x0;

Settings::Settings()
{
    instance_ = this;
}

void Settings::RestoreSettings()
{
    QXmlStreamReader reader;
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    // application doesn't exist yet so our names are all wrong.
    basePath += "/settings.xml";
    if (!QFile::exists(basePath))
        return;

    QFile file(basePath);

    QDomDocument doc;
    if (!doc.setContent(&file))
        return;

    QDomElement docElem = doc.documentElement();
    if (docElem.tagName() != "settings")
        return;

    QDomElement pages = docElem.firstChildElement("page");
    while (!pages.isNull())
    {
        QString pageName = pages.attribute("name");
        if (SettingsPage* page = GetPage(pageName))
            page->Restore(&pages);
        pages = pages.nextSiblingElement("page");
    }

    QDomElement bitNames = docElem.firstChildElement("bitnames");
    while (!bitNames.isNull())
    {
        QString name = bitNames.attribute("key");
        if (SettingsBitNames* names = GetBitNames(name))
            names->Restore(&bitNames);

        bitNames = bitNames.nextSiblingElement("bitnames");
    }
}

void Settings::SaveSettings()
{
    QXmlStreamWriter writer;
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    basePath += "/settings.xml";
    QFile file(basePath);

    if (!file.open(QIODevice::WriteOnly))
        return;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("settings");

    for (auto page : pages_)
        page->Save(&writer);

    for (auto nameList : bitNames_)
        nameList->Save(&writer);

    writer.writeEndElement();

    writer.writeEndDocument();
}

SettingsPage* Settings::CreatePage(const QString& pageName, const QString& pageTip)
{
    SettingsPage* page = new SettingsPage(pageName, pageTip, this);
    pages_.push_back(page);
    return page;
}

SettingsPage* Settings::GetPage(const QString& name) const
{
    // First check for possibility of a partial page.
    QString nm = name;
    if (name.contains('/'))
        nm = name.split('/').first();

    for (auto page : pages_)
        if (page->GetName().compare(nm) == 0)
            return page;
    return 0x0;
}

void Settings::AddBitNames(SettingsBitNames* nameList)
{
    bitNames_.push_back(nameList);
}

SettingsBitNames* Settings::GetBitNames(const QString& nameList)
{
    for (auto names : bitNames_)
        if (names->GetName().compare(nameList) == 0)
            return names;
    return 0x0;
}

SettingValue* Settings::GetValue(const QString& path) const
{
    QStringList terms = path.split('/', QString::SplitBehavior::SkipEmptyParts);
    if (terms.size() == 2)
    {
        if (auto page = GetPage(terms[0]))
            return page->Get(terms[1]);
    }
    return 0x0;
}

std::pair<SettingsPage*, SettingValue*> Settings::GetPageAndValue(const QString& path) const
{
    QStringList terms = path.split('/', QString::SplitBehavior::SkipEmptyParts);
    if (terms.size() == 2)
    {
        if (auto page = GetPage(terms[0]))
            return std::make_pair(page, page->Get(terms[1]));
    }
    return std::pair<SettingsPage*, SettingValue*>(0x0, 0x0);
}

void Settings::SetValue(const QString& path, QVariant value)
{
    QStringList terms = path.split('/', QString::SplitBehavior::SkipEmptyParts);
    if (terms.size() == 2)
    {
        if (auto page = GetPage(terms[0]))
            page->Get(terms[1])->value_ = value;
    }
}

void Settings::NotifyBitNamesChanged(const QString& bitName)
{
    emit BitnamesChanged(this);
}