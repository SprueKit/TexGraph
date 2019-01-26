#include "NormalMapNodes.h"

#include "../Core/Context.h"

namespace SprueEngine
{

    void NormalMapDeviation::Register(Context* context)
    {
        COPY_PROPERTIES(GraphNode, NormalMapDeviation);
    }

    void NormalMapDeviation::Construct()
    {
        AddInput("Normal", TEXGRAPH_RGBA);
        AddOutput("Dev.", TEXGRAPH_FLOAT);
    }

    int NormalMapDeviation::Execute(const Variant& param)
    {
        RGBA in = GetInputSocket(0)->GetValue().getColorSafe(true);
        Vec3 vec = in.ToNormal();
        float deviation = fabsf(vec.Dot(Vec3(0, 1, 0)));
        GetOutputSocket(0)->StoreValue(deviation);
        return GRAPH_EXECUTE_COMPLETE;
    }

    void NormalMapNormalize::Register(Context* context)
    {
        context->CopyBaseProperties("TextureNode", "NormalMapNormalize");
    }

    void NormalMapNormalize::Construct()
    {
        AddInput("Normal", TEXGRAPH_RGBA);
        AddOutput("Fixed", TEXGRAPH_RGBA);
    }

    int NormalMapNormalize::Execute(const Variant& param)
    {
        RGBA in = GetInputSocket(0)->GetValue().getColorSafe(true);
        in.FromNormal(in.ToNormal().Normalized());
        GetOutputSocket(0)->StoreValue(in);
        return GRAPH_EXECUTE_COMPLETE;
    }

    void NormalMapTextureModifier::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "NormalMapTextureModifier");
        REGISTER_PROPERTY_MEMORY(NormalMapTextureModifier, float, offsetof(NormalMapTextureModifier, StepSize), 1.0f, "Step Size", "How many pixels to step", PS_TinyIncrement | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(NormalMapTextureModifier, float, offsetof(NormalMapTextureModifier, Power), 1.0f, "Power", "How sharp the output normals should be", PS_SmallIncrement | PS_Permutable);
    }

    void NormalMapTextureModifier::Construct()
    {
        AddInput("Height", TEXGRAPH_FLOAT);
        AddOutput("Color", TEXGRAPH_RGBA);
    }

    int NormalMapTextureModifier::Execute(const Variant& param)
    {
        Vec4 pos = param.getVec4Safe();
        const float stepScale = CalculateStepSize(StepSize, pos);

#define SAMPLE_HEIGHT_MAP(NAME, X, Y) ForceExecuteUpstreamOnly(pos + Vec4(X * stepScale, Y * stepScale, 0, 0)); float NAME = GetInputSocket(0)->GetValue().getFloatSafe(); 
        SAMPLE_HEIGHT_MAP(l, -1, 0);
        SAMPLE_HEIGHT_MAP(r, 1, 0);
        SAMPLE_HEIGHT_MAP(t, 0, -1);
        SAMPLE_HEIGHT_MAP(b, 0, 1);
        SAMPLE_HEIGHT_MAP(tl, -1, -1);
        SAMPLE_HEIGHT_MAP(tr, 1, -1);
        SAMPLE_HEIGHT_MAP(bl, -1, 1);
        SAMPLE_HEIGHT_MAP(br, 1, 1);
#undef SAMPLE_HEIGHT_MAP

        float dX = tr + 2 * r + br - tl - 2 * l - bl;
        float dY = bl + 2 * b + br - tl - 2 * t - tr;
        Vec3 normal = Vec3(dX, dY, Power).Normalized();
        normal = normal * 0.5f + 0.5f;
        RGBA asRGB(normal.x, normal.y, normal.z);
        GetOutputSocket(0)->StoreValue(asRGB);

        return GRAPH_EXECUTE_COMPLETE;
    }
}