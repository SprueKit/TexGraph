#include "BlurNodes.h"

#include "../Core/Context.h"

namespace SprueEngine
{

void BlurModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "BlurModifier");
    REGISTER_PROPERTY_MEMORY(BlurModifier, int, offsetof(BlurModifier, BlurRadius), 3, "Kernel Radius", "Size of the convolution kernel", PS_Default);
    REGISTER_PROPERTY_MEMORY(BlurModifier, float, offsetof(BlurModifier, BlurStepSize), 1.0f, "Pixel Step", "How many pixels are between each cell of the convolution kernel", PS_Default);
    REGISTER_PROPERTY_MEMORY(BlurModifier, float, offsetof(BlurModifier, Sigma), 5.5f, "Power", "Intensity of the blur's mixing", PS_SmallIncrement);
}

void BlurModifier::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);

    AddOutput("Color", TEXGRAPH_RGBA);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int BlurModifier::Execute(const Variant& param)
{
    Vec4 coord = param.getVec4Safe();
    Vec2 pos = param.getVec2Safe();

    const float stepping = CalculateStepSize(BlurStepSize, coord);
    std::vector<float> kernel;
    kernel.reserve(BlurRadius*BlurRadius * 2);
    CalculateKernel(kernel);

    RGBA sum;
    if (GetInputSocket(0)->HasConnections())
    {
        int blurHalf = BlurRadius / 2;
        for (int y = -blurHalf; y <= blurHalf; ++y)
            for (int x = -blurHalf; x <= blurHalf; ++x)
            {
                ForceExecuteUpstreamOnly(Vec2(pos.x + x * stepping, pos.y + y * stepping));

                RGBA value = GetInputSocket(0)->GetValue().getColorSafe(true);
                sum += value * kernel[ToArrayIndex(x + blurHalf, y + blurHalf, 0, BlurRadius, BlurRadius, 1)];
            }
    }

    GetOutputSocket(0)->StoreValue(sum);
    GetOutputSocket(1)->StoreValue(sum.r);

    return GRAPH_EXECUTE_COMPLETE;
}

#define EULERS_NUMBER 2.71828182846f

void BlurModifier::CalculateKernel(std::vector<float>& target)
{
    float sum = 0;

    int kernelRadius = BlurRadius / 2;

    for (int filterY = -kernelRadius; filterY <= kernelRadius; ++filterY)
    {
        for (int filterX = -kernelRadius; filterX <= kernelRadius; ++filterX)
        {
            float distance = ((filterX * filterX) + (filterY * filterY)) / (2 * (Sigma * Sigma));

            float value = EULERS_NUMBER * expf(-distance);
            target.push_back(value);
            sum += value;
        }
    }

    float inverseSum = 1.0f / sum;
    for (int y = 0; y < BlurRadius; ++y)
    {
        for (int x = 0; x < BlurRadius; ++x)
            target[ToArrayIndex(x, y, 0, BlurRadius, BlurRadius, 1)] *= inverseSum;
    }
}

void StreakModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "StreakModifier");
    REGISTER_PROPERTY_MEMORY(StreakModifier, float, offsetof(StreakModifier, StreakAngle), 0, "Angle", "Angular direction for the streak", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(StreakModifier, float, offsetof(StreakModifier, StreakLength), 1, "Length", "How long each streak should be", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(StreakModifier, unsigned, offsetof(StreakModifier, Samples), 3, "Samples", "Number of samples to take per streak", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(StreakModifier, bool, offsetof(StreakModifier, FadeOff), true, "Fade Off", "Samples will progressively contribute less", PS_Default | PS_Permutable);
}

void StreakModifier::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int StreakModifier::Execute(const Variant& param)
{
    RGBA sum;

    Vec2 pos = param.getVec2Safe();
    Vec2 angleVec = Vec2::PositiveY.Rotate(StreakAngle).Normalized();

    float fade = Samples > 0 ? 1.0f / Samples : 1.0f;
    for (unsigned i = 0; i < Samples; ++i)
    {
        ForceExecuteUpstreamOnly(pos + angleVec * StreakLength * fade * i);
        sum += GetInputSocket(0)->GetValue().getColorSafe(true) * (FadeOff ? (fade * (Samples - i)) : 1.0f);
    }

    if (Samples > 0)
        sum *= (1.0f / (float)Samples);
    GetOutputSocket(0)->StoreValue(sum);

    return GRAPH_EXECUTE_COMPLETE;
}


void AnisotropicBlur::Register(Context* context)
{
    context->CopyBaseProperties("BlurModifier", "AnisotropicBlur");
}

void AnisotropicBlur::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);

    AddOutput("Color", TEXGRAPH_RGBA);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int AnisotropicBlur::Execute(const Variant& param)
{
    Vec4 pos = param.getVec4Safe();

    const float stepSize = CalculateStepSize(BlurStepSize, pos);

#define SAMPLE_SOBEL(NAME, X, Y) ForceExecuteUpstreamOnly(pos + Vec4(X + stepSize, Y + stepSize, 0, 0)); float NAME = GetInputSocket(0)->GetValue().getFloatSafe(); 
    SAMPLE_SOBEL(l, -1, 0);
    SAMPLE_SOBEL(r, 1, 0);
    SAMPLE_SOBEL(t, 0, -1);
    SAMPLE_SOBEL(b, 0, 1);
    SAMPLE_SOBEL(tl, -1, -1);
    SAMPLE_SOBEL(tr, 1, -1);
    SAMPLE_SOBEL(bl, -1, 1);
    SAMPLE_SOBEL(br, 1, 1);
#undef SAMPLE_HEIGHT_MAP

    const float dX = tr + 2 * r + br - tl - 2 * l - bl;
    const float dY = bl + 2 * b + br - tl - 2 * t - tr;
    const float weight = 1.0f - (dX * dY);

    std::vector<float> kernel;
    kernel.reserve(BlurRadius*BlurRadius * 2);
    CalculateKernel(kernel);

    ForceExecuteUpstreamOnly(pos);
    RGBA centerValue = GetInputSocket(0)->GetValue().getColorSafe(true);

    RGBA sum;
    if (GetInputSocket(0)->HasConnections())
    {
        int blurHalf = BlurRadius / 2;
        for (int y = -blurHalf; y <= blurHalf; ++y)
            for (int x = -blurHalf; x <= blurHalf; ++x)
            {
                ForceExecuteUpstreamOnly(Vec2(pos.x + x * stepSize, pos.y + y * stepSize));

                RGBA value = GetInputSocket(0)->GetValue().getColorSafe(true);
                sum += value * kernel[ToArrayIndex(x + blurHalf, y + blurHalf, 0, BlurRadius, BlurRadius, 1)];
            }
    }

    sum = SprueLerp(centerValue, sum, weight);
    GetOutputSocket(0)->StoreValue(sum);
    GetOutputSocket(1)->StoreValue(sum.r);

    return GRAPH_EXECUTE_COMPLETE;
}

}