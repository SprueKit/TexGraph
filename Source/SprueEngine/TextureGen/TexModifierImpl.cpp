#include "TexModifierImpl.h"

#include <SprueEngine/Core/Context.h>

namespace SprueEngine
{
    
    // Helper to minimize typo risks
#define GENERIC_REGISTER(NAME) void NAME :: Register(Context* context) { context->CopyBaseProperties("GraphNode", #NAME); }

///=================================================
/// Invert color
///=================================================

GENERIC_REGISTER(InvertTextureModifier);

void InvertTextureModifier::Construct()
{
    AddInput("In", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int InvertTextureModifier::Execute(const Variant& parameter)
{
    Variant input = GetInputSocket(0)->GetValue();
    RGBA color = input.getColorSafe(true);
    color.r = 1.0f - color.r;
    color.g = 1.0f - color.g;
    color.b = 1.0f - color.b;

    GetOutputSocket(0)->StoreValue(color);
    
    return GRAPH_EXECUTE_COMPLETE;
}

void SolarizeTextureModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "SolarizeTextureModifier");
    REGISTER_PROPERTY_MEMORY(SolarizeTextureModifier, float, offsetof(SolarizeTextureModifier, Threshold), 0.5f, "Threshold", "Only values greater than the threshold will be solarized", PS_NormalRange | PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(SolarizeTextureModifier, bool, offsetof(SolarizeTextureModifier, InvertLower), true, "Invert <", "Whether solarization applies to values below or above the threshold", PS_Default | PS_Permutable);
}

void SolarizeTextureModifier::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int SolarizeTextureModifier::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    if (InvertLower)
    {
        color.r = color.r < Threshold ? 1.0f - color.r : color.r;
        color.g = color.g < Threshold ? 1.0f - color.g : color.g;
        color.b = color.b < Threshold ? 1.0f - color.b : color.b;
    }
    else
    {
        color.r = color.r > Threshold ? 1.0f - color.r : color.r;
        color.g = color.g > Threshold ? 1.0f - color.g : color.g;
        color.b = color.b > Threshold ? 1.0f - color.b : color.b;
    }
    GetOutputSocket(0)->StoreValue(color);
    return GRAPH_EXECUTE_COMPLETE;
}

void ConvolutionFilter::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "ConvolutionFilter");
    REGISTER_PROPERTY_MEMORY(ConvolutionFilter, Mat3x3, offsetof(ConvolutionFilter, Kernel), Mat3x3(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f), "Kernel", "Table of values for performing convolution", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(ConvolutionFilter, float, offsetof(ConvolutionFilter, StepSize), 1.0f, "Step Size", "How many pixels to step per convolution filter cell", PS_Default | PS_Permutable);
}

void ConvolutionFilter::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int ConvolutionFilter::Execute(const Variant& param)
{
    Vec4 pos = param.getVec4Safe();

    const float stepSize = CalculateStepSize(StepSize, pos);

    RGBA sum;
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            ForceExecuteUpstreamOnly(pos + Vec4(x * stepSize, y * stepSize, 0, 0));
            sum += GetInputSocket(0)->GetValue().getColorSafe(true) * Kernel.v[1 + x][1 + y];
        }
    }
    GetOutputSocket(0)->StoreValue(sum);
    return GRAPH_EXECUTE_COMPLETE;
}

void SharpenFilter::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "SharpenFilter");
    REGISTER_PROPERTY_MEMORY(SharpenFilter, float, offsetof(SharpenFilter, Power), 5.0f, "Power", "Intensity to sharpen by", PS_VisualConsequence | PS_SmallIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(SharpenFilter, float, offsetof(SharpenFilter, StepSize), 1.0f, "Step Size", "Number of pixels to step for the neighborhood testing", PS_VisualConsequence | PS_Permutable);
}

void SharpenFilter::Construct()
{
    AddInput("", TEXGRAPH_CHANNEL);
    AddOutput("", TEXGRAPH_CHANNEL);
}

int SharpenFilter::Execute(const Variant& param)
{
    const float center = 2.0f * Power;
    const float side = -0.25f * Power;
    Mat3x3 kernel(0, side, 0, side, center, side, 0, side, 0);
    Vec4 pos = param.getVec4Safe();

    const float stepping = CalculateStepSize(StepSize, pos);

    RGBA sum;
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            ForceExecuteUpstreamOnly(pos + Vec4(x * stepping, y * stepping, 0, 0));
            sum += GetInputSocket(0)->GetValue().getColorSafe(true) * kernel.v[1 + x][1 + y];
        }
    }
    GetOutputSocket(0)->StoreValue(sum);
    return GRAPH_EXECUTE_COMPLETE;
}

///=================================================
/// 4 Color Gradient ramp
///=================================================

void GradientRampTextureModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "GradientRampTextureModifier");
    REGISTER_PROPERTY_MEMORY(GradientRampTextureModifier, ColorRamp, offsetof(GradientRampTextureModifier, Gradient), ColorRamp(), "Gradient", "", PS_Default | PS_Permutable);
}

void GradientRampTextureModifier::Construct()
{
    AddInput("Pos", TEXGRAPH_CHANNEL);
    //OLD COS BASED GRADIENT: AddInput("Start", TEXGRAPH_CHANNEL);
    //OLD COS BASED GRADIENT: AddInput("1/3rd", TEXGRAPH_CHANNEL);
    //OLD COS BASED GRADIENT: AddInput("2/3rd", TEXGRAPH_CHANNEL);
    //OLD COS BASED GRADIENT: AddInput("End", TEXGRAPH_CHANNEL);

    AddOutput("Color", TEXGRAPH_CHANNEL);
}

int GradientRampTextureModifier::Execute(const Variant& param)
{
    float gradientValue = GetInputSocket(0)->GetValue().getFloatSafe();
    //OLD COS BASED GRADIENT: RGBA a = GetInputSocket(1)->GetValue().getColorSafe(true);
    //OLD COS BASED GRADIENT: RGBA b = GetInputSocket(1)->GetValue().getColorSafe(true);
    //OLD COS BASED GRADIENT: RGBA c = GetInputSocket(1)->GetValue().getColorSafe(true);
    //OLD COS BASED GRADIENT: RGBA d = GetInputSocket(1)->GetValue().getColorSafe(true);
    //OLD COS BASED GRADIENT: 
    //OLD COS BASED GRADIENT: RGBA firstResult = (c * gradientValue + d);
    //OLD COS BASED GRADIENT: firstResult.r = cosf(2.0f * PI * firstResult.r);
    //OLD COS BASED GRADIENT: firstResult.g = cosf(2.0f * PI * firstResult.g);
    //OLD COS BASED GRADIENT: firstResult.b = cosf(2.0f * PI * firstResult.b);
    //OLD COS BASED GRADIENT: firstResult.a = cosf(2.0f * PI * firstResult.a);
    //OLD COS BASED GRADIENT: 
    //OLD COS BASED GRADIENT: RGBA result = a + b * firstResult;

    RGBA result = Gradient.Get(gradientValue);
    GetOutputSocket(0)->StoreValue(result);

    return GRAPH_EXECUTE_COMPLETE;
}

void CurveTextureModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "CurveTextureModifier");
    REGISTER_PROPERTY_MEMORY(CurveTextureModifier, ColorCurves, offsetof(CurveTextureModifier, Curves), ColorCurves(), "Curves", "", PS_Default | PS_Permutable);
}

void CurveTextureModifier::Construct()
{
    AddInput("In", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int CurveTextureModifier::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    color = Curves.Curve(color);
    GetOutputSocket(0)->StoreValue(color);
    return GRAPH_EXECUTE_COMPLETE;
}

void SobelTextureModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "SobelTextureModifier");
    REGISTER_PROPERTY_MEMORY(SobelTextureModifier, float, offsetof(SobelTextureModifier, StepSize), 1.0f, "Step Size", "Number of pixels to step for the neighborhood test", PS_TinyIncrement | PS_Permutable);
}

void SobelTextureModifier::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_FLOAT);
}

int SobelTextureModifier::Execute(const Variant& param)
{
    Vec4 pos = param.getVec4Safe();

    const float stepping = CalculateStepSize(StepSize, pos);

#define SAMPLE_SOBEL(NAME, X, Y) ForceExecuteUpstreamOnly(pos + Vec4(X * stepping, Y * stepping, 0, 0)); float NAME = GetInputSocket(0)->GetValue().getFloatSafe(); 
    SAMPLE_SOBEL(l, -1, 0);
    SAMPLE_SOBEL(r, 1, 0);
    SAMPLE_SOBEL(t, 0, -1);
    SAMPLE_SOBEL(b, 0, 1);
    SAMPLE_SOBEL(tl, -1, -1);
    SAMPLE_SOBEL(tr, 1, -1);
    SAMPLE_SOBEL(bl, -1, 1);
    SAMPLE_SOBEL(br, 1, 1);
#undef SAMPLE_HEIGHT_MAP

    float dX = tr + 2 * r + br - tl - 2 * l - bl;
    float dY = bl + 2 * b + br - tl - 2 * t - tr;

    GetOutputSocket(0)->StoreValue(dX * dY);
    return GRAPH_EXECUTE_COMPLETE;
}

///=================================================
/// Highpass/Lowpass in One Node
///=================================================

void ClipTextureModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "ClipTextureModifier");
    REGISTER_PROPERTY_MEMORY(ClipTextureModifier, RangedFloat, offsetof(ClipTextureModifier, Range), RangedFloat(0.0f, 1.0f), "Range", "Any socket connections take priority over the min/max", PS_Default | PS_Permutable);
}

void ClipTextureModifier::Construct()
{
    AddInput("In", TEXGRAPH_CHANNEL);
    AddInput("Min", TEXGRAPH_CHANNEL);
    AddInput("Max", TEXGRAPH_CHANNEL);

    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int ClipTextureModifier::Execute(const Variant& param)
{
    RGBA value = GetInputSocket(0)->GetValue().getColorSafe(true);

    const bool leftHas = GetInputSocket(1)->HasConnections();
    const bool rightHas = GetInputSocket(2)->HasConnections();

    RGBA lowerBound = GetInputSocket(1)->GetValue().getColorSafe(true);
    RGBA upperBound = GetInputSocket(2)->GetValue().getColorSafe(true);

    if (!leftHas)
        lowerBound = RGBA(Range.getLowerBound(), Range.getLowerBound(), Range.getLowerBound(), Range.getLowerBound());
    if (!rightHas)
        upperBound = RGBA(Range.getUpperBound(), Range.getUpperBound(), Range.getUpperBound(), Range.getUpperBound());

    value.r = SprueMin(upperBound.r, SprueMax(lowerBound.r, value.r));
    value.g = SprueMin(upperBound.g, SprueMax(lowerBound.g, value.g));
    value.b = SprueMin(upperBound.b, SprueMax(lowerBound.b, value.b));
    value.a = SprueMin(upperBound.a, SprueMax(lowerBound.a, value.a));

    GetOutputSocket(0)->StoreValue(value);

    return GRAPH_EXECUTE_COMPLETE;
}

void EmbossModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "EmbossModifier");
    REGISTER_PROPERTY_MEMORY(EmbossModifier, float, offsetof(EmbossModifier, Angle), 0.0f, "Angle", "Angle of the light source for the emboss effect", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(EmbossModifier, float, offsetof(EmbossModifier, StepSize), 1.0f, "Step Size", "How many pixels to step for height estimate", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(EmbossModifier, float, offsetof(EmbossModifier, Bias), 0.5f, "Bias", "Constant factor added to the value", PS_SmallIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(EmbossModifier, float, offsetof(EmbossModifier, Power), 1.0f, "Power", "Inputs will be scaled by this value", PS_SmallIncrement | PS_Permutable);
}

void EmbossModifier::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int EmbossModifier::Execute(const Variant& param)
{
    Vec4 pos = param.getVec4Safe();

    const float stepping = CalculateStepSize(StepSize, pos);

    Vec2 angleVec = Vec2::PositiveY.Rotate(Angle).Normalized();
    RGBA sum(Bias,Bias,Bias);
    for (int y = -1; y <= 1; ++y)
    {
        if (y == 0)
            continue;
        for (int x = -1; x <= 1; ++x)
        {
            if (x == 0)
                continue;
            Vec4 sampleVec = pos + Vec4(x * stepping, y * stepping, 0, 0);
            ForceExecuteUpstreamOnly(sampleVec);
            Vec2 dirVec = (sampleVec.xy() - pos.xy()).Normalized();
            float dp = dirVec.Dot(angleVec);
            sum += GetInputSocket(0)->GetValue().getColorSafe(true) * dp * Power;
        }
    }

    GetOutputSocket(0)->StoreValue(sum);

    return GRAPH_EXECUTE_COMPLETE;
}

void TileModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "TileModifier");
    REGISTER_PROPERTY_MEMORY(TileModifier, Vec2, offsetof(TileModifier, Tiling), Vec2(2, 2), "Tiling", "", PS_Default | PS_Permutable);
}

void TileModifier::Construct()
{
    AddInput("Source", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int TileModifier::Execute(const Variant& param)
{
    Vec4 coord = param.getVec4Safe();
    coord.x = coord.x * Tiling.x;
    coord.y = coord.y * Tiling.y;
    coord.x -= floorf(coord.x);
    coord.y -= floorf(coord.y);

    ForceExecuteUpstreamOnly(coord);
    
    GetOutputSocket(0)->StoreValue(GetInputSocket(0)->GetValue().getColorSafe(true));

    return GRAPH_EXECUTE_COMPLETE;
}

void TransformModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "TransformModifier");
    REGISTER_PROPERTY_MEMORY(TransformModifier, Mat3x3, offsetof(TransformModifier, Matrix), Mat3x3(1, 0, 0, 0, 1, 0, 0, 0, 1), "Matrix", "", PS_Default | PS_Permutable);
}

void TransformModifier::Construct()
{
    Matrix = Mat3x3(1, 0, 0,
                    0, 1, 0,
                    0, 0, 1);
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Color", TEXGRAPH_RGBA);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

Variant TransformModifier::FilterParameter(const Variant& parameter) const
{
    Vec4 coord = parameter.getVec4Safe();
    Vec2 vec = parameter.getVec2Safe();
    Vec3 trans = Matrix * Vec3(vec.x, vec.y, 0);
    return Vec4(trans.x, trans.y, coord.z, coord.w);
}

int TransformModifier::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    GetOutputSocket(0)->StoreValue(color);
    GetOutputSocket(1)->StoreValue(color.r);
    return GRAPH_EXECUTE_COMPLETE;
}

void SimpleTransformModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "SimpleTransformModifier");
    REGISTER_PROPERTY_MEMORY(SimpleTransformModifier, Vec2, offsetof(SimpleTransformModifier, Offset), Vec2(), "Offset", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(SimpleTransformModifier, float, offsetof(SimpleTransformModifier, Rotation), 0.0f, "Rotation", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(SimpleTransformModifier, Vec2, offsetof(SimpleTransformModifier, Scale), Vec2(1, 1), "Scale", "", PS_Default | PS_Permutable);
}

void SimpleTransformModifier::Construct()
{
    AddInput("Source", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

Variant SimpleTransformModifier::FilterParameter(const Variant& parameter) const
{
    math::float3x3 transMat;
    transMat.SetTranslation(Offset);
    
    math::float3x3 rotMat;
    rotMat.SetRotation(Rotation);
    
    Mat3x3 sclMat(
        1, 0, 0,
        0, 1, 0,
        0, 0, 1);
    sclMat = sclMat.Scaled(Vec3(Scale.x, Scale.y, 1.0f));
    
    Vec4 truePt = parameter.getVec4Safe();
    Vec2 pt = parameter.getVec2Safe();
    Vec3 asVec3(pt.x, pt.y, 0.0f);
    Vec3 transformed = (sclMat * rotMat) * asVec3;
    transformed.x += Offset.x;
    transformed.y += Offset.y;
    return Vec4(transformed.x, transformed.y, truePt.z, truePt.w);
}

int SimpleTransformModifier::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    GetOutputSocket(0)->StoreValue(color);
    return GRAPH_EXECUTE_COMPLETE;
}

void CartesianToPolarModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "CartesianToPolarModifier");
    //REGISTER_PROPERTY_MEMORY(CartesianToPolarModifier, float, offsetof(CartesianToPolarModifier, velocity_), 0.0f, "Velocity", "Acceleration factor for creating an Archimedean spiral", PS_VisualConsequence);
    //REGISTER_PROPERTY_MEMORY(CartesianToPolarModifier, float, offsetof(CartesianToPolarModifier, spacing_), 1.0f, "Spacing Scale", "Defines that spacing for an Archimedean spiral", PS_VisualConsequence);
}

void CartesianToPolarModifier::Construct()
{
    AddInput("Source", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

Variant CartesianToPolarModifier::FilterParameter(const Variant& parameter) const
{
    Vec4 p = parameter.getVec4Safe();

    float R = sqrtf(p.x*p.x + p.y*p.y);
    float Theta = atan2(p.y, p.x);
    if (Theta < 0.0f)
        Theta += PI * 2;
    p.x = R;
    p.y = Theta;
    return p;
}

int CartesianToPolarModifier::Execute(const Variant& param)
{
    GetOutputSocket(0)->StoreValue(GetInputSocket(0)->GetValue());
    return GRAPH_EXECUTE_COMPLETE;
}

void PolarToCartesianModifier::Register(Context* context)
{
    COPY_PROPERTIES(GraphNode, PolarToCartesianModifier);
}

void PolarToCartesianModifier::Construct()
{
    AddInput("Source", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

Variant PolarToCartesianModifier::FilterParameter(const Variant& parameter) const
{
    Vec4 p = parameter.getVec4Safe();
    float X = p.x * cosf(p.y);
    float Y = p.x * sinf(p.y);
    p.x = X;
    p.y = Y;
    return p;
}

int PolarToCartesianModifier::Execute(const Variant& param)
{
    GetOutputSocket(0)->StoreValue(GetInputSocket(0)->GetValue());
    return GRAPH_EXECUTE_COMPLETE;
}

void DivModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "DivModifier");
    REGISTER_PROPERTY_MEMORY(DivModifier, float, offsetof(DivModifier, Fraction), 0.5f, "Fraction", "Where the divider is placed", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(DivModifier, bool, offsetof(DivModifier, Vertical), false, "Vertical", "Controls how the division is oriented", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(DivModifier, bool, offsetof(DivModifier, NormalizeCoordinates), false, "Normalize Coords.", "Upstream nodes will be reevaluated in the local space of each division", PS_Default | PS_Permutable);
}

void DivModifier::Construct()
{
    AddInput("Left", TEXGRAPH_CHANNEL);
    AddInput("Right", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int DivModifier::Execute(const Variant& param)
{
    Vec4 value = param.getVec4Safe();
    if (Vertical)
    {
        if (value.x < Fraction)
        {
            if (NormalizeCoordinates)
                value.x = NORMALIZE(value.x, 0.0f, Fraction);
            ForceExecuteUpstreamOnly(value);
            GetOutputSocket(0)->StoreValue(GetInputSocket(0)->GetValue());
        }
        else
        {
            if (NormalizeCoordinates)
                value.x = NORMALIZE(value.x, Fraction, 1.0f);
            ForceExecuteUpstreamOnly(value);
            GetOutputSocket(0)->StoreValue(GetInputSocket(1)->GetValue());
        }
    }
    else // horizontal
    {
        if (value.y < Fraction)
        {
            if (NormalizeCoordinates)
                value.y = NORMALIZE(value.y, 0.0f, Fraction);
            ForceExecuteUpstreamOnly(value);
            GetOutputSocket(0)->StoreValue(GetInputSocket(0)->GetValue());
        }
        else
        {
            if (NormalizeCoordinates)
                value.y = NORMALIZE(value.y, Fraction, 1.0f);
            ForceExecuteUpstreamOnly(value);
            GetOutputSocket(0)->StoreValue(GetInputSocket(1)->GetValue());
        }
    }
    return GRAPH_EXECUTE_COMPLETE;
}

void TrimModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "TrimModifier");
    REGISTER_PROPERTY_MEMORY(TrimModifier, float, offsetof(TrimModifier, TrimSize), 0.2f, "Trim Size", "The size of each piece of trim", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(TrimModifier, bool, offsetof(TrimModifier, Vertical), false, "Vertical", "Controls the orientation of the trim", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(TrimModifier, bool, offsetof(TrimModifier, NormalizeCoordinates), false, "Normalize Coords.", "Upstream nodes will be evaluated based on the pixel's position in each cell", PS_Default | PS_Permutable);
}

void TrimModifier::Construct()
{
    AddInput("Left Trim", TEXGRAPH_CHANNEL);
    AddInput("Center", TEXGRAPH_CHANNEL);
    AddInput("Right Trim", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int TrimModifier::Execute(const Variant& param)
{
    Vec4 coord = param.getVec4Safe();

    // do we need to ask for right edge?
    const bool hasRightEdge = GetInputSocket(2)->HasConnections();

    // Vertically oriented trim
    if (Vertical)
    {
        // inside left trim?
        if (coord.x <= TrimSize)
        {
            if (NormalizeCoordinates)
                coord.x = NORMALIZE(coord.x, 0.0f, TrimSize);
            ForceExecuteUpstreamOnly(coord);
            GetOutputSocket(0)->StoreValue(GetInputSocket(0)->GetValue());
        }
        else if (coord.x > 1.0f - TrimSize) // inside right trim?
        {
            if (NormalizeCoordinates)
                coord.x = NORMALIZE(coord.x, 0.0f, TrimSize);
            ForceExecuteUpstreamOnly(coord);
            GetOutputSocket(0)->StoreValue(hasRightEdge ? GetInputSocket(2)->GetValue() : GetInputSocket(0)->GetValue());
        }
        else // inside center
        {
            if (NormalizeCoordinates)
                coord.x = NORMALIZE(coord.x, TrimSize, 1.0f - TrimSize);
            ForceExecuteUpstreamOnly(coord);
            GetOutputSocket(0)->StoreValue(GetInputSocket(1)->GetValue());
        }
    }
    else // Horizontally oriented trim
    {
        // inside left trim?
        if (coord.y <= TrimSize)
        {
            if (NormalizeCoordinates)
                coord.y = NORMALIZE(coord.y, 0.0f, TrimSize);
            ForceExecuteUpstreamOnly(coord);
            GetOutputSocket(0)->StoreValue(GetInputSocket(0)->GetValue());
        }
        else if (coord.y > 1.0f - TrimSize) // inside right trim?
        {
            if (NormalizeCoordinates)
                coord.y = NORMALIZE(coord.y, 0.0f, TrimSize);
            ForceExecuteUpstreamOnly(coord);
            GetOutputSocket(0)->StoreValue(hasRightEdge ? GetInputSocket(2)->GetValue() : GetInputSocket(0)->GetValue());
        }
        else // inside center
        {
            if (NormalizeCoordinates)
                coord.y = NORMALIZE(coord.y, TrimSize, 1.0f - TrimSize);
            ForceExecuteUpstreamOnly(coord);
            GetOutputSocket(0)->StoreValue(GetInputSocket(1)->GetValue());
        }
    }


    return GRAPH_EXECUTE_COMPLETE;
}

void WarpModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "WarpModifier");
    REGISTER_PROPERTY_MEMORY(WarpModifier, float, offsetof(WarpModifier, Intensity), 0.01f, "Intensity", "How large the warp's step size will be", PS_TinyIncrement | PS_Permutable);
}

void WarpModifier::Construct()
{
    AddInput("Warp", TEXGRAPH_CHANNEL);
    AddInput("Perturb X", TEXGRAPH_FLOAT);
    AddInput("Perturb Y", TEXGRAPH_FLOAT);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int WarpModifier::Execute(const Variant& param)
{
    Vec4 vec = param.getVec4Safe();
    float x_coord = vec.x + GetInputSocket(1)->GetValue().getFloatSafe() * Intensity;
    float x_coord_lo = (float)x_coord;
    float x_coord_hi = x_coord_lo + 1 * Intensity;
    float x_frac = x_coord - x_coord_lo;
    float y_coord = vec.y + GetInputSocket(2)->GetValue().getFloatSafe() * Intensity;
    float y_coord_lo = (float)y_coord;
    float y_coord_hi = y_coord_lo + 1 * Intensity;
    float y_frac = y_coord - y_coord_lo;

    ForceExecuteUpstreamOnly(Vec4(x_coord_lo, y_coord_lo, vec.z, vec.w));
    RGBA tl = GetInputSocket(0)->GetValue().getColorSafe(true);

    ForceExecuteUpstreamOnly(Vec4(x_coord_hi, y_coord_lo, vec.z, vec.w));
    RGBA tr = GetInputSocket(0)->GetValue().getColorSafe(true);

    ForceExecuteUpstreamOnly(Vec4(x_coord_lo, y_coord_hi, vec.z, vec.w));
    RGBA bl = GetInputSocket(0)->GetValue().getColorSafe(true);

    ForceExecuteUpstreamOnly(Vec4(x_coord_hi, y_coord_hi, vec.z, vec.w));
    RGBA br = GetInputSocket(0)->GetValue().getColorSafe(true);

    RGBA left = SprueLerp(tl, tr, x_frac);
    RGBA right = SprueLerp(bl, br, x_frac);
    RGBA result = SprueLerp(left, right, y_frac);

    GetOutputSocket(0)->StoreValue(result);

    return GRAPH_EXECUTE_COMPLETE;
}

void PosterizeModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "PosterizeModifier");
    REGISTER_PROPERTY_MEMORY(PosterizeModifier, unsigned, offsetof(PosterizeModifier, Range), 8, "Range", "Sets the amount to divide by to posterize", PS_Default | PS_Permutable);
}

void PosterizeModifier::Construct()
{
    AddInput("In", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int PosterizeModifier::Execute(const Variant& param)
{
    RGBA inColor = GetInputSocket(0)->GetValue().getColorSafe(true);
    inColor.r = Posterize(inColor.r);
    inColor.g = Posterize(inColor.g);
    inColor.b = Posterize(inColor.b);

    GetOutputSocket(0)->StoreValue(inColor);

    return GRAPH_EXECUTE_COMPLETE;
}

float PosterizeModifier::Posterize(float in)
{
    if (Range > 1)
        return ((int)(in * Range) / (float)Range);
    else if (Range == 1)
        return 0.0f;
    else
        return 1.0f;
}

void ErosionModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "ErosionModifier");
    REGISTER_PROPERTY_MEMORY(ErosionModifier, unsigned, offsetof(ErosionModifier, Iterations), 3, "Iterations", "How many passes of talus erosion to perform", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(ErosionModifier, float, offsetof(ErosionModifier, Intensity), 1.0f, "Intensity", "How much material should be removed per pass", PS_VisualConsequence | PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(ErosionModifier, float, offsetof(ErosionModifier, Talus), 0.1f, "Talus", "How much material to remove per pass", PS_VisualConsequence | PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(ErosionModifier, float, offsetof(ErosionModifier, StepSize), 1.0f, "Step Size", "How many pixels to use for the neighborhood check", PS_VisualConsequence | PS_TinyIncrement | PS_Permutable);
}

void ErosionModifier::Construct()
{
    AddInput("Scalar", TEXGRAPH_FLOAT);
    AddInput("Intensity", TEXGRAPH_FLOAT);
    AddOutput("Eroded", TEXGRAPH_FLOAT);
}

int ErosionModifier::Execute(const Variant& param)
{
    Vec4 pos = param.getVec4Safe();

    const float stepping = CalculateStepSize(StepSize, pos);

    if (GetInputSocket(0)->HasConnections() && Iterations > 0)
    {
        float neighbors[3][3] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
        // Above
        ForceExecuteUpstreamOnly(pos + Vec4(0, stepping * -1, 0, 0));
        neighbors[1][0] = GetInputSocket(0)->GetValue().getFloatSafe();
        // left
        ForceExecuteUpstreamOnly(pos + Vec4(stepping * -1, 0, 0, 0));
        neighbors[0][1] = GetInputSocket(0)->GetValue().getFloatSafe();
        // right
        ForceExecuteUpstreamOnly(pos + Vec4(stepping * 1, 0, 0, 0));
        neighbors[2][1] = GetInputSocket(0)->GetValue().getFloatSafe();
        // Below
        ForceExecuteUpstreamOnly(pos + Vec4(0, stepping * 1, 0, 0));
        neighbors[1][2] = GetInputSocket(0)->GetValue().getFloatSafe();
        // Center
        ForceExecuteUpstreamOnly(pos);
        neighbors[1][1] = GetInputSocket(0)->GetValue().getFloatSafe();

        for (int i = 0; i < Iterations; ++i)
        {
            const float h1 = neighbors[1][0];
            const float h2 = neighbors[0][1];
            const float h3 = neighbors[2][1];
            const float h4 = neighbors[1][2];
            
            float d1 = neighbors[1][1] - h1;
            float d2 = neighbors[1][1] - h2;
            float d3 = neighbors[1][1] - h3;
            float d4 = neighbors[1][1] - h4;

            float maxVal = 0.0f;
            IntVec2 offset;
            if (d1 > maxVal)
            {
                maxVal = d1;
                offset.y = 1;
            }
            if (d2 > maxVal)
            {
                maxVal = d2;
                offset.x = -1;
            }
            if (d3 > maxVal)
            {
                maxVal = d3;
                offset.x = 1;
            }
            if (d4 > maxVal)
            {
                maxVal = d4;
                offset.y = -1;
            }
            
            if (maxVal < Talus)
                continue;

            maxVal *= 0.5f;
            neighbors[1 + offset.x][1 + offset.y] = neighbors[1 + offset.x][1 + offset.y] + maxVal;
            neighbors[1][1] = neighbors[1][1] - maxVal;
        }
        GetOutputSocket(0)->StoreValue(neighbors[1][1]);
    }
    return GRAPH_EXECUTE_COMPLETE;
}

void SampleSizeModifier::Register(Context* context)
{
    COPY_PROPERTIES(GraphNode, SampleSizeModifier);
    REGISTER_PROPERTY_MEMORY(SampleSizeModifier, Vec2, offsetof(SampleSizeModifier, newSize), Vec2(128, 128), "New Size", "Size to perform sampling at", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(SampleSizeModifier, bool, offsetof(SampleSizeModifier, Bilinear), true, "Bilinear Filtering", "The output of this node will be filtered bilinearly", PS_VisualConsequence | PS_Permutable);
}

void SampleSizeModifier::Construct()
{
    AddInput("In", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

Variant SampleSizeModifier::FilterParameter(const Variant& param) const
{
    Vec4 p = param.getVec4Safe();
    p.z = newSize.x;
    p.y = newSize.y;
    return p;
}

int SampleSizeModifier::Execute(const Variant& param)
{
    if (!cache)
    {
        cache.reset(new FilterableBlockMap<RGBA>(newSize.x, newSize.y));
        for (unsigned y = 0; y < newSize.y; ++y)
        {
            for (unsigned x = 0; x < newSize.x; ++x)
            {
                ForceExecuteUpstreamOnly(Vec4(((float)x) / newSize.x, ((float)y) / newSize.y, newSize.x, newSize.y));
                cache->set(GetInputSocket(0)->GetValue().getColorSafe(), x, y);
            }
        }
    }

    Vec2 coord = param.getVec2Safe();
    if (Bilinear)
        GetOutputSocket(0)->StoreValue(cache->getBilinear(coord.x, coord.y));
    else
        GetOutputSocket(0)->StoreValue(cache->get(coord.x * cache->getWidth(), coord.y * cache->getHeight()));

    return GRAPH_EXECUTE_COMPLETE;
}

}

