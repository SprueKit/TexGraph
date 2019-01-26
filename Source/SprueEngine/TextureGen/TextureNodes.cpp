#include "TextureNode.h"

#include <SprueEngine/Core/Context.h>
#include "ArtisticNoise.h"
#include "BakerNodes.h"
#include "BlurNodes.h"
#include "ColorNodes.h"
#include "GeneralNodes.h"
#include "NormalMapNodes.h"
#include "PatternGen.h"
#include "PBRNodes.h"
#include "SpecializedGen.h"
#include "TexGenImpl.h"
#include "TexModifierImpl.h"

namespace SprueEngine
{
    Vec4 TextureNode::Make4D(Vec2 coord, Vec2 tiling)
    {
        float x1 = 0, y1 = 0, x2 = tiling.x, y2 = tiling.y;
        float s = coord.x;
        float t = coord.y;
        float dx = x2 - x1;
        float dy = y2 - y1;

        float nx = x1 + cosf(s * 2 * PI)*dx / (2 * PI);
        float ny = y1 + cosf(t * 2 * PI)*dy / (2 * PI);
        float nz = x1 + sinf(s * 2 * PI)*dx / (2 * PI);
        float nw = y1 + sinf(t * 2 * PI)*dy / (2 * PI);

        return Vec4(nw, nx, ny, nz);
    }

    float TextureNode::CalculateStepSize(float stepSize, const Vec4& coordinates)
    {
        return (1.0f / Vec2(coordinates.z, coordinates.w).MaxElement()) * stepSize;
    }

    std::shared_ptr<FilterableBlockMap<RGBA>> PreviewableNode::GetPreview(unsigned width, unsigned height)
    {
        std::shared_ptr<FilterableBlockMap<RGBA>> ret(new FilterableBlockMap<RGBA>(width, height));
        unsigned ctx = 0;
        for (unsigned y = 0; y < height; ++y)
        {
            for (unsigned x = 0; x < width; ++x)
            {
                // OLD BUG: 0 * x == 0, need to add one to make sure graph execution context is good (previously was getting repeating 0s, resulting in executing only once for the first row (socket stored the last success, creating a line)
                ctx = (y+1) * (x+1);
                ExecuteUpstream(ctx, Vec4(x / (float)width, y / (float)height, width, height));
                if (GraphSocket* socket = GetOutputSocket(0))
                {
                    RGBA color = socket->GetValue().getColorSafe(true);
                    color.Clip();
                    ret->set(color, x, y);
                }
            }
        }

        return ret;
    }

    std::shared_ptr<FilterableBlockMap<RGBA>> SelfPreviewableNode::GetPreview(unsigned width, unsigned height)
    {
        std::shared_ptr<FilterableBlockMap<RGBA>> ret(new FilterableBlockMap<RGBA>(width, height));

        for (unsigned y = 0; y < height; ++y)
        {
            for (unsigned x = 0; x < width; ++x)
            {
                Execute(Vec4(x / (float)width, y / (float)height, width, height));
                if (GraphSocket* socket = GetOutputSocket(0))
                {
                    RGBA color = socket->GetValue().getColorSafe(true);
                    color.Clip();
                    ret->set(color, x, y);
                }
            }
        }

        return ret;
    }

    static const char* TEXGEN_OUTPUT_TYPE_NAMES[] = {
        "Albedo",
        "Roughness",
        "Glossiness",
        "Metallic",
        "Normal",
        "Specular",
        "Surface Thickness",
        "Subsurface Color",
        "Height",
        "Custom",
        0x0
    };

    static const char* TEXGEN_OUTPUT_FORMAT_NAMES[] = {
        "RGB",
        "RGBA",
        "Alpha / Grayscale",
        0x0,
    };

    void TextureOutputNode::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "TextureOutputNode");
        REGISTER_ENUM_MEMORY(TextureOutputNode, int, offsetof(TextureOutputNode, OutputType), 0, "Output Type", "Determines the purpose of this output and how it is used for preview", PS_Default, TEXGEN_OUTPUT_TYPE_NAMES);
        REGISTER_ENUM_MEMORY(TextureOutputNode, int, offsetof(TextureOutputNode, Format), 0, "Format", "Channel specification for the output", PS_Default, TEXGEN_OUTPUT_FORMAT_NAMES);
        REGISTER_PROPERTY_MEMORY(TextureOutputNode, RGBA, offsetof(TextureOutputNode, DefaultColor), RGBA(0, 0, 01), "Default Color", "If no connections exist to this output then this color will be used", PS_TinyIncrement);
        REGISTER_PROPERTY_MEMORY(TextureOutputNode, unsigned, offsetof(TextureOutputNode, Width), 128, "Width", "Width in pixels", PS_Default);
        REGISTER_PROPERTY_MEMORY(TextureOutputNode, unsigned, offsetof(TextureOutputNode, Height), 128, "Height", "Height in pixels", PS_Default);
    }

    void TextureOutputNode::Construct()
    {
        AddInput("", TEXGRAPH_CHANNEL);
        AddOutput("", TEXGRAPH_CHANNEL)->secret = true;
    }

    int TextureOutputNode::Execute(const Variant& param)
    {
        if (!GetInputSocket(0)->HasConnections())
            GetOutputSocket(0)->StoreValue(DefaultColor);
        else
            GetOutputSocket(0)->StoreValue(GetInputSocket(0)->GetValue());
        return GRAPH_EXECUTE_COMPLETE;
    }

    std::shared_ptr<FilterableBlockMap<RGBA>> TextureOutputNode::GetPreview(unsigned width, unsigned height)
    {
        std::shared_ptr<FilterableBlockMap<RGBA>> ret(new FilterableBlockMap<RGBA>(width, height));

        unsigned ctx = 0;
        for (unsigned y = 0; y < width; ++y)
        {
            for (unsigned x = 0; x < height; ++x)
            {
                ctx = (y + 1) * (x + 1);
                ExecuteUpstream(ctx, Vec4(x / (float)width, y / (float)height, width, height));
                if (GraphSocket* socket = GetOutputSocket(0))
                {
                    RGBA color = socket->GetValue().getColorSafe(true);
                    color.Clip();

                    if (Format == TGOF_RGB)
                        color.a = 1.0f;
                    if (Format == TGOF_Alpha)
                        ret->set(RGBA(color.r, color.r, color.r), x, y);
                    else
                        ret->set(color, x, y);
                }
            }
        }

        return ret;
    }

#define REG(NAME, TIP) context->RegisterFactory<NAME>( #NAME, #TIP ); NAME::Register(context)

    void RegisterTextureNodes(Context* context)
    {
        REG(TextureOutputNode, "Final rendition into a texture");

        // General Nodes
        REG(AverageNode, "Returns the average value of the input (RGB channels only)");
        REG(BlendNode, "Blends two values together with an optional weight map");
        REG(BrightnessRGBNode, "Returns the brightness of an input value");
        REG(Clamp01Node, "Clamps the input to a 0-1 normal range");
        REG(ColorNode, "Specifies an explicit color value");
        REG(CombineNode, "Combines multiple values into one RGBA value");
        REG(CosNode, "Applies cosine function to all inputs");
        REG(ExpNode, "Applies exp function to all inputs");
        REG(FloatNode, "Specifies an explicit floating point (grayscale) value");
        REG(HSVToRGBNode, "Converts an HSV value into an RGB value");
        REG(MaxNode, "Returns the maximum value of the input (from RGB channels)");
        REG(MinNode, "Returns the minimum value of the input (from RGB channels)");
        REG(PowNode, "Applies the power function to all inputs");
        REG(RGBToHSVNode, "Converts an RGB value to an HSV value");
        REG(SinNode, "Applies the sine function to all inputs");
        REG(SplitNode, "Splits on RGBA value into seperate values for channel extraction");
        REG(SqrtNode, "Applies the square root function to all inputs");
        REG(TanNode, "Applies the tangent function to all inputs");
        REG(BrightnessNode, "Adjusts the brightness of the input");
        REG(ContrastNode, "Adjusts the contrast of the input");
        REG(ToGammaNode, "Converts the input TO gamma space");
        REG(FromGammaNode, "Converts the input FROM gamma space");
        REG(ReplaceColorModifier, "Replaces a color within a specified tolerance threshold");
        REG(ToNormalizedRange, "Remaps values into a normalized range");
        REG(FromNormalizedRange, "Remaps values out of a normalized range");
        REG(PBRAlbedoEnforcerNode, "Enforces the validity of albedo for PBR");

        // Generators
        REG(BitmapGenerator, "Specifies an explicit bitmap to sample");
        REG(SVGGenerator, "Rasterizes an SVG file to a bitmap for sampling");
        REG(BrickGenerator, "Generates a brick pattern");
        REG(CheckerGenerator, "Generates a checkerboard pattern");
        REG(FBMGenerator, "Uses fractal brownian motion on perlin noise to generate a clouds/heightmap image");
        REG(GradientGenerator, "Generates linear, reflected, radial, or angular gradients");
        REG(NoiseGenerator, "Generates random white noise");
        REG(PerlinNoiseGenerator, "Generates pure perlin noise");
        REG(RowsGenerator, "Generates horizontal or vertical rows");
        REG(TextureBombGenerator, "Splats a texture randomly with varying scale or rotation");
        REG(VoronoiGenerator, "Generates cellular textures with different methods of distance calculation");
        REG(WeaveGenerator, "Generates a woven texture");
        REG(ScratchesGenerator, "Produces random lines with optional fade");
        REG(TextureFunction2D, "Uses simple wave functions combined on different axes to generate a texture");
        REG(ArtisticNoise, "Uses many texture with varying parameters to generate easily controllable and tileable noise");
        REG(ScalesGenerator, "Generates a scale-mail like pattern");
        REG(ChainGenerator, "Generates a chain along the Y axis");
        REG(ChainMailGenerator, "Generates an interlocked chainmail image");
        REG(OddBlocksGenerator, "Generates irregularly shaped blocks with a gutter");
        REG(GaborNoiseGenerator, "Generates Gabor Noise");
        REG(IDMapGenerator, "Converts an RGB image into a collection of mask outputs");
        REG(UberNoiseGenerator, "Generates UberNoise");

        // Modifiers and Filters
        REG(ClipTextureModifier, "Clips the input to the specified range");
        REG(CurveTextureModifier, "Applies free-form curves to the input channels");
        REG(EmbossModifier, "Creates a beveling and emboss effect on the input");
        REG(GradientRampTextureModifier, "Remaps the input to a color gradient");
        REG(InvertTextureModifier, "Returns the inverted value of the input");
        REG(NormalMapTextureModifier, "Converts a grayscale image to a normal map based on value slope");
        REG(TileModifier, "Remaps the sample coordinates to tile the input");
        REG(TransformModifier, "Transforms the sample coordinates according to the provided 3x3 matrix");
        REG(WarpModifier, "Perturbs the sample coordinates based on the specified offset inputs and intensity");
        REG(PosterizeModifier, "Divides the input into the specified number of steps");
        REG(BlurModifier, "Blurs the input using a gaussian kernel");
        REG(StreakModifier, "Directionally streaks the inputs");
        REG(SobelTextureModifier, "Returns the perceived edges of the input using Sobel edge filtering");
        REG(ConvolutionFilter, "Applies an arbitrary 3x3 convolution filter to the image");
        REG(SolarizeTextureModifier, "Applies an image solarization effect to the input");
        REG(ErosionModifier, "Hydraulically erodes the input based on the talus on iterations");
        REG(AnisotropicBlur, "Combines edge detection and blur to blur edges less than the interiors");
        REG(NormalMapDeviation, "Returns the deviation (dot-prod) of a normal map from a vector pointing away from the surface");
        REG(NormalMapNormalize, "Normalizes an image assuming that it is intended to be used as a tangent space normal map");
        REG(SharpenFilter, "Applies an image sharpening convolution filter to the inputs");
        REG(SimpleTransformModifier, "Sets up a Matrix transform based on the more user friendly decomposed parameters");
        REG(DivModifier, "Divides the texture space into two separates haves");
        REG(TrimModifier, "Divides the texture into space for the center and a trim on each side (similar to a Div)");
        REG(SelectColorModifier, "Outputs the distance of input colors from a chosen color, useful for generating masks");
        REG(SampleSizeModifier, "Performs up/down sampling by processing the graph at lower or higher resolutions");
        REG(CartesianToPolarModifier, "");
        REG(PolarToCartesianModifier, "");

        // Baker Nodes
        TextureBakerNode::Register(context);
        REG(AmbientOcclusionBakerNode, "Bakes the ambient occlusion of a mesh");
        REG(ObjectSpacePositionBakerNode, "Bakes the object space position (relative to the bounds)");
        REG(ObjectSpaceNormalBakerNode, "Bakes the object space normal map");
        REG(CurvatureBakerNode, "Bakes the per-vertex gaussian curvature of the mesh, with red as concave, and blue as convex");
        REG(VertexColorBakerNode, "Bakes the vertex colors to a texture");
        REG(DominantPlaneBakerNode, "Writes each pixels dominant plane based on the interpolated vertex normals");
        REG(FacetBakerNode, "Draws lines for edges whose angles with neighboring triangles are less than the specified edge threshold");
        REG(TriplanarBakerNode, "Projects a texture using triplanar prjection onto the mesh");
        REG(ObjectSpaceGradientBakerNode, "Renders a gradient ranging from the minimum of the bounds to the maximum of the bounds, similar to object space position");
    }

}