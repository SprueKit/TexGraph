#include "TextureInspectorGenTask.h"

#include "Documents/TexGen/Controls/TextureInspector.h"

#include <EditorLib/Controls/ISignificantControl.h>

namespace SprueEditor
{

    TextureInspectorGenTask::TextureInspectorGenTask(SprueEngine::Graph* graph, SprueEngine::GraphNode* node, unsigned width, unsigned height) :
        TextureGenTask(graph, node, 0x0)
    {
        width_ = width;
        height_ = height;
    }

    void TextureInspectorGenTask::FinishTask()
    {
        if (TextureInspector* panel = ISignificantControl::GetControl<TextureInspector>())
        {
            panel->SetImage(generatedImage_.get());
        }
    }

}