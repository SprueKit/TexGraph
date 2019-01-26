#pragma once

#include <EditorLib/editorlib_global.h>

#include <QString>

/// Utility class that fetches GPU information when constructed.
class EDITORLIB_EXPORT VideoCard
{
public:
    /// Construct and query for data.
    VideoCard();

    /// Video memory (integrated GPUs encountered so far report as negative values for allocated system memory)
    QString videoMemory;
    /// Name string for the video card.
    QString cardName;
    /// Hardware error status messages
    QString errorStatus;
    /// Identifier for the device.
    QString deviceID;
    /// Driver version.
    QString deviceVersion;
    /// Description of the device
    QString description;
};