#pragma once

#include <EditorLib/DocumentBase.h>

namespace SprueEditor
{

/// Identifier for a scene that is the main scene of view
#define SCENE_DOC_SCENE 0
/// Identifier for a scene that is likely a UV-map or other type of chart.
#define SCENE_DOC_CHART 1

    /// Base class for a document that contains Urho3D Scenes.
    class UrhoSceneDocument
    {
    public:
    };

}