#include "ColorNodes.h"

#include "../Core/Context.h"
#include <SprueEngine/StringConversion.h>

namespace SprueEngine
{

void ReplaceColorModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "ReplaceColorModifier");
    REGISTER_PROPERTY_MEMORY(ReplaceColorModifier, RGBA, offsetof(ReplaceColorModifier, Replace), RGBA(0, 0, 0), "Replace", "Color to seek out for replacement", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(ReplaceColorModifier, RGBA, offsetof(ReplaceColorModifier, With), RGBA(0, 0, 0), "With", "Color that will be substituted in place", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(ReplaceColorModifier, float, offsetof(ReplaceColorModifier, Tolerance), 0.1f, "Tolerance", "Maximum deviation for to replace", PS_VisualConsequence | PS_TinyIncrement | PS_Permutable);
}

void ReplaceColorModifier::Construct()
{
    AddInput("In", TEXGRAPH_RGBA);
    AddOutput("Out", TEXGRAPH_RGBA);
}

int ReplaceColorModifier::Execute(const Variant& param)
{
    RGBA inColor = GetInputSocket(0)->GetValue().getColorSafe(true);
    RGBA modified = Replace - inColor;
    float length = sqrtf(modified.r * modified.r + modified.g * modified.g + modified.b * modified.b);
    float multiplier = inColor.Brightness(); // NORMALIZE(inColor.Brightness(), 0.0f, With.Brightness());
    if (length < Tolerance)
        GetOutputSocket(0)->StoreValue(RGBA(With.r * multiplier, With.g * multiplier, With.b * multiplier, With.a));
    else
        GetOutputSocket(0)->StoreValue(inColor);
    return GRAPH_EXECUTE_COMPLETE;
}

void SelectColorModifier::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "SelectColorModifier");
    REGISTER_PROPERTY_MEMORY(SelectColorModifier, RGBA, offsetof(SelectColorModifier, Select), RGBA(0, 0, 0), "Color", "Color to seek out", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(SelectColorModifier, float, offsetof(SelectColorModifier, Tolerance), 0.1f, "Tolerance", "Maximum deviation for color selection", PS_VisualConsequence | PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(SelectColorModifier, bool , offsetof(SelectColorModifier, Boolean), false, "Boolean", "Value will be clamped to 0 or 1", PS_VisualConsequence | PS_Permutable);
}

void SelectColorModifier::Construct()
{
    AddInput("In", TEXGRAPH_RGBA);
    AddOutput("Out", TEXGRAPH_FLOAT);
}

int SelectColorModifier::Execute(const Variant& param)
{
    RGBA inColor = GetInputSocket(0)->GetValue().getColorSafe(true);
    RGBA modified = Select - inColor;
    float length = fabsf(sqrtf(modified.r * modified.r + modified.g * modified.g + modified.b * modified.b));
    float multiplier = inColor.Brightness(); // NORMALIZE(inColor.Brightness(), 0.0f, With.Brightness());
    if (length < Tolerance)
        GetOutputSocket(0)->StoreValue(Boolean ? 1.0f : 1.0f - length);
    else
        GetOutputSocket(0)->StoreValue(0.0f);
    return GRAPH_EXECUTE_COMPLETE;
}

void IDMapGenerator::Register(Context* context)
{
    //context->RegisterFactory<IDMapGenerator>("IDMapGenerator", "Processes an RGB map to generate unique ID masks for each unique RGB value");
    context->CopyBaseProperties("GraphNode", "IDMapGenerator");
    REGISTER_RESOURCE(IDMapGenerator, BitmapResource, GetBitmapResourceHandle, SetBitmapResourceHandle, GetImageData, SetImageData, ResourceHandle("Image"), "Image", "Unique colors in the image will be isolated into unique masks", PS_Default);
}

void IDMapGenerator::Construct()
{
}

void IDMapGenerator::SetImageData(const std::shared_ptr<BitmapResource>& img) { 
    ImageData = img; 
    if (ImageData)
    {
        std::vector<RGBA> colors;
        for (unsigned y = 0; y < ImageData->GetImage()->getHeight(); ++y)
        {
            for (unsigned x = 0; x < ImageData->GetImage()->getWidth(); ++x)
            {
                RGBA col = ImageData->GetImage()->get(x, y);
                if (std::find(colors.begin(), colors.end(), col) == colors.end())
                    colors.push_back(col);
            }
        }
        this->colors = colors;

        if (!inSerialization)
        {
            if (colors.size() != outputSockets.size())
            {
                int difference = ((int)colors.size()) - ((int)outputSockets.size());
                if (difference > 0)
                {
                    // new sockets added
                    while (difference)
                    {
                        AddOutput(ToHexString(colors[colors.size() - difference]), TEXGRAPH_FLOAT);
                        --difference;
                    }
                    NotifySocketsChange();
                }
                else
                {
                    // sockets removed
                    outputSockets.erase(outputSockets.begin() + (outputSockets.size() - difference), outputSockets.end());
                    NotifySocketsChange();
                }
            }
        }
    }
}

int IDMapGenerator::Execute(const Variant& param)
{
    auto vec = param.getVec4Safe();
    if (ImageData && ImageData->GetImage())
    {
        const int ix = vec.x * ImageData->GetImage()->getWidth();
        const int iy = vec.y * ImageData->GetImage()->getHeight();

        for (unsigned i = 0; i < outputSockets.size() && i < colors.size(); ++i)
            outputSockets[i]->StoreValue(colors[i] == ImageData->GetImage()->get(ix, iy) ? RGBA::White : RGBA::Black);
    }

    return GRAPH_EXECUTE_COMPLETE;
}

std::shared_ptr<FilterableBlockMap<RGBA>> IDMapGenerator::GetPreview(unsigned width, unsigned height)
{
    std::shared_ptr<FilterableBlockMap<RGBA>> ret(new FilterableBlockMap<RGBA>(width, height));
    ret->fill(RGBA::Black);

    unsigned ctx = 0;
    if (ImageData)
    {
        for (unsigned y = 0; y < height; ++y)
        {
            float fy = ((float)y) / height;
            for (unsigned x = 0; x < width; ++x)
            {
                float fx = ((float)x) / width;
                ret->set(ImageData->GetImage()->getBilinear(fx, fy), x, y);
            }
        }
    }
    return ret;
}

bool IDMapGenerator::Deserialize(Deserializer* src, const SerializationContext& context)
{
    inSerialization = true;
    const bool ret = GraphNode::Deserialize(src, context);
    inSerialization = false;
    return ret;
}

}