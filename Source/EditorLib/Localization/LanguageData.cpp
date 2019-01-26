#include "LanguageData.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QFile>
#include <QXmlStreamWriter>
#include <QDomDocument>

LanguageData::LanguageData(const QString& filePath)
{
    QString basePath = QCoreApplication::applicationDirPath();
    basePath += "/";
    basePath += filePath;

    QXmlStreamReader reader;
    QFile file(basePath);
    QDomDocument doc;
    if (!doc.setContent(&file))
        return;

    QDomElement docElem = doc.documentElement();
    if (docElem.tagName() != "language")
        return;
}

QString LanguageData::Localize(const QString& input) const
{
    auto found = translation_.find(input);
    if (found != translation_.end())
        return found->second;
    return input;
}

void LanguageData::Save(const QString& path) const
{

}