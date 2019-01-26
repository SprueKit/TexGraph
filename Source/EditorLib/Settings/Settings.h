#pragma once

#include <EditorLib/editorlib_global.h>

#include <QColor>
#include <QKeySequence>
#include <QObject>
#include <QVariant>


class SettingsPage;
class SettingValue;

/// Encapsulates the application wide settings. Settings must be constructed before the program is launched and before saved settings are read. 
/// Saved settings work against the settings that are defined at construction.
class EDITORLIB_EXPORT Settings : public QObject
{
    friend class SettingsPage;
    friend class SettingsBitNames;
    Q_OBJECT
public:
    /// Construct.
    Settings();

    /// Loads settings from a file in the application data directory.
    void RestoreSettings();
    /// Saves settings to a file in the application data directory.
    void SaveSettings();

    /// Returns the static signleton instances.
    static Settings* GetInstance() { return instance_; }

    /// During initialization of settings this creates a new page.
    SettingsPage* CreatePage(const QString& pageName, const QString& pageTip);
    /// Returns an existing page (or 0x0) by name.
    SettingsPage* GetPage(const QString& name) const;
    /// Returns the list of all pages in the settings.
    std::vector<SettingsPage*>& GetPages() { return pages_; }
    /// Returns the list of all pages in the settings.
    const std::vector<SettingsPage*>& GetPages() const { return pages_; }

    /// Fetches a SettingValue with a "{Page Name}/{Setting Name}" format
    SettingValue* GetValue(const QString& path) const;
    /// Returns both the containing page and the setting value if found from a "{Page Name}/{Setting Name}" format.
    std::pair<SettingsPage*, SettingValue*> GetPageAndValue(const QString& path) const;
    /// Sets a SettingValue using a similar path format as above.
    void SetValue(const QString& path, QVariant value);

    /// Adds a set of bit-flag names to the settings for initialization.
    void AddBitNames(SettingsBitNames* nameList);
    /// Gets a set of bit-flag names by value.
    SettingsBitNames* GetBitNames(const QString& nameList);
    /// Returns all bit-flag names in the settings.
    std::vector<SettingsBitNames*>& GetAllBitNames() { return bitNames_; }
    /// Returns all bit-flag names in the settings.
    const std::vector<SettingsBitNames*>& GetAllBitNames() const { return bitNames_; }
    /// Emits the SettingsChanged signal for the given bitfield name.
    void NotifyBitNamesChanged(const QString& bitName);

signals:
    void BitnamesChanged(Settings* settings);

private:
    /// Customizable names for scene object flags
    std::vector<SettingsPage*> pages_;
    /// List of bitflag field names.
    std::vector<SettingsBitNames*> bitNames_;
    /// Singleton instance.
    static Settings* instance_;
};