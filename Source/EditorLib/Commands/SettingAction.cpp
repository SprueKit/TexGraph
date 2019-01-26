#include "SettingAction.h"

#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsValue.h>

SettingAction::SettingAction(const QString& setting, const QVariant& compValue, const QString& actionClass, const QString& text, QObject* owner) :
    RegisteredAction(actionClass, text, owner),
    setValue_(compValue)
{
    InitValue(setting);
    setCheckable(true);
}

SettingAction::SettingAction(const QString& setting, const QVariant& compValue, const QString& actionClass, const QIcon& icon, const QString& text, QObject* owner) :
    RegisteredAction(actionClass, icon, text, owner),
    setValue_(compValue)
{
    InitValue(setting);
    setCheckable(true);
}

SettingAction::~SettingAction()
{

}

void SettingAction::InitValue(const QString& setting)
{
    value_ = Settings::GetInstance()->GetValue(setting);
    Q_ASSERT(value_);
    connect(this, &QAction::toggled, [=](bool state) {
        if (state)
        {
            value_->value_ = setValue_;
            value_->Notify();
        }
    });
}

void SettingAction::CheckCmdUI()
{
    Q_ASSERT(value_);
    
    if (value_)
    {
        blockSignals(true);
        RegisteredAction::CheckCmdUI();
        setChecked(value_->value_ == setValue_);
        blockSignals(false);
    }
}