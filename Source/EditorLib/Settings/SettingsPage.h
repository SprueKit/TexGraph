#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <QDomElement>
#include <QString>
#include <QVariant>
#include <QXmlStreamWriter>

#include <vector>

class Settings;

/// Contains a set of settings for something, such as a document type. Direct manipulation of the underly std::map is deliberately prevented.
/// For GUI population reasons it is always preferable to prefil the list of settings with defaults instead of relying on the default functions.
class EDITORLIB_EXPORT SettingsPage : private std::vector<SettingValue*>
{
public:
    SettingsPage(const QString& pageName, const QString& pageTip, Settings* owner);

    void InitializeSetting(SettingValue* value);

    const QString& GetName() const { return pageName_; }
    const QString& GetTip() const { return pageTip_; }

    SettingValue* Get(const QString& key);
    QVariant GetValue(const QString& key) { return Get(key)->value_; }
    void SetValue(const QString& key, QVariant value);

    using std::vector<SettingValue*>::begin;
    using std::vector<SettingValue*>::end;
    using std::vector<SettingValue*>::size;

    typedef std::vector<SettingValue*>::iterator iterator;
    typedef std::vector<SettingValue*>::const_iterator const_iterator;

    void Save(QXmlStreamWriter* parentElement);
    void Restore(QDomElement* parentElement);

    bool IsSecret() const { return isSecret_; }
    void SetSecret(bool secrecy) { isSecret_ = secrecy; }

private:
    QString pageName_;
    QString pageTip_;
    Settings* owner_;
    bool isSecret_ = false;
};
