#include "UrhoEditorApplication.h"

#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsBitNames.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

namespace UrhoEditor
{

    void Urho3D_ConstructSettings(Settings* settings)
    {
        // General settings
        {
            SettingsPage* page = settings->CreatePage("General", "Configuration options that have no more suitable home");
            page->InitializeSetting(new SettingValue { ST_VARIANT_TYPE, "Show Quick Start Window?", "Determines whether to show the startup dialog", QVariant(true), QVariant(true), QVariant() });
            page->InitializeSetting(new SettingValue { ST_VARIANT_TYPE, "Enable High-DPI scaling", "Changing DPI scaling mode requires restarting UrhoEditor for changes to take effect", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue { ST_VARIANT_TYPE, "Use Ribbon UI", "Switches between ribbon and legacy UI display", QVariant(true), QVariant(true), QVariant() });
        }

        // Viewport settings
        {
            SettingsPage* page = settings->CreatePage("Viewport", "3D/2D viewport control settings");
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Move view forward", "Moves the camera forward", QVariant(QKeySequence("W")), QVariant(QKeySequence("W")), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Move view backward", "Moves the camera backward", QVariant(QKeySequence("S")), QVariant(QKeySequence("S")), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Move view left", "Moves the camera left (strafe)", QVariant(QKeySequence("A")), QVariant(QKeySequence("A")), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Move view right", "Moves the camera right (strafe)", QVariant(QKeySequence("D")), QVariant(QKeySequence("D")), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Move view up", "Moves the camera upward", QVariant(QKeySequence("E")), QVariant(QKeySequence("E")), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Move view down", "Moves the camera downward", QVariant(QKeySequence("C")), QVariant(QKeySequence("C")), QVariant() });

            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Invert mouse Y", "Will flip the Y direction of mouse movement", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Translate mode", "Whether translation mode is active", QVariant(true), QVariant(true), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Rotate mode", "Whether rotation mode is active", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Scale mode", "Whether scaling mode is active", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Axial mode", "Whether axial rotation mode is active", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Local transform", "Whether local transforms are used", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Snap position", "Whether snapping position is active", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Snap position spacing", "How many units snapping is spaced", QVariant(1.0), QVariant(1.0), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Snap rotation", "Whether snapping rotation is active", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Snap rotation degrees", "How many degrees snapping is spaced", QVariant(45.0), QVariant(45.0), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Translate speed", "How quickly position changes happen", QVariant(1.0), QVariant(1.0), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Rotate speed", "How quickly rotation changes happen", QVariant(1.0), QVariant(1.0), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Scaling speed", "How quickly scaling changes happen", QVariant(1.0), QVariant(1.0), QVariant() });
        }

        // Graphics settings
        {
            SettingsPage* page = settings->CreatePage("Graphics", "Graphics and GPU options");
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Desired Framerate", "Maximum FPS desired", QVariant(30), QVariant(30), QVariant() });
        }
    }

}