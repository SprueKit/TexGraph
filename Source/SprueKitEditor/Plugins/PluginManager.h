#pragma once

#include <QString>

#include <vector>

namespace SprueEditor
{
    /// Manages the loading and roster information of plugins for the SprueEditor framework.
    class PluginManager
    {
    public:
        struct PluginInfo
        {
            /// Pretty name of the plugin.
            QString name_;
            /// Version string.
            QString versionInfo_;
            /// Path to the DLL file for the plugin.
            QString dllPath_;
        };

    private:
        
    };

}