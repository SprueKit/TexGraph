#pragma once

#include "TextureGenTask.h"

namespace SprueEditor
{
    
    class TextureInspectorGenTask : public TextureGenTask
    {
    public:
        TextureInspectorGenTask(SprueEngine::Graph* graph, SprueEngine::GraphNode* node, unsigned width, unsigned height);

        virtual void FinishTask() override;
        virtual bool Supercedes(Task* other) {
            if (TextureInspectorGenTask* rhs = dynamic_cast<TextureInspectorGenTask*>(other))
                return rhs->nodeID_ == nodeID_;
            return false;
        }
    };
}