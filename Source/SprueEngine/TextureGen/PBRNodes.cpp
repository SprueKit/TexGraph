#include "PBRNodes.h"

#include <SprueEngine/Core/Context.h>

namespace SprueEngine
{

    void PBRAlbedoEnforcerNode::Register(Context* context)
    {
        COPY_PROPERTIES(GraphNode, PBRAlbedoEnforcerNode);
        REGISTER_PROPERTY_MEMORY(PBRAlbedoEnforcerNode, bool, offsetof(PBRAlbedoEnforcerNode, AlertMode), false, "Alert Mode", "Results will display red where values are too low and green where too high", PS_VisualConsequence);
        REGISTER_PROPERTY_MEMORY(PBRAlbedoEnforcerNode, bool, offsetof(PBRAlbedoEnforcerNode, StrictMode), false, "Strict Mode", "Raiser the lower bound from 30 to 50", PS_VisualConsequence);
    }

    void PBRAlbedoEnforcerNode::Construct()
    {
        AddInput("", TEXGRAPH_RGBA);
        AddOutput("", TEXGRAPH_RGBA);
    }

    int PBRAlbedoEnforcerNode::Execute(const Variant& param)
    {
        RGBA input = GetInputSocket(0)->GetValue().getColorSafe(true);
        const float lowerBound = StrictMode ? 50.0f / 255.0f : 30.0f / 255.0f;
        const float upperBound = 240.0f / 255.0f;

        bool anyLow = false;
        bool anyHigh = false;

#define TEST_ALBEDO(CHANNEL) if (input.CHANNEL < lowerBound) { anyLow = true; input.CHANNEL = lowerBound; } else if (input.CHANNEL > upperBound) { anyHigh = true; input.CHANNEL = upperBound; }
        TEST_ALBEDO(r);
        TEST_ALBEDO(g);
        TEST_ALBEDO(b);

        if (AlertMode)
        {
            if (anyLow)
                input = RGBA::Red;
            else if (anyHigh)
                input = RGBA::Green;
        }

        GetOutputSocket(0)->StoreValue(input);
        return GRAPH_EXECUTE_COMPLETE;
    }

}