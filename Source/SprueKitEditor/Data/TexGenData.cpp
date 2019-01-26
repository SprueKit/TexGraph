#include "TexGenData.h"

#include <SprueEngine/Core/Context.h>

#include <algorithm>

#include <qfile.h>
#include <qxmlstream.h>

namespace SprueEditor
{

#define DECL_STRING(HASH, NAME, CLAZZ) std::make_pair(#HASH, NodeNameData { #NAME, #CLAZZ, #HASH })

    const std::map<SprueEngine::StringHash, QColor> TexGenData::TitleColors = {
        { "SPECIAL",  QColor(40, 40, 40) },
        { "Math", QColor(128, 0, 0) },
        { "Color", QColor(0.0, 128, 0) },
        { "Generator", QColor(128, 128, 0) },
        { "Value", QColor(0.0, 0.0, 128) },
        { "Filter", QColor(0, 128, 128) },
        { "Normal Maps", QColor(128, 128, 255) },
        { "Bakers", QColor(255, 128, 128) }
    };

    const std::map<SprueEngine::StringHash, TexGenData::NodeNameData > TexGenData::NodeNames = {
        DECL_STRING(TextureOutputNode, OUTPUT, SPECIAL),

        DECL_STRING(BlendNode, Blend, SPECIAL),

        // Math nodes
        DECL_STRING(AverageNode, Average, Math),
        DECL_STRING(Clamp01Node, Clamp 0 - 1, Math),
        DECL_STRING(CosNode, Cos, Math),
        DECL_STRING(ExpNode, Exp, Math),
        DECL_STRING(MaxNode, Max, Math),
        DECL_STRING(MinNode, Min, Math),
        DECL_STRING(PowNode, Pow, Math),
        DECL_STRING(SinNode, Sin, Math),
        DECL_STRING(SqrtNode, Sqrt, Math),
        DECL_STRING(TanNode, Tan, Math),
        DECL_STRING(SampleSizeModifier, Sample Size, Math),

        // Color Nodes
        DECL_STRING(BrightnessRGBNode, Extract Brightness, Color),
        DECL_STRING(CombineNode, Combine, Color),
        DECL_STRING(HSVToRGBNode, HSV To RGB, Color),
        DECL_STRING(RGBToHSVNode, RGB To HSV, Color),
        DECL_STRING(SplitNode, Split, Color),
        DECL_STRING(ToGammaNode, To Gamma, Color),
        DECL_STRING(FromGammaNode, From Gamma, Color),
        DECL_STRING(BrightnessNode, Brightness, Color),
        DECL_STRING(ContrastNode, Contrast, Color),
        DECL_STRING(ReplaceColorModifier, Replace Color, Color),
        DECL_STRING(PBRAlbedoEnforcerNode, PBR - Enforce Albedo, Color),

        // Value nodes
        DECL_STRING(ColorNode, Color, Value),        
        DECL_STRING(FloatNode, Float, Value),
        DECL_STRING(BitmapGenerator, Bitmap, Value),
        DECL_STRING(SVGGenerator, SVG, Value),

        // Generators
        DECL_STRING(BrickGenerator, Bricks, Generator),
        DECL_STRING(CheckerGenerator, Checkers, Generator),
        DECL_STRING(FBMGenerator, FBM, Generator),
        DECL_STRING(GradientGenerator, Gradient, Generator),
        DECL_STRING(NoiseGenerator, Noise, Generator),
        DECL_STRING(PerlinNoiseGenerator, Perlin Noise, Generator),
        DECL_STRING(RowsGenerator, Rows, Generator),
        DECL_STRING(TextureBombGenerator, Tex Bomb, Generator),
        DECL_STRING(VoronoiGenerator, Voronoi, Generator),
        DECL_STRING(WeaveGenerator, Weave, Generator),
        DECL_STRING(ScratchesGenerator, Scratches, Generator),
        DECL_STRING(TextureFunction2D, Function 2D, Generator),
        DECL_STRING(ArtisticNoise, Art Noise, Generator),
        DECL_STRING(ScalesGenerator, Scales, Generator),
        DECL_STRING(ChainGenerator, Chain, Generator),
        DECL_STRING(ChainMailGenerator, Chainmail, Generator),
        DECL_STRING(OddBlocksGenerator, Irregular Blocks, Generator),
        DECL_STRING(GaborNoiseGenerator, Gabor Noise, Generator),
        DECL_STRING(IDMapGenerator, ID Map, Generator),
        DECL_STRING(UberNoiseGenerator, Uber Noise, Generator),

        // Modifiers and Filters
        DECL_STRING(ClipTextureModifier, Clip, Filter),
        DECL_STRING(CurveTextureModifier, Curve, Filter),
        DECL_STRING(EmbossModifier, Emboss, Filter),
        DECL_STRING(GradientRampTextureModifier, Gradient Ramp, Filter),
        DECL_STRING(InvertTextureModifier, Invert, Filter),
        DECL_STRING(SolarizeTextureModifier, Solarize, Filter),
        DECL_STRING(TileModifier, Tile, Filter),
        DECL_STRING(TransformModifier, Transform (Matrix), Filter),
        DECL_STRING(WarpModifier, Warp, Filter),
        DECL_STRING(PosterizeModifier, Posterize, Filter),
        DECL_STRING(BlurModifier, Blur, Filter),
        DECL_STRING(StreakModifier, Streak, Filter),
        DECL_STRING(SobelTextureModifier, Sobel Edge, Filter),
        DECL_STRING(ConvolutionFilter, Convolution Filter, Filter),
        DECL_STRING(AnisotropicBlur, Anisotropic Blur, Filter),
        DECL_STRING(ErosionModifier, Erosion, Filter),
        DECL_STRING(SharpenFilter, Sharpen, Filter),
        DECL_STRING(SimpleTransformModifier, Transform (Simple), Filter),
        DECL_STRING(DivModifier, Divide Space, Filter),
        DECL_STRING(TrimModifier, Border Trim, Filter),
        DECL_STRING(ToNormalizedRange, To Normalized Range, Filter),
        DECL_STRING(FromNormalizedRange, From Normalized Range, Filter),
        DECL_STRING(SelectColorModifier, Select Color, Filter),
        DECL_STRING(CartesianToPolarModifier, Cart. To Polar, Filter),
        DECL_STRING(PolarToCartesianModifier, Polar To Cart., Filter),

        // Normal Map filters
        DECL_STRING(NormalMapTextureModifier, To Normal Map, Normal Maps),
        DECL_STRING(NormalMapNormalize, Normal Map Normalize, Normal Maps),
        DECL_STRING(NormalMapDeviation, Normal Map Deviation, Normal Maps),

        // Baker nodes
        DECL_STRING(CurvatureBakerNode, Curvature, Bakers),
        DECL_STRING(ObjectSpacePositionBakerNode, Object Space Position, Bakers),
        DECL_STRING(ObjectSpaceNormalBakerNode, Object Space Normal, Bakers),
        DECL_STRING(VertexColorBakerNode, Vertex Color, Bakers),
        DECL_STRING(DominantPlaneBakerNode, Dominant Plane, Bakers),
        DECL_STRING(AmbientOcclusionBakerNode, Ambient Occlusion, Bakers),
        DECL_STRING(FacetBakerNode, Facets, Bakers),
        DECL_STRING(TriplanarBakerNode, Triplanar Texture, Bakers),
        DECL_STRING(ObjectSpaceGradientBakerNode, Object Space Gradient, Bakers),
    };

    std::vector<TexGenData::NodeNameRecord> TexGenData::GetSortedNodeRecords(const char* groupName)
    {
        std::vector<NodeNameRecord> found;
        for (auto nodename : NodeNames)
        {
            if (strcmp(nodename.second.second, groupName) == 0)
                found.push_back(nodename);
        }

        std::sort(found.begin(), found.end(), [=](NodeNameRecord lhs, NodeNameRecord rhs) {
            return strcmp(lhs.second.first, rhs.second.first) == -1;
        });

        return found;
    }

    void TexGenData::DumpToXML(const char* fileName)
    {
        QXmlStreamWriter writer;
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly))
            return;

        writer.setDevice(&file);
        writer.writeStartDocument();
        writer.writeStartElement("typedata");
        writer.writeStartElement("language");
        writer.writeAttribute("name", "English");
        for (auto record : NodeNames)
        {
            writer.writeStartElement("type");
            std::string hashName = SprueEngine::Context::GetInstance()->GetHashName(record.first);
            writer.writeAttribute("hash", hashName.c_str());
            
            writer.writeTextElement("name", record.second.first);

            std::string desc = SprueEngine::Context::GetInstance()->GetTypeDescription(record.first);
            writer.writeTextElement("desc", desc.c_str());

            writer.writeStartElement("group");
            writer.writeTextElement("group", record.second.second);

            writer.writeEndElement();
        }
        writer.writeEndElement();
        writer.writeEndDocument();
    }

    QIcon TexGenData::GetIconFor(const char* typeName)
    {
        QString path = QString(":/Images/TextureNodes/%1.png").arg(typeName);
        QIcon icon(path);
        if (!QFile::exists(path))
        {
            if (QString(typeName).contains("baker", Qt::CaseInsensitive))
                icon = QIcon(QString(":/Images/TextureNodes/generic_baker.png"));
            else
                icon = QIcon(QString(":/Images/TextureNodes/Generic.png"));
        }
        return icon;
    }

    QColor TexGenData::GetColorOf(const char* typeName)
    {
        auto found = NodeNames.find(typeName);
        if (found != NodeNames.end())
        {
            auto foundColor = TitleColors.find(found->second.second);
            if (foundColor != TitleColors.end())
                return foundColor->second;
        }
        return Qt::white;
    }
}