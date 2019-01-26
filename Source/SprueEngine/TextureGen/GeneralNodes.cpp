#include <SprueEngine/TextureGen/GeneralNodes.h>

#include <SprueEngine/Core/Context.h>

namespace SprueEngine
{

// Helper to minimize typo risks
#define GENERIC_REGISTER(NAME) void NAME :: Register(Context* context) { context->CopyBaseProperties("GraphNode", #NAME); }

void ColorNode::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "ColorNode");
    REGISTER_PROPERTY_MEMORY(ColorNode, RGBA, offsetof(ColorNode, Value), RGBA(1,0,0), "Color", "Value to output", PS_Default | PS_Permutable);
}

void ColorNode::Construct()
{
    AddOutput("Value", TEXGRAPH_RGBA);
}

int ColorNode::Execute(const Variant& param)
{
    GetOutputSocket(0)->StoreValue(Value);
    return GRAPH_EXECUTE_COMPLETE;
}

std::shared_ptr<FilterableBlockMap<RGBA> > ColorNode::GetPreview(unsigned, unsigned)
{
    std::shared_ptr<FilterableBlockMap<RGBA> > ret = std::shared_ptr<FilterableBlockMap<RGBA> >(new FilterableBlockMap<RGBA>(64, 32));
    ret->fill(Value);
    return ret;
}

void FloatNode::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "FloatNode");
    REGISTER_PROPERTY_MEMORY(FloatNode, float, offsetof(FloatNode, Value), 0.5f, "Value", "Value to output", PS_TinyIncrement | PS_Permutable);
}

void FloatNode::Construct()
{
    AddOutput("Value", TEXGRAPH_FLOAT);
}

int FloatNode::Execute(const Variant& param)
{
    GetOutputSocket(0)->StoreValue(Value);
    return GRAPH_EXECUTE_COMPLETE;
}

std::shared_ptr<FilterableBlockMap<RGBA> > FloatNode::GetPreview(unsigned, unsigned)
{
    std::shared_ptr<FilterableBlockMap<RGBA> > ret = std::shared_ptr<FilterableBlockMap<RGBA> >(new FilterableBlockMap<RGBA>(64, 32));
    RGBA value(Value, Value, Value);
    ret->fill(value);
    return ret;
}

#define MATH_FUNC_NODE(TYPENAME, FUNCNAME) void TYPENAME::Register(Context* context) { \
   context->CopyBaseProperties("GraphNode", #TYPENAME); \
} \
void TYPENAME::Construct() \
{ \
    AddInput("In", TEXGRAPH_CHANNEL); \
    AddOutput("Out", TEXGRAPH_CHANNEL); \
} \
int TYPENAME::Execute(const Variant& param) { \
    RGBA col = GetInputSocket(0)->GetValue().getColorSafe(true); \
    col.r = FUNCNAME(col.r);\
    col.g = FUNCNAME(col.g);\
    col.b = FUNCNAME(col.b);\
    col.a = FUNCNAME(col.a);\
    GetOutputSocket(0)->StoreValue(col); \
    return GRAPH_EXECUTE_COMPLETE; }

MATH_FUNC_NODE(CosNode, cosf);
MATH_FUNC_NODE(SinNode, sinf);
MATH_FUNC_NODE(SqrtNode, sqrtf);
MATH_FUNC_NODE(ExpNode, expf);
MATH_FUNC_NODE(TanNode, tanf);

GENERIC_REGISTER(PowNode);

void PowNode::Construct()
{
    AddInput("In", TEXGRAPH_CHANNEL);
    AddInput("Exponent", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int PowNode::Execute(const Variant& param)
{    
    RGBA leftColor = GetInputSocket(0)->GetValue().getColorSafe(true);
    RGBA rightColor = GetInputSocket(1)->GetValue().getColorSafe(true);
    
    leftColor.r = powf(leftColor.r, rightColor.r);
    leftColor.g = powf(leftColor.g, rightColor.g);
    leftColor.b = powf(leftColor.b, rightColor.b);
    leftColor.a = powf(leftColor.a, rightColor.a);

    GetOutputSocket(0)->StoreValue(leftColor);

    return GRAPH_EXECUTE_COMPLETE;
}

GENERIC_REGISTER(AverageNode);

void AverageNode::Construct()
{
    AddInput("In", TEXGRAPH_RGBA);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int AverageNode::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    GetOutputSocket(0)->StoreValue(color.AverageRGB());
    return GRAPH_EXECUTE_COMPLETE;
}

GENERIC_REGISTER(MinNode);

void MinNode::Construct()
{
    AddInput("In", TEXGRAPH_RGBA);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int MinNode::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    GetOutputSocket(0)->StoreValue(SprueMin(color.r, SprueMin(color.g, color.b)));
    return GRAPH_EXECUTE_COMPLETE;
}

GENERIC_REGISTER(MaxNode);

void MaxNode::Construct()
{
    AddInput("In", TEXGRAPH_RGBA);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int MaxNode::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    GetOutputSocket(0)->StoreValue(SprueMax(color.r, SprueMax(color.g, color.b)));
    return GRAPH_EXECUTE_COMPLETE;
}

GENERIC_REGISTER(Clamp01Node);

void Clamp01Node::Construct()
{
    AddInput("In", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int Clamp01Node::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    color.Clip();
    GetOutputSocket(0)->StoreValue(color);

    return GRAPH_EXECUTE_COMPLETE;
}

GENERIC_REGISTER(SplitNode);

void SplitNode::Construct()
{
    AddInput("In", TEXGRAPH_CHANNEL);
    AddOutput("RGB", TEXGRAPH_RGBA);
    AddOutput("R", TEXGRAPH_FLOAT);
    AddOutput("G", TEXGRAPH_FLOAT);
    AddOutput("B", TEXGRAPH_FLOAT);
    AddOutput("A", TEXGRAPH_FLOAT);
}

int SplitNode::Execute(const Variant& param)
{
    RGBA value = GetInputSocket(0)->GetValue().getColorSafe(true);

    GetOutputSocket(0)->StoreValue(RGBA(value.r, value.g, value.b, 1.0f));
    GetOutputSocket(1)->StoreValue(value.r);
    GetOutputSocket(2)->StoreValue(value.g);
    GetOutputSocket(3)->StoreValue(value.b);
    GetOutputSocket(4)->StoreValue(value.a);

    return GRAPH_EXECUTE_COMPLETE;
}

void SplitNode::VersionUpdate(unsigned fromVersion)
{
    if (fromVersion < 2)
        InsertOutput(0, "RGB", TEXGRAPH_CHANNEL);
}

GENERIC_REGISTER(CombineNode);

void CombineNode::Construct()
{
    AddInput("RGB", TEXGRAPH_CHANNEL);
    AddInput("R", TEXGRAPH_FLOAT);
    AddInput("G", TEXGRAPH_FLOAT);
    AddInput("B", TEXGRAPH_FLOAT);
    AddInput("A", TEXGRAPH_FLOAT);
    AddOutput("Color", TEXGRAPH_RGBA);
}

void CombineNode::VersionUpdate(unsigned fromVersion)
{
    if (fromVersion < 2)
        InsertInput(0, "RGB", TEXGRAPH_CHANNEL);
}

int CombineNode::Execute(const Variant& param)
{
    RGBA col = GetInputSocket(0)->GetValue().getColorSafe();
    float r = GetInputSocket(1)->GetValue().getFloatSafe();
    float g = GetInputSocket(2)->GetValue().getFloatSafe();
    float b = GetInputSocket(3)->GetValue().getFloatSafe();
    float a = GetInputSocket(4)->GetValue().getFloatSafe();

    if (GetInputSocket(0)->HasConnections())
        GetOutputSocket(0)->StoreValue(RGBA(col.r, col.b, col.g, GetInputSocket(4)->HasConnections() ? a : 1.0f));
    else
        GetOutputSocket(0)->StoreValue(RGBA(r, g, b, GetInputSocket(4)->HasConnections() ? a : 1.0f));
    return GRAPH_EXECUTE_COMPLETE;
}

GENERIC_REGISTER(RGBToHSVNode);

void RGBToHSVNode::Construct()
{
    AddInput("In", TEXGRAPH_RGBA);
    AddOutput("HSV", TEXGRAPH_RGBA);
}

int RGBToHSVNode::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    
    RGBA hsv;
    float min, max, delta;
    min = SprueMin(color.r, SprueMin(color.g, color.b));
    max = SprueMax(color.r, SprueMax(color.g, color.b));
    hsv.b = max;
    delta = max - min;
    
    if (max == 0)
    {
        hsv.g = 0;
        hsv.b = -1;
    }
    else
    {
        hsv.g = delta / max;
        if (color.r == max)
            hsv.r = (color.g - color.b) / delta;
        else if (color.g == max)
            hsv.r = 2 + (color.b - color.r) / delta;
        else
            hsv.r = 4 + (color.r - color.g) / delta;
        hsv.r *= 60;
        if (hsv.r < 0)
            hsv.r += 360;
    }
    GetOutputSocket(0)->StoreValue(hsv);
    return GRAPH_EXECUTE_COMPLETE;
}

GENERIC_REGISTER(HSVToRGBNode);

void HSVToRGBNode::Construct()
{
    AddInput("HSV", TEXGRAPH_RGBA);
    AddOutput("RGB", TEXGRAPH_RGBA);
}

int HSVToRGBNode::Execute(const Variant& param)
{
    RGBA hsv = GetInputSocket(0)->GetValue().getColorSafe(true);
    RGBA color;

    int i;
    float f, p, q, t;
    if (hsv.g == 0) 
    {
        color.r = color.g = color.b = hsv.b;
    }
    else
    {
        hsv.r /= 60;
        i = floor(hsv.r);
        f = hsv.r - i;
        p = hsv.b * (1 - hsv.g);
        q = hsv.b * (1 - hsv.g * f);
        t = hsv.b * (1 - hsv.g * (1 - f));
        switch (i) {
        case 0:
            color.r = hsv.b;
            color.g = t;
            color.b = p;
            break;
        case 1:
            color.r = q;
            color.g = hsv.b;
            color.b = p;
            break;
        case 2:
            color.r = p;
            color.g = hsv.b;
            color.b = t;
            break;
        case 3:
            color.r = p;
            color.g = q;
            color.b = hsv.b;
            break;
        case 4:
            color.r = t;
            color.g = p;
            color.b = hsv.b;
            break;
        default:
            color.r = hsv.b;
            color.g = p;
            color.b = q;
            break;
        }
    }

    GetOutputSocket(0)->StoreValue(hsv);
    return GRAPH_EXECUTE_COMPLETE;
}

GENERIC_REGISTER(BrightnessRGBNode);

void BrightnessRGBNode::Construct()
{
    AddInput("In", TEXGRAPH_RGBA);
    AddOutput("Out", TEXGRAPH_FLOAT);
}

int BrightnessRGBNode::Execute(const Variant& param)
{
    RGBA in = GetInputSocket(0)->GetValue().getColorSafe(true);
    GetOutputSocket(0)->StoreValue(in.Brightness());

    return GRAPH_EXECUTE_COMPLETE;
}

static const char* BlendModeNames[] = {
    "Normal",
    "Additive",
    "Subtract",
    "Multiply",
    "Divide",
    "Color Burn",
    "Linear Burn",
    "Screen",
    "Color Dodge",
    "Linear Dodge",
    "Dissolve",
    "Normal Map",
    0x0
};

static const char* TexGenBlendSourceNames[] = {
    "Use Weight",
    "Use Destination Alpha",
    "Use Source Alpha",
    0x0
};

void BlendNode::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "BlendNode");
    REGISTER_ENUM(BlendNode, TexGenBlendMode, GetBlendMode, SetBlendMode, TGB_Normal, "Blend Mode", "Determines how blending will be performed", PS_Default, BlendModeNames);
    REGISTER_ENUM_MEMORY(BlendNode, int, offsetof(BlendNode, alphaMode_), 0, "Blend Source", "Determines what value to use for controlling the blend, if source or destination alpha are used then weight is ignored and their alpha channels will be set to 1.0", PS_Default, TexGenBlendSourceNames);
}

void BlendNode::Construct()
{
    mode_ = TGB_Normal;
    AddInput("Dest", TEXGRAPH_CHANNEL);
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddInput("Weight", TEXGRAPH_FLOAT);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int BlendNode::Execute(const Variant& param)
{
    RGBA src = GetInputSocket(1)->GetValue().getColorSafe(true);
    RGBA dest = GetInputSocket(0)->GetValue().getColorSafe(true);
    Variant weightVal = GetInputSocket(2)->GetValue();
    float blendWeight = 0.0f;
    
    if (alphaMode_ == TGBA_UseWeight)
        blendWeight = weightVal.getType() != VT_None ? weightVal.getFloatSafe() : 0.5f;
    else if (alphaMode_ == TGBA_UseDestAlpha)
    {
        blendWeight = dest.a;
        dest.a = 1.0f;
        src.a = 1.0f;
    }
    else if (alphaMode_ == TGBA_UseSourceAlpha)
    {
        blendWeight = src.a;
        src.a = 1.0f;
        dest.a = 1.0f;
    }

    RGBA resultColor;
    switch (GetBlendMode())
    {
    // These two do the same things
    case TGB_Normal:
        //alpha * new + (1 - alpha) * old
        resultColor = (src * blendWeight) + (dest * (1 - blendWeight));
        break;
    case TGB_Additive:
    case TGB_LinearDodge:
        resultColor = dest + src;
        break;
    case TGB_Subtract:
        resultColor = dest - src;
        break;
    case TGB_Multiply:
        resultColor = dest * src;
        break;
    case TGB_Divide:
        resultColor = dest / src;
        break;
    case TGB_ColorBurn:
        resultColor = RGBA::White - ((RGBA::White - src) / dest);
        break;
    case TGB_LinearBurn:
        resultColor = dest + src - RGBA::White;
        break;
    case TGB_Screen:
        resultColor = RGBA::White - (RGBA::White - dest) * (RGBA::White - src);
        break;
    case TGB_ColorDodge:
        resultColor = src / (RGBA::White - dest);
        break;
    case TGB_Dissolve: {
        /// Get a random float for our position
        Vec2 uv = param.getVec2Safe();
        unsigned seed = StringHash::fnv1a(&uv, sizeof(Vec2));
        float dissolveWeight = RandomFloat(seed);
        resultColor = dissolveWeight < blendWeight ? dest : src;
        } break;
    case TGB_NormalMap: {
        // Weighted variation of Ruby 'WhiteOut' style normal map blending
        float destWeight = 1.0f - blendWeight;
        Vec3 n1 = dest.ToNormal().Normalized() * destWeight;
        Vec3 n2 = src.ToNormal().Normalized() * blendWeight;
        Vec3 newNormal(n1.x + n2.x, n1.y + n2.y, n1.z * n2.z);
        resultColor.FromNormal(newNormal.Normalized());
        } break;
    }

    // Blend time
    resultColor = SprueLerp(dest, resultColor, blendWeight);

    GetOutputSocket(0)->StoreValue(resultColor);

    return GRAPH_EXECUTE_COMPLETE;
}

void BrightnessNode::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "BrightnessNode");
    REGISTER_PROPERTY_MEMORY(BrightnessNode, float, offsetof(BrightnessNode, Power), 1.2f, "Power", "Factor to multiply the input by", PS_SmallIncrement | PS_Permutable);
}

void BrightnessNode::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int BrightnessNode::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    color *= Power;
    color.Clip();
    GetOutputSocket(0)->StoreValue(color);

    return GRAPH_EXECUTE_COMPLETE;
}

void ContrastNode::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "ContrastNode");
    REGISTER_PROPERTY_MEMORY(ContrastNode, float, offsetof(ContrastNode, Power), 1.2f, "Power", "", PS_SmallIncrement | PS_Permutable);
}

void ContrastNode::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int ContrastNode::Execute(const Variant& param)
{
    RGBA color = GetInputSocket(0)->GetValue().getColorSafe(true);
    color.r -= 0.5f;
    color.g -= 0.5f;
    color.b -= 0.5f;
    color *= Power;
    color.r += 0.5f;
    color.g += 0.5f;
    color.b += 0.5f;
    GetOutputSocket(0)->StoreValue(color);

    return GRAPH_EXECUTE_COMPLETE;
}

void ToGammaNode::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "ToGammaNode");
    REGISTER_PROPERTY_MEMORY(ToGammaNode, float, offsetof(ToGammaNode, Gamma), 2.2f, "Gamma", "Target gamma for correction", PS_TinyIncrement | PS_Permutable);
}

void ToGammaNode::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int ToGammaNode::Execute(const Variant& param)
{
    RGBA value = GetInputSocket(0)->GetValue().getColorSafe(true);
    value.r = powf(value.r, 1.0f / 2.2f);
    value.g = powf(value.g, 1.0f / 2.2f);
    value.b = powf(value.b, 1.0f / 2.2f);
    GetOutputSocket(0)->StoreValue(value);
    return GRAPH_EXECUTE_COMPLETE;
}

void FromGammaNode::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "FromGammaNode");
    REGISTER_PROPERTY_MEMORY(ToGammaNode, float, offsetof(ToGammaNode, Gamma), 2.2f, "Gamma", "Target gamma for correction", PS_TinyIncrement | PS_Permutable);
}

void FromGammaNode::Construct()
{
    AddInput("Src", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int FromGammaNode::Execute(const Variant& param)
{
    RGBA value = GetInputSocket(0)->GetValue().getColorSafe(true);
    value.r = powf(value.r, 2.2f);
    value.g = powf(value.g, 2.2f);
    value.b = powf(value.b, 2.2f);
    GetOutputSocket(0)->StoreValue(value);
    return GRAPH_EXECUTE_COMPLETE;
}

void ToNormalizedRange::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "ToNormalizedRange");
    REGISTER_PROPERTY_MEMORY(ToNormalizedRange, RangedFloat, offsetof(ToNormalizedRange, range_), RangedFloat(0.0f, 1.0f), "Range", "Remaps values INTO a normalized range, ie. 0.5-1.0 into 0-1", PS_Default | PS_NormalRange | PS_Permutable);
}

void ToNormalizedRange::Construct()
{
    AddInput("", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int ToNormalizedRange::Execute(const Variant& param)
{
    RGBA value = GetInputSocket(0)->GetValue().getColorSafe(true);
    
    value.r = NORMALIZE(value.r, range_.getLowerBound(), range_.getUpperBound());
    value.g = NORMALIZE(value.g, range_.getLowerBound(), range_.getUpperBound());
    value.b = NORMALIZE(value.b, range_.getLowerBound(), range_.getUpperBound());
    value.Clip();

    GetOutputSocket(0)->StoreValue(value);
    return GRAPH_EXECUTE_COMPLETE;
}

void FromNormalizedRange::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "FromNormalizedRange");
    REGISTER_PROPERTY_MEMORY(FromNormalizedRange, RangedFloat, offsetof(ToNormalizedRange, range_), RangedFloat(0.0f, 1.0f), "Range", "Remaps values OUT of a normalized range, ie. 0.0-1.0 into 0.5-1.0", PS_Default | PS_NormalRange | PS_Permutable);
}

void FromNormalizedRange::Construct()
{
    AddInput("", TEXGRAPH_CHANNEL);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int FromNormalizedRange::Execute(const Variant& param)
{
    RGBA value = GetInputSocket(0)->GetValue().getColorSafe(true);

    value.r = DENORMALIZE(value.r, range_.getLowerBound(), range_.getUpperBound());
    value.g = DENORMALIZE(value.g, range_.getLowerBound(), range_.getUpperBound());
    value.b = DENORMALIZE(value.b, range_.getLowerBound(), range_.getUpperBound());
    value.Clip();

    GetOutputSocket(0)->StoreValue(value);
    return GRAPH_EXECUTE_COMPLETE;
}

}