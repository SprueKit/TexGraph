#include <SprueEngine/TextureGen/TexGenImpl.h>

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/Core/Context.h>
#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/Libs/ANL_NoiseGen.h>

#define FAST_NOISE_ADDR(TYPE, PARAM) (offsetof(TYPE, noise_) + offsetof(FastNoise, PARAM))

#define PREVIEW_SIZE 128

namespace SprueEngine
{

    static Vec4 Make4D(Vec2 coord, Vec2 tiling)
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

static const char* INTERP_NAMES[] = {
    "Linear",
    "Hermite",
    0x0
};

static const char* FRACTAL_NAMES[] = {
    "FBM",
    "Billow",
    "Rigid Multi",
    0x0
};

static const char* VALUE_NAMES[] = {
    "Value",
    "Value FBM",
    "Gradient",
    "Gradient FBM",
    "Simplex",
    "Simplex FBM",
    "Cellular",
    "Cellular HQ",
    "White Noise",
    0x0
};

static const char* CELL_DISTANCE_NAMES[] = {
    "Euclidean", 
    "Manhattan", 
    "Natural",
    "Rect",
    0x0
};

static const char* CELL_RETURN_NAMES[] = {
    "Cell Value",
    "Noise Lookup",
    "Distance To Center",
    "Distance To Center XValue",
    "Distance To Center Sq",
    "Distance To Center Sq XValue",
    "Distance To Edge",
    "Distance To Edge XValue",
    "Distance To Edge Sq",
    "Distance To Edge Sq XValue",
    0x0
};

static FastNoise::Interp INTERP_TYPES[] = {
    FastNoise::Interp::InterpLinear,
    FastNoise::Interp::InterpHermite,
};

void RowsGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "RowsGenerator");
    REGISTER_PROPERTY_MEMORY(RowsGenerator, unsigned, offsetof(RowsGenerator, RowCount), 6, "Rows", "How many bands will be generated", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(RowsGenerator, bool, offsetof(RowsGenerator, Vertical), false, "Vertical", "Determines the orientation of the bands", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(RowsGenerator, bool, offsetof(RowsGenerator, AlternateDeadColumns), false, "Alternate Dead Rows", "Rows will not be output in alteration", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(RowsGenerator, float, offsetof(RowsGenerator, PerturbPower), 1.0f, "Perturb Power", "Adjust the intensity of perturbation", PS_VisualConsequence | PS_TinyIncrement | PS_Permutable);
}

void RowsGenerator::Construct()
{
    AddInput("Pertrub", TEXGRAPH_FLOAT);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int RowsGenerator::Execute(const Variant& param)
{
    const float perturb = GetInputSocket(0)->GetValue().getAsDoubleValue() * PerturbPower;

    const Vec2 pos = param.getVec2Safe();
    const float samplingX = Vertical ? pos.x : pos.y;

    float sineValue = sinf((samplingX + perturb) * 3.141596f * RowCount);
    if (AlternateDeadColumns)
        sineValue = CLAMP(sineValue, 0.0f, 1.0f);
    else
        sineValue = CLAMP(fabsf(sineValue), 0.0f, 1.0f);
    
    GetOutputSocket(0)->StoreValue(sineValue);
    return GRAPH_EXECUTE_COMPLETE;
}

void CheckerGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "CheckerGenerator");
    REGISTER_PROPERTY_MEMORY(CheckerGenerator, RGBA, offsetof(CheckerGenerator, ColorA), RGBA(1, 1, 1), "Color A", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(CheckerGenerator, RGBA, offsetof(CheckerGenerator, ColorB), RGBA(0,0,0), "Color B", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(CheckerGenerator, IntVec2, offsetof(CheckerGenerator, TileCount), IntVec2(4, 4), "Tiles", "Number of tiles to output", PS_Default | PS_Permutable);
}

void CheckerGenerator::Construct()
{
    ColorA = RGBA(1, 1, 1);
    ColorB = RGBA(0, 0, 0);
    TileCount = IntVec2(4, 4);
    AddInput("Perturb X", TEXGRAPH_FLOAT);
    AddInput("Perturb Y", TEXGRAPH_FLOAT);
    AddOutput("Color", TEXGRAPH_RGBA);
}

int CheckerGenerator::Execute(const Variant& param)
{
    const float dX = GetInputSocket(0)->GetValue().getAsDoubleValue();
    const float dY = GetInputSocket(1)->GetValue().getAsDoubleValue();
    const Vec2 samplePos = param.getVec2Safe() + Vec2(dX, dY);

    const int horizontalIndex = ((int)samplePos.x) / 1;
    const int verticalIndex = ((int)samplePos.y) / 1;
        
    const bool useAlternateColor = !(((int)((samplePos.x * TileCount.x)) + ((int)((samplePos.y * TileCount.y)))) & 1);
    
    if (useAlternateColor)
        GetOutputSocket(0)->StoreValue(ColorB);
    else
        GetOutputSocket(0)->StoreValue(ColorA);

    return GRAPH_EXECUTE_COMPLETE;
}

void BrickGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "BrickGenerator");
    REGISTER_PROPERTY_MEMORY(BrickGenerator, Vec2, offsetof(BrickGenerator, TileSize), Vec2(0.5f, 0.5f), "Brick Size", "Adjusts the size of each individual brick's space, gutter is taken out of this", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(BrickGenerator, float, offsetof(BrickGenerator, RowOffset), 0.5f, "Row Offset", "Adjusts the offset of each row of bricks", PS_VisualConsequence | PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(BrickGenerator, Vec2, offsetof(BrickGenerator, Gutter), Vec2(0.05f,0.05f), "Gutter", "Controls how much gutter space exists between the bricks", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(BrickGenerator, Vec2, offsetof(BrickGenerator, PerturbPower), Vec2(1, 1), "Perturb Power", "Adjusts the intensity with which perturbation is applied", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(BrickGenerator, RGBA, offsetof(BrickGenerator, BlockColor), RGBA::White, "Block Color", "Color used for the bricks", PS_VisualConsequence | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(BrickGenerator, RGBA, offsetof(BrickGenerator, GroutColor), RGBA::Black, "Grout Color", "Color to use for the mortar between blocks", PS_VisualConsequence | PS_Permutable);
}

void BrickGenerator::Construct()
{
    AddInput("Perturb X", TEXGRAPH_FLOAT);
    AddInput("Perturb Y", TEXGRAPH_FLOAT);
    AddOutput("Scalar", TEXGRAPH_CHANNEL);
}

void BrickGenerator::VersionUpdate(unsigned oldVersion)
{
    GetOutputSocket(0)->typeID = TEXGRAPH_CHANNEL;
}

int BrickGenerator::Execute(const Variant& param)
{
    const float dX = GetInputSocket(0)->GetValue().getFloatSafe() * PerturbPower.x;
    const float dY = GetInputSocket(1)->GetValue().getFloatSafe() * PerturbPower.y;
    const Vec2 pos = param.getVec2Safe() + Vec2(dX, dY);

    float xtile = pos.x / TileSize.x;
    float ytile = pos.y / TileSize.y;
    int row = floorf(ytile);

    xtile += fmodf(RowOffset * row, 1.0f);
    int column = floorf(xtile);

    xtile -= column;
    ytile -= row;

    float xGutter = Gutter.x;
    float yGutter = Gutter.y;

    float value = xtile > (0.0f + Gutter.x / 2.0f) && xtile < (1.0f - Gutter.x / 2.0f) && ytile > (0.0f + Gutter.y / 2.0f) && ytile < (1.0f - Gutter.y / 2.0f) ? 1.0f : 0.0f;

    // Get the maximum value, then clip
    GetOutputSocket(0)->StoreValue(value > 0.5f ? BlockColor : GroutColor);

    return GRAPH_EXECUTE_COMPLETE;
}

void NoiseGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "NoiseGenerator");
    REGISTER_PROPERTY_MEMORY(NoiseGenerator, Vec3, offsetof(NoiseGenerator, Period), Vec3(8, 8, 8), "Period", "Density of the noise in a single tiling iteration", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(NoiseGenerator, bool, offsetof(NoiseGenerator, Inverted), false, "Invert", "Output will be inverted", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(NoiseGenerator, int, FAST_NOISE_ADDR(NoiseGenerator, m_seed), 0, "Seed", "Controls the seed value for the RNG", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(NoiseGenerator, float, FAST_NOISE_ADDR(NoiseGenerator, m_frequency), 0, "Frequency", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_ENUM_MEMORY(NoiseGenerator, int, FAST_NOISE_ADDR(NoiseGenerator, m_interp), 0, "Interp", "", PS_Default, INTERP_NAMES);
    REGISTER_ENUM_MEMORY(NoiseGenerator, int, FAST_NOISE_ADDR(NoiseGenerator, m_noiseType), 0, "Noise Func", "", PS_Secret, VALUE_NAMES);
}

void NoiseGenerator::Construct()
{
    Inverted = false;
    noise_.m_frequency = 8;
    noise_.m_interp = FastNoise::Interp::InterpLinear;
    noise_.SetNoiseType(FastNoise::NoiseType::Value);
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int NoiseGenerator::Execute(const Variant& param)
{
    Vec3 vec = param.getVec3Safe() * Period;
    float value = noise_.GetValue(vec.x, vec.y, vec.z);
    value = NORMALIZE(value, -1, 1);
    float g = noise_.GetValue(vec.x * 1024, vec.y * 4096, vec.z * 512);
    g = NORMALIZE(g, -1, 1);
    float b = noise_.GetValue(vec.x * 666, vec.y * 1111, vec.z * 1024);
    b = NORMALIZE(b, -1, 1);
    if (Inverted)
    {
        value = 1.0f - value;
        g = 1.0f - g;
        b = 1.0f - g;
    }
    GetOutputSocket(0)->StoreValue(RGBA(value, g, b));
    return GRAPH_EXECUTE_COMPLETE;
}

void FBMGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "FBMGenerator");
    REGISTER_PROPERTY_MEMORY(FBMGenerator, Vec3, offsetof(FBMGenerator, Period), Vec3(8, 8, 8), "Period", "Density of the noise in a single tiling iteration", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(FBMGenerator, int, FAST_NOISE_ADDR(FBMGenerator, m_seed), 0, "Seed", "Sets the seed for the RNG used", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(FBMGenerator, bool, offsetof(FBMGenerator, Inverted), false, "Invert", "Output will be inverted", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(FBMGenerator, float, FAST_NOISE_ADDR(FBMGenerator, m_lacunarity), 2.0f, "Lacunarity", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(FBMGenerator, float, FAST_NOISE_ADDR(FBMGenerator, m_gain), 0.5f, "Gain", "", PS_SmallIncrement | PS_Permutable);
    REGISTER_ENUM_MEMORY(FBMGenerator, int, FAST_NOISE_ADDR(FBMGenerator, m_interp), 0, "Interp", "", PS_Default, INTERP_NAMES);
    REGISTER_PROPERTY_MEMORY(FBMGenerator, unsigned, FAST_NOISE_ADDR(FBMGenerator, m_octaves), 3, "Octaves", "How many octaves will be accumulated", PS_Default | PS_Permutable);
    REGISTER_ENUM_MEMORY(FBMGenerator, int, FAST_NOISE_ADDR(FBMGenerator, m_noiseType), 0, "Noise Func", "", PS_Secret, VALUE_NAMES);    
    REGISTER_ENUM_MEMORY(FBMGenerator, int, FAST_NOISE_ADDR(FBMGenerator, m_fractalType), 0, "Fractal Type", "", PS_Default, FRACTAL_NAMES);
}

void FBMGenerator::Construct()
{
    noise_.SetFrequency(0.02f);
    noise_.SetFractalGain(0.5f);
    noise_.SetFractalLacunarity(2.0f);
    noise_.SetFractalOctaves(3);
    noise_.SetInterp(FastNoise::Interp::InterpLinear);
    noise_.SetNoiseType(FastNoise::NoiseType::GradientFractal);
    noise_.SetFractalType(FastNoise::FractalType::FBM);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int FBMGenerator::Execute(const Variant& param)
{
    Vec2 vec = param.getVec2Safe();// *Period;
    Vec4 coord = Make4D(vec, Vec2(Period.x, Period.y));

    float value = 0.0f;
    if (noise_.m_fractalType == FastNoise::FractalType::FBM)
    {
        unsigned seed = noise_.GetSeed();
        float sum = anl::gradient_noise4D(coord.x, coord.y, coord.z, coord.w, seed, anl::linearInterp);
        float max = 1.0f;
        float amp = 1.0f;
        unsigned int i = 0;

        while (++i < noise_.m_octaves)
        {
            seed = (seed + i) & 0x7fffffff;
            coord.x *= noise_.m_lacunarity;
            coord.y *= noise_.m_lacunarity;
            coord.z *= noise_.m_lacunarity;
            coord.w *= noise_.m_lacunarity;

            amp *= noise_.m_gain;
            max += amp;
            sum += anl::gradient_noise4D(coord.x, coord.y, coord.z, coord.w, ++seed, anl::linearInterp) * amp;
        }
        value = sum / max;
    }
    else if (noise_.m_fractalType == FastNoise::FractalType::Billow)
    {
        int seed = noise_.m_seed;
        float sum = fabsf(anl::gradval_noise4D(coord.x, coord.y, coord.z, coord.w, seed, anl::linearInterp)) *2.0f - 1.0f;
        float max = 1.0f;
        float amp = 1.0f;
        unsigned int i = 0;

        while (++i < noise_.m_octaves)
        {
            seed = (seed + i) & 0x7fffffff;
            coord.x *= noise_.m_lacunarity;
            coord.y *= noise_.m_lacunarity;
            coord.z *= noise_.m_lacunarity;
            coord.w *= noise_.m_lacunarity;

            amp *= noise_.m_gain;
            max += amp;
            sum += (fabsf(anl::gradval_noise4D(coord.x, coord.y, coord.z, coord.w, seed, anl::linearInterp)) * 2.0f - 1.0f) * amp;
        }
        value = sum;
    }
    else if (noise_.m_fractalType == FastNoise::FractalType::RigidMulti)
    {
        int seed = noise_.m_seed;
        float sum = 1.0f - fabsf(anl::gradval_noise4D(coord.x, coord.y, coord.z, coord.w, seed, anl::linearInterp));
        float amp = 1.0f;
        unsigned int i = 0;
        float weight = 1.0f;

        while (++i < noise_.m_octaves)
        {
            seed = (seed + i) & 0x7fffffff;
            coord.x *= noise_.m_lacunarity;
            coord.y *= noise_.m_lacunarity;
            coord.z *= noise_.m_lacunarity;
            coord.w *= noise_.m_lacunarity;

            amp *= noise_.m_gain;
            sum -= (1.0f - fabsf(anl::gradval_noise4D(coord.x, coord.y, coord.z, coord.w, seed, anl::linearInterp))) * amp;
        }
        value = sum;
    }

    //float value = noise_.GetGradientFractal(coord.x, coord.y, coord.z);
    value = NORMALIZE(value, -1, 1);
    if (Inverted)
        value = 1.0f - value;
    GetOutputSocket(0)->StoreValue(value);
    return GRAPH_EXECUTE_COMPLETE;
}

void PerlinNoiseGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "PerlinNoiseGenerator");
    REGISTER_PROPERTY_MEMORY(PerlinNoiseGenerator, Vec3, offsetof(PerlinNoiseGenerator, Period), Vec3(8, 8, 8), "Period", "Density of the noise in a single tiling iteration", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(PerlinNoiseGenerator, bool, offsetof(PerlinNoiseGenerator, Inverted), false, "Invert", "Output will be flipped", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(PerlinNoiseGenerator, int, FAST_NOISE_ADDR(PerlinNoiseGenerator, m_seed), 0, "Seed", "Sets the seed for the RNG", PS_Default | PS_Permutable);
    REGISTER_ENUM_MEMORY(PerlinNoiseGenerator, int, FAST_NOISE_ADDR(PerlinNoiseGenerator, m_interp), 1, "Interp", "", PS_Default, INTERP_NAMES);
    REGISTER_ENUM_MEMORY(PerlinNoiseGenerator, int, FAST_NOISE_ADDR(PerlinNoiseGenerator, m_noiseType), 0, "Noise Func", "", PS_Secret, VALUE_NAMES);
}

void PerlinNoiseGenerator::Construct()
{
    Inverted = false;
    noise_.SetFrequency(0.1f);
    noise_.SetFractalLacunarity(2.0f);
    noise_.SetFractalOctaves(3);
    noise_.SetNoiseType(FastNoise::NoiseType::Gradient);
    noise_.SetFractalType(FastNoise::FractalType::RigidMulti);
    noise_.SetInterp(FastNoise::Interp::InterpHermite);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int PerlinNoiseGenerator::Execute(const Variant& param)
{
    Vec3 vec = param.getVec3Safe();// *Period;
    Vec4 coord = Make4D(param.getVec2Safe(), Vec2(Period.x, Period.y));
    float value = anl::gradval_noise4D(coord.x, coord.y, coord.z, coord.w, noise_.GetSeed(), anl::linearInterp);
    value = NORMALIZE(value, -1, 1);
    if (Inverted)
        value = 1.0f - value;
    GetOutputSocket(0)->StoreValue(value);
    return GRAPH_EXECUTE_COMPLETE;
}

void VoronoiGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "VoronoiGenerator");
    REGISTER_PROPERTY_MEMORY(VoronoiGenerator, Vec3, offsetof(VoronoiGenerator, Period), Vec3(8, 8, 8), "Period", "Density of the noise in a single tiling iteration", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(VoronoiGenerator, bool, offsetof(VoronoiGenerator, Inverted), false, "Invert", "Output will be flipped", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(VoronoiGenerator, int, FAST_NOISE_ADDR(VoronoiGenerator, m_seed), 0, "Seed", "Sets the seed for the RNG", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(VoronoiGenerator, float, FAST_NOISE_ADDR(VoronoiGenerator, m_frequency), 0.5f, "Frequency", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_ENUM_MEMORY(VoronoiGenerator, int, FAST_NOISE_ADDR(VoronoiGenerator, m_cellularDistanceFunction), 0, "Distance Func", "Determines what method is used for output of the cell values", PS_Default, CELL_DISTANCE_NAMES);
    REGISTER_ENUM_MEMORY(VoronoiGenerator, int, FAST_NOISE_ADDR(VoronoiGenerator, m_interp), 0, "Interp", "", PS_Default, INTERP_NAMES);
    REGISTER_ENUM_MEMORY(VoronoiGenerator, int, FAST_NOISE_ADDR(VoronoiGenerator, m_cellularReturnType), 0, "Type", "", PS_Default, CELL_RETURN_NAMES);
}

void VoronoiGenerator::Construct()
{
    Period.x = 8;
    Period.y = 8;
    noise_.SetFrequency(0.5f);
    Inverted = false;
    noise_.SetCellularDistanceFunction(FastNoise::CellularDistanceFunction::Euclidean);
    noise_.SetCellularReturnType(FastNoise::CellularReturnType::CellValue);
    noise_.SetInterp(SprueEngine::FastNoise::Interp::InterpLinear);
    noise_.SetNoiseType(FastNoise::NoiseType::Cellular);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int VoronoiGenerator::Execute(const Variant& param)
{
    Vec2 vec = param.getVec2Safe();
    Vec4 coord = Make4D(vec, Vec2(Period.x, Period.y));
    Vec4 f;
    Vec4 dist;
    //anl::cellular_function4D(coord.x, coord.y, coord.z, coord.w, noise_.m_seed, &f[0], &dist[0], anl::distEuclid4);
    //float value = dist[0];
    float value = noise_.GetCellular(coord.x, coord.y, coord.z, coord.w);
    switch (noise_.m_cellularReturnType)
    {
    case FastNoise::CellularReturnType::CellValue:
        value = CLAMP(value, -1, 1);
        value = NORMALIZE(value, -1, 1);
        break;
    default:
        value = CLAMP(value, 0, 1);
        break;
    }
    if (Inverted)
        value = 1.0f - value;
    GetOutputSocket(0)->StoreValue(value);
    return GRAPH_EXECUTE_COMPLETE;
}

void UberNoiseGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "UberNoiseGenerator");
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, Vec3, offsetof(UberNoiseGenerator, Period), Vec3(8, 8, 8), "Period", "Density of the noise in a single tiling iteration", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, int, FAST_NOISE_ADDR(UberNoiseGenerator, m_seed), 0, "Seed", "Sets the seed for the RNG used", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, float, offsetof(UberNoiseGenerator, Lacunarity), 2.0f, "Lacunarity", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, float, offsetof(UberNoiseGenerator, Gain), 0.5f, "Gain", "", PS_SmallIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, unsigned, offsetof(UberNoiseGenerator, Octaves), 3, "Octaves", "How many octaves will be accumulated", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, float, offsetof(UberNoiseGenerator, PerturbFeatures), 0.2f, "Perturb Features", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, float, offsetof(UberNoiseGenerator, Sharpness), 0.7f, "Sharpness", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, float, offsetof(UberNoiseGenerator, AmplifyFeatures), 0.7f, "Amplify Features", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, float, offsetof(UberNoiseGenerator, AltitudeErosion), 0.7f, "Altitude Erosion", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, float, offsetof(UberNoiseGenerator, RidgeErosion), 0.7f, "Ridge Erosion", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(UberNoiseGenerator, float, offsetof(UberNoiseGenerator, SlopeErosion), 0.7f, "Slope Erosion", "", PS_TinyIncrement | PS_Permutable);
}

void UberNoiseGenerator::Construct()
{
    AddOutput("", TEXGRAPH_FLOAT);
}

static float hash1(float n)
{
    return Frac(n * 17.0f * Frac(n*0.3183099f));
}

static Vec4 DerivativeNoise(Vec4 p, unsigned seed)
{
    float baseNoise = anl::gradval_noise4D(p.x, p.y, p.z, p.w, seed, anl::quinticInterp);
    float H = 0.5f;
    const float dx = anl::gradval_noise4D(p.x + H, p.y, p.z, p.w, seed, anl::quinticInterp) - anl::gradient_noise4D(p.x - H, p.y, p.z, p.w, seed, anl::quinticInterp);
    const float dy = anl::gradval_noise4D(p.x, p.y + H, p.z, p.w, seed, anl::quinticInterp) - anl::gradient_noise4D(p.x, p.y - H, p.z, p.w, seed, anl::quinticInterp);
    const float dz = anl::gradval_noise4D(p.x, p.y, p.z + H, p.w, seed, anl::quinticInterp) - anl::gradient_noise4D(p.x, p.y, p.z - H, p.w, seed, anl::quinticInterp);

    auto v = Vec3(dx, dy, dz);
    return Vec4(baseNoise, v.x, v.y, 1);// v.z);
}

int UberNoiseGenerator::Execute(const Variant& param)
{
    Vec4 pos = Make4D(param.getVec2Safe(), Vec2(Period.x, Period.y));

    float value = 0.0f;
    unsigned seed = noise_.GetSeed();
    float sum = 0.0f, featureNoise = 0.0f;
    auto noiseValue = DerivativeNoise(pos, seed);
    featureNoise = noiseValue.x;

    float max = 0.0f;
    float amp = 1.0f;
    float currentGain = Gain;
    unsigned int i = 0;

    Vec3 lDerivative(noiseValue.y, noiseValue.z, noiseValue.w);
    pos += lDerivative * PerturbFeatures;
    Vec3 ridgeErosionDerivative(0, 0, 0);
    Vec3 slopeErosionDerivative(0, 0, 0);
    while (++i < Octaves)
    {
    // Accumulate max (amp * 2 covers full damped amp range)
        max += amp * 2 + (currentGain * AmplifyFeatures);

    // Accumulate derivatives
        ridgeErosionDerivative += lDerivative * RidgeErosion;
        slopeErosionDerivative += lDerivative * SlopeErosion;

    // Sharpness
        float ridgedNoise = ((1.0f) - fabsf(featureNoise));
        float billowNoise = featureNoise * featureNoise;
        featureNoise = SprueLerp(featureNoise, billowNoise, std::max(0.0f, Sharpness));
        featureNoise = SprueLerp(featureNoise, ridgedNoise, fabsf(std::min(0.0f, Sharpness)));

    // Slope Erosion
        sum += amp * featureNoise * (1.0f / (1.0f + slopeErosionDerivative.LengthSq()));

    // Amplitude damping
        float dampedAmp = amp * (1.0f - (RidgeErosion / (1.0f * ridgeErosionDerivative.LengthSq())));
        sum += dampedAmp * featureNoise * (1.0f / (1.0f * slopeErosionDerivative.LengthSq()));
        amp *= SprueLerp(currentGain, currentGain * SprueLerp(0.0f, 1.0f, sum/max), AltitudeErosion);

    // Amplify features
        sum += featureNoise * currentGain * AmplifyFeatures;
        currentGain = currentGain * AmplifyFeatures;

    // Prepare for next pass
        seed = (seed + i) & 0x7fffffff;
        pos += lDerivative * PerturbFeatures;
        pos.x *= Lacunarity;
        pos.y *= Lacunarity;
        pos.z *= Lacunarity;
        pos.w *= Lacunarity;

        noiseValue = DerivativeNoise(pos, ++seed);
        featureNoise = noiseValue.x;
        lDerivative = Vec3(noiseValue.y, noiseValue.z, noiseValue.w);
    }
    value = sum / max;
    value = NORMALIZE(value, -1.0f, 1.0f);
    GetOutputSocket(0)->StoreValue(value);
    return GRAPH_EXECUTE_COMPLETE;
}

void TextureBombGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "TextureBombGenerator");
    REGISTER_PROPERTY_MEMORY(TextureBombGenerator, int, FAST_NOISE_ADDR(TextureBombGenerator, m_seed), 0, "Seed", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(TextureBombGenerator, RangedFloat, offsetof(TextureBombGenerator, AngularRange), RangedFloat(0,0), "Rot. Range", "", PS_Default | PS_Rotation | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(TextureBombGenerator, RangedFloat, offsetof(TextureBombGenerator, ScaleRange), RangedFloat(1,1), "Scale Range", "", PS_Default | PS_Scale | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(TextureBombGenerator, unsigned, offsetof(TextureBombGenerator, DesiredDensity), 4, "Density", "", PS_Default | PS_Permutable);
    REGISTER_RESOURCE(TextureBombGenerator, BitmapResource, GetBitmapResourceHandle, SetBitmapResourceHandle, GetImageData, SetImageData, ResourceHandle("Image"), "Image", "", PS_VisualConsequence | PS_Permutable);
    //context->Register("TextureBombGenerator", new ResourcePropertyImpl<TextureBombGenerator, BitmapResource>(&TextureBombGenerator::GetBitmapResourceHandle, &TextureBombGenerator::SetBitmapResourceHandle, &TextureBombGenerator::GetImageData, &TextureBombGenerator::SetImageData, ResourceHandle(), "Image", "", PS_Default));
}

void TextureBombGenerator::Construct()
{
    AddOutput("Color", TEXGRAPH_CHANNEL);
    AddOutput("Alpha", TEXGRAPH_FLOAT);
}

int TextureBombGenerator::Execute(const Variant& param)
{
    Vec2 pos = param.getVec2Safe();
    float output = 0.0f;

    Vec3 offset(5, 7, 11);

    RGBA current = RGBA(0, 0, 0, 0);

    if (ImageData && ImageData->GetImage())
    {
        for (int np = 0; np < DesiredDensity; ++np)
        {
    #define GET_RAND_VAL(NAME) float NAME = noise_.GetNoise(offset.x, offset.y * 67, np); \
                offset *= 1.2f; \
                NAME *= 2.0f; NAME += 1.0f;
                //NORMALIZE(NAME, -1, 2);

            GET_RAND_VAL(px);   // X offset
            GET_RAND_VAL(py);   // Y offset
            GET_RAND_VAL(r);    // rotation
            GET_RAND_VAL(s);    // scale
    #undef GET_RAND_VAL
            s = NORMALIZE(s, -1, 1) * ScaleRange.GetRange();
            r = AngularRange.Clip(r * 359.0f * 0.5f);
            s = ScaleRange.Clip(s);
            Mat3x3 transMat; transMat.SetTranslation(Vec2(px, py));
            Mat3x3 rotMat; rotMat.SetRotation(r);
            Vec3 coord = rotMat * Vec3((pos.x + px) * s, (pos.y + py) * s, 0.0f);//(transMat * rotMat) * Vec3(pos.x ,pos.y, 0.0f);
            coord.x = fmodf(coord.x, 1.0f);
            while (coord.x < 0.0f)
                coord.x += 1.0f;
            coord.y = fmodf(coord.y, 1.0f);
            while (coord.y < 0.0f)
                coord.y += 1.0f;

            RGBA newColor = ImageData->GetImage()->getBilinear(coord.x, coord.y);
            if (newColor.a >= current.a)
                current = newColor;
            //current = current + newColor;
        }
    }

    GetOutputSocket(0)->StoreValue(current);
    GetOutputSocket(1)->StoreValue(current.a);
    return GRAPH_EXECUTE_COMPLETE;
}

static const char* TexGenGradientNames[] = {
    "Linear",
    "Reflected",
    "Radial",
    "Angle",
    0x0
};

void BitmapGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "BitmapGenerator");
    REGISTER_RESOURCE(BitmapGenerator, BitmapResource, GetBitmapResourceHandle, SetBitmapResourceHandle, GetImageData, SetImageData, ResourceHandle("Image"), "Image", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(BitmapGenerator, bool, offsetof(BitmapGenerator, WrapX), true, "Wrap X", "Whether to wrap or clamp the X coordinate", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(BitmapGenerator, bool, offsetof(BitmapGenerator, WrapY), true, "Wrap Y", "Whether to wrap or clamp the Y coordinate", PS_Default | PS_Permutable);
}

void BitmapGenerator::Construct()
{
    AddOutput("Color", TEXGRAPH_RGBA);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int BitmapGenerator::Execute(const Variant& param)
{
    Vec2 pos = param.getVec2Safe();
    if (ImageData && ImageData->GetImage())
    {
        if (WrapX)
            pos.x = fmodf(pos.x, 1.0f);
        if (WrapY)
            pos.y = fmodf(pos.y, 1.0f);
        
        RGBA pixel = ImageData->GetImage()->getBilinear(pos.x, pos.y);
        GetOutputSocket(0)->StoreValue(pixel);
        GetOutputSocket(1)->StoreValue(pixel.r);
    }
    return GRAPH_EXECUTE_COMPLETE;
}

void SVGGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "SVGGenerator");
    REGISTER_RESOURCE(SVGGenerator, SVGResource, GetSVGResourceHandle, SetSVGResourceHandle, GetSVGData, SetSVGData, ResourceHandle("SVG"), "SVG FIle", "", PS_Default);
    REGISTER_PROPERTY_MEMORY(SVGGenerator, IntVec2, offsetof(SVGGenerator, Size), IntVec2(128,128), "Size", "Pixel size to rasterize the SVG file to", PS_Default);
}

void SVGGenerator::Construct()
{
    AddOutput("Color", TEXGRAPH_RGBA);
    AddOutput("Scalar", TEXGRAPH_FLOAT);
}

int SVGGenerator::Execute(const Variant& param)
{
    Vec2 pos = param.getVec2Safe();
    if (svgResource && !rasterData)
        rasterData = svgResource->GetImage(Size.x, Size.y);
    if (rasterData)
    {
        //if (WrapX)
        //    pos.x = fmodf(pos.x, 1.0f);
        //if (WrapY)
        //    pos.y = fmodf(pos.y, 1.0f);

        RGBA pixel = rasterData->getBilinear(pos.x, pos.y);
        GetOutputSocket(0)->StoreValue(pixel);
        GetOutputSocket(1)->StoreValue(pixel.r);
    }
    return GRAPH_EXECUTE_COMPLETE;
}

static const char* GRADIENT_NAMES[] = {
    "Linear",
    "Reflected",
    "Radial",
    "Angle",
    0x0
};

void GradientGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "GradientGenerator");
    REGISTER_ENUM_MEMORY(GradientGenerator, int, offsetof(GradientGenerator, type_), 0, "Type", "", PS_Default, GRADIENT_NAMES);
    REGISTER_PROPERTY_MEMORY(GradientGenerator, RGBA, offsetof(GradientGenerator, Start), RGBA(1,1,1), "Start Color", "", PS_Default);
    REGISTER_PROPERTY_MEMORY(GradientGenerator, RGBA, offsetof(GradientGenerator, End), RGBA(0, 0, 0), "End Color", "", PS_Default);
    REGISTER_PROPERTY_MEMORY(GradientGenerator, float, offsetof(GradientGenerator, Length), 1.0f, "Length", "", PS_TinyIncrement);
    REGISTER_PROPERTY_MEMORY(GradientGenerator, float, offsetof(GradientGenerator, Angle), 1.0f, "Angle", "", PS_Default);
    REGISTER_PROPERTY_MEMORY(GradientGenerator, Vec2, offsetof(GradientGenerator, Offset), Vec2(0, 0), "Offset", "", PS_Default);
}

void GradientGenerator::Construct()
{
    AddOutput("Out", TEXGRAPH_CHANNEL);
}

int GradientGenerator::Execute(const Variant& param)
{
    Vec2 UV = param.getVec2Safe();

    // Check direction, get offset displacement, then normalize, get angle as DP
    const Vec2 dirVec = Vec2::PositiveY.Rotate(Angle);
    float sampleToOriginDP = (UV - Offset).Normalized().Dot(dirVec);
    
    UV -= Offset;
    
    RGBA finalColor;
    
    switch (GetGradientType())
    {
    case TGG_Reflected:
    case TGG_Linear: {
        const Vec2 emissionLineVec = dirVec.PerpendicularClockwise();
        float distanceFromLine = (emissionLineVec.Cross(UV) / Length) * -1;
        if (type_ == TGG_Reflected)
            distanceFromLine = fabsf(distanceFromLine);
        finalColor = SprueLerp(Start, End, CLAMP01(distanceFromLine));
        } break;
    case TGG_Radial: {
        float sampleLen = UV.Length() / Length;
        finalColor = SprueLerp(End, Start, CLAMP01(sampleLen));
        } break;
    case TGG_Angle:
        finalColor = SprueLerp(End, Start, NORMALIZE(sampleToOriginDP, -1.0f, 1.0f) / Length);
        break;
    }

    GetOutputSocket(0)->StoreValue(finalColor);

    return GRAPH_EXECUTE_COMPLETE;
}

void GaborNoiseGenerator::Register(Context* context)
{
    COPY_PROPERTIES(GraphNode, GaborNoiseGenerator);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, float, offsetof(GaborNoiseGenerator, K), 1.0f, "K", "", PS_VisualConsequence | PS_TinyIncrement);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, float, offsetof(GaborNoiseGenerator, Alpha), 0.05f, "Alpha", "", PS_VisualConsequence | PS_TinyIncrement);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, float, offsetof(GaborNoiseGenerator, F0), 0.0625f, "F0", "", PS_VisualConsequence | PS_TinyIncrement);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, float, offsetof(GaborNoiseGenerator, Omega), PI / 4.0f, "Omega", "", PS_VisualConsequence | PS_TinyIncrement);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, float, offsetof(GaborNoiseGenerator, Impulses), 64.0f, "Impulses", "", PS_VisualConsequence);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, float, offsetof(GaborNoiseGenerator, Period), 256.0f, "Period", "", PS_VisualConsequence);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, float, offsetof(GaborNoiseGenerator, Offset), 0.0f, "Offset", "", PS_VisualConsequence | PS_TinyIncrement);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, float, FAST_NOISE_ADDR(GaborNoiseGenerator, m_frequency), 8, "Frequency", "", PS_Default);
    REGISTER_PROPERTY_MEMORY(GaborNoiseGenerator, unsigned, FAST_NOISE_ADDR(GaborNoiseGenerator, m_seed), 0, "Seed", "", PS_VisualConsequence);
}

void GaborNoiseGenerator::Construct()
{
    noise_.m_seed = 0;
    noise_.m_frequency = 8;
    noise_.SetNoiseType(FastNoise::NoiseType::Value);
    AddOutput("", TEXGRAPH_FLOAT);
}

int GaborNoiseGenerator::Execute(const Variant& param)
{
    const Vec2 p = param.getVec2Safe();

    const float kernelRadius = std::sqrtf(-std::log(0.05) / PI) / Alpha;
    const float impulseDensity = Impulses / (PI * kernelRadius * kernelRadius);

    Vec4 pos = Make4D(p, Vec2(Period, Period));
    float x = pos.x; //p.x * Period;
    float y = pos.y; //p.y * Period;
    float z = pos.z;
    float w = pos.w;

    x /= kernelRadius, y /= kernelRadius;
    float int_x = std::floor(x), int_y = std::floor(y);
    float frac_x = x - int_x, frac_y = y - int_y;
    int i = int(int_x), j = int(int_y);
    float noise = 0.0;
    for (int di = -1; di <= +1; ++di) {
        for (int dj = -1; dj <= +1; ++dj) {
            noise += CellValue(i + di, j + dj, frac_x - di, frac_y - dj, impulseDensity, kernelRadius);
        }
    }

    GetOutputSocket(0)->StoreValue(noise);
    return GRAPH_EXECUTE_COMPLETE;
}

float GaborNoiseGenerator::CellValue(int i, int j, float x, float y, float impulseDensity, float kernelRadius)
{
    unsigned s = Morton(i, j) + Offset; // nonperiodic noise
    if (s == 0) s = 1;
    
    double number_of_impulses_per_cell = impulseDensity * kernelRadius * kernelRadius;
    unsigned number_of_impulses = NORMALIZE(noise_.GetValue(i, j), -1, 1) * number_of_impulses_per_cell;
    float noise = 0.0;
    for (unsigned i = 0; i < number_of_impulses; ++i) {
        float x_i = NORMALIZE(noise_.GetValue(noise_.m_seed + i, j * 13), -1, 1);
        float y_i = NORMALIZE(noise_.GetValue(noise_.m_seed + i * 21, j), -1, 1);
        float w_i = noise_.GetValue(noise_.m_seed + i * 37, j * 37463);
        float omega_0_i = NORMALIZE(noise_.GetValue(noise_.m_seed + i * 471, j * 7), -1, 1) * 2.0 * PI;
        float x_i_x = x - x_i;
        float y_i_y = y - y_i;
        if (((x_i_x * x_i_x) + (y_i_y * y_i_y)) < 1.0) {
            noise += w_i * Gabor(K, Alpha, F0, Omega, x_i_x * kernelRadius, y_i_y * kernelRadius); // anisotropic
            //noise += w_i * gabor(K_, a_, F_0_, omega_0_i, x_i_x * kernel_radius_, y_i_y * kernel_radius_); // isotropic
        }
    }
    return noise;
}

unsigned GaborNoiseGenerator::Morton(unsigned x, unsigned y)
{
    unsigned z = 0;
    for (unsigned i = 0; i < (sizeof(unsigned) * CHAR_BIT); ++i) {
        z |= ((x & (1 << i)) << i) | ((y & (1 << i)) << (i + 1));
    }
    return z;
}

float GaborNoiseGenerator::Gabor(float K, float a, float F_0, float omega_0, float x, float y)
{
    float gaussian_envelop = K * std::exp(-PI * (a * a) * ((x * x) + (y * y)));
    float sinusoidal_carrier = std::cos(2.0 * PI * F_0 * ((x * std::cos(omega_0)) + (y * std::sin(omega_0))));
    return gaussian_envelop * sinusoidal_carrier;
}

}