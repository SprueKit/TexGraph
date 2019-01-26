#include "TextureGraphApplication.h"

#include <EditorLib/Settings/Settings.h>
#include <EditorLib/Settings/SettingsBitNames.h>
#include <EditorLib/Settings/SettingsPage.h>
#include <EditorLib/Settings/SettingsValue.h>

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

namespace SprueEditor
{

    void TextureGraph_ConstructSettings(Settings* settings)
    {
        // General settings
        {
            SettingsPage* page = settings->CreatePage("General", "Configuration options that have no more suitable home");
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Show Quick Start Window?", "Determines whether to show the startup dialog", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Enable High-DPI scaling", "Changing DPI scaling mode requires restarting SprueKit for changes to take effect", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Use Ribbon UI", "Switches between ribbon and legacy UI display", QVariant(true), QVariant(true), QVariant() });
        }

        // Resource settings
        //{
        //    SettingsPage* page = settings->CreatePage("Resources", "Configure resource path locations");
        //    page->InitializeSetting(new SettingValue{ ST_PATH_LIST, "Parts folders", "Paths to folders containing parts that should be displayed in the parts library", QVariant(QStringList()), QVariant(), QVariant() });
        //    page->InitializeSetting(new SettingValue{ ST_PATH_LIST, "Reporting folders", "Paths to folders containing Sprue models that should be used for reporting purposes", QVariant(QStringList()), QVariant(), QVariant() });
        //}

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
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Use OpenCL meshing", "If checked OpenCL will be used for meshing acceleration", QVariant(true), QVariant(true), QVariant() });
            QStringList shadingStyleNames({ "MatCap Clay", "MatCap Jade", "Normals", "Object space position", "Legacy", "PBR" });
            page->InitializeSetting(new SettingValue{ ST_ENUM, "Shading style", "How meshes displayed in the viewport should be shaded", 0, 0, shadingStyleNames });

            QStringList meshingMethodNames({ "Naive Surface Nets", "Dual Contouring", "Blocks", "Dual Marching Cubes" });
            page->InitializeSetting(new SettingValue{ ST_ENUM, "Meshing Method", "How density fields are polygonzied", 0, 0, meshingMethodNames });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Voxel Edge Length", "How large a voxel is", 1.0f, 1.0f, QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Meshing Chunk Size", "value^3 voxels will be used per chunk", 64, 64, QVariant() });

            QStringList texCoordQualityNames({ "Automatic", "Fast", "Best" });
            page->InitializeSetting(new SettingValue{ ST_ENUM, "Texture coordinate quality", "Desired quality to use when generating texture coordinates", QVariant(1), QVariant(1), texCoordQualityNames });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Tex coord stretch", "Allowed 'stretching' distortion in generated UV coordinates", 0.5, 0.5, QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Tex coord charts", "Desired number of UV islands, if 0 only stretch is used and the islands are uncapped", 5, 5, QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Tex chart gutter", "Spacing alloted (in pixels) between UV islands", 2.0, 2.0, QVariant() });
        }

        // Search settings
        {
            SettingsPage* page = settings->CreatePage("Search", "Configuration for searching");
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Search properties", "If enabled all properties of a objects will be searched (slow)", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Search quick start guides", "Results from the quick start guides will be included (lower priority), may yield excessive results as well as results that are not useful to an experienced user", QVariant(true), QVariant(true), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Search help", "Results from help documentation will be included (lower priority)", QVariant(true), QVariant(true), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Search assets", "Assets in configured asset directories will be scanned (slow, but indexed)", QVariant(false), QVariant(false), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Search online answers", "Web queries will be made for answers in the online answers database", QVariant(false), QVariant(false), QVariant() });
        }

        // Backup settings
        {
            SettingsPage* page = settings->CreatePage("Auto Backup", "Configure timing and storage of automatic backups");
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Enable automatic backup", "If enabled backups will be executed per the time criteria below", QVariant(true), QVariant(true), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Backup every N minutes", "How frequently automatic backups will be performed", QVariant(5), QVariant(5), QVariant() });

            QString defaultBackup = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + "Backups");
            page->InitializeSetting(new SettingValue{ ST_PATH, "Write backups into", "Folder to write backups into", defaultBackup, defaultBackup, QVariant() });
        }

        // Secret export settings
        {
            SettingsPage* page = settings->CreatePage("Texture Graph Export", "Settings for the export of textures, secret");
            page->SetSecret(true);

            page->InitializeSetting(new SettingValue{ ST_PATH, "Export To", "Folder to export images into", QString(), QString(), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Naming Convention", "Determines how to name the files exported", QString("%1_%2"), QString("%1_%2"), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Use Short Names", "Whether to use short affix names in exported files", false, false, QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Export Format", "Type of format to use for image export", 0, 0, QVariant() });
        }

        // Secret Texture reporting settings
        {
            SettingsPage* page = settings->CreatePage("Texture Report Settings", "Settings for report generation dialog");
            page->SetSecret(true);

            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Report Tab", "Index of tab to use", 0, 0, QVariant() });

            // Single-report
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Single Report File", "File to run the folder on", QString(), QString(), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Single Detailed", "Make the report in detail or brief", true, true, QVariant() });

            // Multi-report
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Multi Report Folder", "Folder to process", QString(), QString(), QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Multi Detailed", "Make the report in detail or brief mode", true, true, QVariant() });
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Multi Recurse", "Recurse through all folders", true, true, QVariant() });
        }

        // Common GUI secret settings
        {
            SettingsPage* page = settings->CreatePage("Secret GUI State", "Secretive settings that contain GUI state");
            page->SetSecret(true);
            page->InitializeSetting(new SettingValue{ ST_VARIANT_TYPE, "Ribbon Collapsed", "", false, false, QVariant() });
        }

        // Flags
        {
            settings->AddBitNames(new SettingsBitNames("Permutation Flags", false, 32));
            settings->AddBitNames(new SettingsBitNames("Capabilities", false, 32));
            settings->AddBitNames(new SettingsBitNames("Flags", false, 32));
        }
    }

}