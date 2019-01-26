#include "FileHelpers.h"

#include <QXmlStream.h>

bool CheckXMLRootElement(const QString& filePath, const QString& rootName)
{
    QFile file(filePath);
    if (file.exists())
        return CheckXMLRootElement(&file, rootName);
    return false;
}

bool CheckXMLRootElement(QFile* file, const QString& rootName)
{
    if (!file || !file->exists())
        return false;

    if (!file->isOpen())
        file->open(QFile::ReadOnly | QFile::Text);
    if (!file->isOpen())
        return false;

    QXmlStreamReader rdr(file);
    if (rdr.hasError())
        return false;
    if (rdr.atEnd())
        return false;

    if (rdr.readNextStartElement())
    {
        if (rdr.name().compare(rootName, Qt::CaseInsensitive) == 0)
            return true;
    }

    return false;
}