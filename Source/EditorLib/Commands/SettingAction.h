#pragma once

#include <EditorLib/Commands/RegisteredAction.h>
#include <EditorLib/Settings/SettingsValue.h>

/// RegisteredAction that is linked to the value of a setting.
/// Checked status is based on the compValue and when triggered it will
/// set the SettingValue to match the compValue.
class EDITORLIB_EXPORT SettingAction : public RegisteredAction
{
public:
    SettingAction(const QString& setting, const QVariant& compValue, const QString& actionClass, const QString& text, QObject* owner = 0x0);
    SettingAction(const QString& setting, const QVariant& compValue, const QString& actionClass, const QIcon& icon, const QString& text, QObject* owner = 0x0);
    virtual ~SettingAction();

    /// Invokes baseclass and does a QVariant::compare of the setting value to the compValue.
    virtual void CheckCmdUI() override;

private:
    void InitValue(const QString& setting);

    QVariant setValue_;
    SettingValue* value_;
};