#pragma once

#include <EditorLib/editorlib_global.h>

#include <QString>

#include <map>

/// Contains a string table for mapping English strings to a translated string.
class EDITORLIB_EXPORT LanguageData
{
protected:
    /// Protected default constructor only intended for specialized usage (translation key recording, etc).
    LanguageData() { }
public:
    /// Construct the language data from an XML file at the given path.
    LanguageData(const QString& filePath);

    /// Request for a localized string, will return the input string if a localized version could not be found. Virtual is used for "recording" requests.
    virtual QString Localize(const QString& input) const;

    /// Saving exists for building initial language databases.
    void Save(const QString& filePath) const;

protected:
    /// String table.
    mutable std::map<QString, QString> translation_;
};