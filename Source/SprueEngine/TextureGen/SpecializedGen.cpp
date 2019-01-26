#include "SpecializedGen.h"

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Math/Trig.h>

#define FAST_NOISE_ADDR(TYPE, PARAM) (offsetof(TYPE, noise_) + offsetof(FastNoise, PARAM))

namespace SprueEngine
{
#define ARC(x) sqrtf(1.0f-(x-0.5f)*(x-0.5f)/0.25f);

    void ScratchesGenerator::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "ScratchesGenerator");
        REGISTER_PROPERTY_MEMORY(ScratchesGenerator, int, (offsetof(ScratchesGenerator, noise_) + offsetof(FastNoise, m_seed)), 0, "Seed", "Sets the seed for the RNG", PS_Default | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ScratchesGenerator, unsigned, offsetof(ScratchesGenerator, Density), 18, "Density", "How many scratches to attempt to place", PS_Default | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ScratchesGenerator, float, offsetof(ScratchesGenerator, Length), 0.1f, "Length", "Length of each scratch", PS_TinyIncrement | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ScratchesGenerator, bool, offsetof(ScratchesGenerator, Inverted), false, "Inverted", "Output will be inverted to black on white", PS_Default | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ScratchesGenerator, bool, offsetof(ScratchesGenerator, FadeOff), false, "Fade Off", "Scratches will taper off", PS_Default | PS_Permutable);
    }

    void ScratchesGenerator::Construct()
    {
        AddOutput("Scalar", TEXGRAPH_FLOAT);
    }

    int ScratchesGenerator::Execute(const Variant& parameter)
    {
        Vec2 pos = parameter.getVec2Safe();
        float output = 0.0f;

        Vec3 offset(5, 7, 11);
        const float len2 = Length * Length;

        for (int np = 0; np < Density; ++np)
        {
#define GET_RAND_VAL(NAME) float NAME = noise_.GetWhiteNoise(offset.x, offset.y * 67); \
            offset *= 1.2f; \
            NAME *= 2.0f; NAME += 1.0f;
            //NORMALIZE(NAME, -1, 1);
                    
            GET_RAND_VAL(px);
            GET_RAND_VAL(py);
            GET_RAND_VAL(ox);
            GET_RAND_VAL(oy);
#undef GET_RAND_VAL

            Vec2 start(ox, oy);
            Vec2 end(px, py);
            Vec2 closest = ClosestPoint(start, end, pos);
            float distance = (closest - pos).LengthSq();
            if (distance < len2)
            {
                float mult = 1.0f;
                if (FadeOff)
                {
                    mult = (closest - start).LengthSq() / (start - end).LengthSq();
                }
                distance = len2 - distance;
                distance = NORMALIZE(distance, 0, len2);
                output += CLAMP01(distance) * mult;
            }
        }

        output = CLAMP01(output);
        if (Inverted)
            output = 1.0f - output;
        GetOutputSocket(0)->StoreValue(output);
        return GRAPH_EXECUTE_COMPLETE;
    }


static const char* TextureFunctionFunctionNames[] =
{
    "None",
    "Sin",
    "Cos",
    "Saw",
    "Square",
    "Triangle",
    0x0
};

static const char* TextureFunctionMixNames[]
{
    "Add",
    "Subtract",
    "Mul",
    "Divide",
    "Max",
    "Min",
    "Pow",
    0x0
};

    void TextureFunction2D::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "TextureFunction2D");
        REGISTER_ENUM_MEMORY(TextureFunction2D, int, offsetof(TextureFunction2D, XFunction), TFF_Sin, "X Func", "", PS_Default, TextureFunctionFunctionNames);
        REGISTER_ENUM_MEMORY(TextureFunction2D, int, offsetof(TextureFunction2D, YFunction), TFF_Sin, "Y Func", "", PS_Default, TextureFunctionFunctionNames);
        REGISTER_PROPERTY_MEMORY(TextureFunction2D, Vec2, offsetof(TextureFunction2D, Offset), Vec2(), "Offset", "", PS_Default | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(TextureFunction2D, Vec2, offsetof(TextureFunction2D, Period), Vec2(128, 128), "Period", "", PS_Default | PS_Permutable);
        REGISTER_ENUM_MEMORY(TextureFunction2D, int, offsetof(TextureFunction2D, Mix), TFM_Add, "Mix", "", PS_Default, TextureFunctionMixNames);
    }

    void TextureFunction2D::Construct()
    {
        AddInput("X Ofs.", TEXGRAPH_FLOAT);
        AddInput("Y Ofs.", TEXGRAPH_FLOAT);
        AddOutput("Scalar", TEXGRAPH_FLOAT);
    }

    int TextureFunction2D::Execute(const Variant& param)
    {
        Vec2 coord = param.getVec2Safe();

        if (GetInputSocket(0)->HasConnections())
            coord.x += GetInputSocket(0)->GetValue().getFloatSafe();
        if (GetInputSocket(1)->HasConnections())
            coord.y += GetInputSocket(1)->GetValue().getFloatSafe();

        coord += Offset;
        coord *= Period;

        const float xVal = DoFunction(coord.x, XFunction);
        const float yVal = DoFunction(coord.y, YFunction);

        float finalVal = 0.0f;
        switch (Mix)
        {
        case TFM_Add:
            finalVal = xVal + yVal;
            break;
        case TFM_Subtract:
            finalVal = xVal - yVal;
            break;
        case TFM_Mul:
            finalVal = xVal * yVal;
            break;
        case TFM_Divide:
            finalVal = xVal / (yVal != 0.0f ? yVal : 1.0f);
            break;
        case TFM_Max:
            finalVal = SprueMax(xVal, yVal);
            break;
        case TFM_Min:
            finalVal = SprueMin(xVal, yVal);
            break;
        case TFM_Pow:
            finalVal = powf(xVal, yVal);
            break;
        }

        finalVal = CLAMP01(finalVal);
        GetOutputSocket(0)->StoreValue(finalVal);
        return GRAPH_EXECUTE_COMPLETE;
    }

    float TextureFunction2D::DoFunction(float val, TextureFunctionFunction func) const
    {
        switch (func)
        {
        case TFF_Sin:
            return sinf(val);
        case TFF_Cos:
            return cosf(val);
        case TFF_Saw:
            return 1.0f - (val - floorf(val));
        case TFF_Square:
            return sgn(cosf(val));
        case TFF_Triangle:
            return 1.0f - fabsf(fmodf(val, 2.0f) - 1.0f);
        }
        return 0.0f;
    }

    void ChainGenerator::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "ChainGenerator");
        REGISTER_PROPERTY_MEMORY(ChainGenerator, RGBA, offsetof(ChainGenerator, Background), RGBA::Black, "Background Color", "Color for areas not covered by the chain", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ChainGenerator, RGBA, offsetof(ChainGenerator, CenterLink), RGBA::White, "Center Link Color", "Color of the center link in the mail", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ChainGenerator, RGBA, offsetof(ChainGenerator, ConnectingLink), RGBA::Gray, "Connecting Links Color", "Color of the links that connect into the center link", PS_VisualConsequence | PS_Permutable);
    }

    void ChainGenerator::Construct()
    {
        Background = RGBA::Black;
        CenterLink = RGBA::White;
        ConnectingLink = RGBA::Gray;
        AddOutput("Color", TEXGRAPH_RGBA);
        AddOutput("Mask", TEXGRAPH_FLOAT);
    }

    int ChainGenerator::Execute(const Variant& param)
    {
        Vec2 pos = param.getVec2Safe();
        const Vec2 center(0.5f, 0.5f);
        const Vec2 upperCenter(0.5f, 0.0f);
        const Vec2 lowerCenter(0.5f, 1.0f);

        float radius = 0.5f;
        float thickness = 0.1f;
        float innerRadius = radius - thickness;
        RGBA color = Background;
        if (pos.x < 0.5f)
        {
            if (pos.y > 0.5f)
            {
                if (SprueBetween(center.Distance(pos), innerRadius, radius))
                    color = CenterLink;
                else if (SprueBetween(lowerCenter.Distance(pos), innerRadius, radius))
                    color = ConnectingLink;
            }
            else
            {
                if (SprueBetween(upperCenter.Distance(pos), innerRadius, radius))
                    color = ConnectingLink;
                else if (SprueBetween(center.Distance(pos), innerRadius, radius))
                    color = CenterLink;
            }
        }
        else
        {
            if (pos.y > 0.5f)
            {
                if (SprueBetween(lowerCenter.Distance(pos), innerRadius, radius))
                    color = ConnectingLink;
                else if (SprueBetween(center.Distance(pos), innerRadius, radius))
                    color = CenterLink;
            }
            else
            {
                if (SprueBetween(center.Distance(pos), innerRadius, radius))
                    color = CenterLink;
                else if (SprueBetween(upperCenter.Distance(pos), innerRadius, radius))
                    color = ConnectingLink;
            }
        }

        GetOutputSocket(0)->StoreValue(color);
        GetOutputSocket(1)->StoreValue(color.AverageRGB());
        return GRAPH_EXECUTE_COMPLETE;
    }

    void ChainMailGenerator::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "ChainMailGenerator");
        REGISTER_PROPERTY_MEMORY(ChainMailGenerator, Vec2, offsetof(ChainMailGenerator, ChainSize), Vec2(1, 1), "Repetitions", "Number of links in X and Y", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ChainMailGenerator, RGBA, offsetof(ChainMailGenerator, Background), RGBA::Black, "Background Color", "Color for areas not covered by the chain", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ChainMailGenerator, RGBA, offsetof(ChainMailGenerator, CenterLink), RGBA::White, "Center Link Color", "Color of the center link in the mail", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ChainMailGenerator, RGBA, offsetof(ChainMailGenerator, ConnectingLink), RGBA::Gray, "Connecting Links Color", "Color of the links that connect into the center link", PS_VisualConsequence | PS_Permutable);
    }

    void ChainMailGenerator::Construct()
    {
        Background = RGBA::Black;
        CenterLink = RGBA::White;
        ConnectingLink = RGBA::Gray;
        AddOutput("Color", TEXGRAPH_RGBA);
        AddOutput("Mask", TEXGRAPH_FLOAT);
    }

    float ChainMailGenerator::Remap(float a, float b, float* ra)
    {
        if (SprueBetween(ra[0], a, b)) { return ra[0]; }
        if (SprueBetween(ra[1], a, b)) { return ra[1]; }
        if (SprueBetween(ra[2], a, b)) { return ra[2]; }
        return -1;
    }

    float ChainMailGenerator::Pattern(float r1, float r2, float r3, int fx, int fy, float Rm, float Rp)
    {
        float x0y0[3] = { r3, r1, r2 };
        float x0y1[3] = { r2, r3, r1 };
        float x1y0[3] = { r1, r3, r2 };
        float x1y1[3] = { r2, r1, r3 };

        float r = -1;
        if (fx)
        {
            if (fy)
                r = Remap(Rm, Rp, &x1y1[0]);
            else
                r = Remap(Rm, Rp, &x1y0[0]);
        }
        else 
        {
            if (fy)
                r = Remap(Rm, Rp, &x0y1[0]);
            else
                r = Remap(Rm, Rp, &x0y0[0]);
        }
        return r;
    }

    int ChainMailGenerator::Execute(const Variant& param)
    {
        Vec2 p = param.getVec2Safe() * ChainSize;
        float x = fmodf(p[0], 1);
        float y = fmodf(p[1], 1);

        float Radius = 0.47f;
        float Width = 0.08f;

        float Fac = 0.0f;
        float Disp = 0.0f;

        float Rm = Radius - Width;
        float Rp = Radius + Width;

        float r = -1, r1, r2, r3, cr1, cr2, cr3;

        int fx = 0, fy = 0, flip = y > x, flipt = y > (1 - x);
        if (x > 0.5){ x = 1 - x; fx = 1; }
        if (y > 0.5){ y = 1 - y; fy = 1; }

        r1 = hypot(x - 0.5, y - 0.5);
        r2 = hypot(x - 0.5, y + 0.5);
        r3 = hypot(x + 0.5, y - 0.5);

        float xc = fmodf(p[0] + 0.5, 1);
        float yc = fmodf(p[1] + 0.5, 1);

        int fxc = 0, fyc = 0, flipc = y > x;

        if (xc > 0.5){ xc = 1 - xc; fxc = 1; }
        if (yc > 0.5){ yc = 1 - yc; fyc = 1; }

        cr1 = hypot(xc - 0.5, yc - 0.5);
        cr2 = hypot(xc - 0.5, yc + 0.5);
        cr3 = hypot(xc + 0.5, yc - 0.5);
        
        RGBA color = Background;

        if (flip ^ flipt){
            // base pattern
            r = Pattern(r1, r2, r3, fx, fy, Rm, Rp);
            if (r> -1){
                Fac = 1;
                color = CenterLink;
                Disp = ARC(r, Rm, Rp);
            }
            else {
                // connecting rings
                r = Pattern(cr1, cr2, cr3, fxc, fyc, Rm, Rp);
                if (r> -1){
                    Fac = 0.5f;
                    color = ConnectingLink;
                    Disp = Fac * ARC(r, Rm, Rp);
                }
            }
        }
        else {
            // connecting rings
            r = Pattern(cr1, cr2, cr3, fxc, fyc, Rm, Rp);
            if (r> -1){
                Fac = 0.5f;
                color = ConnectingLink;
                Disp = Fac * ARC(r, Rm, Rp);
            }
            else {
                // base patterm
                r = Pattern(r1, r2, r3, fx, fy, Rm, Rp);
                if (r> -1){
                    Fac = 1;
                    color = CenterLink;
                    Disp = ARC(r, Rm, Rp);
                }
            }
        }

        float val = Disp;
        GetOutputSocket(0)->StoreValue(color);
        GetOutputSocket(1)->StoreValue(Fac);
        return GRAPH_EXECUTE_COMPLETE;
    }

    void ScalesGenerator::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "ScalesGenerator");
        REGISTER_PROPERTY_MEMORY(ScalesGenerator, Vec2, offsetof(ScalesGenerator, ScaleSize), Vec2(1, 1), "Scale Count", "Dimensions of each scale", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ScalesGenerator, RGBA, offsetof(ScalesGenerator, EvenColor), RGBA::White, "Color A", "One of the two colors of the scales", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(ScalesGenerator, RGBA, offsetof(ScalesGenerator, OddColor), RGBA::Black, "Color B", "One of the two colors of the scales", PS_VisualConsequence | PS_Permutable);
    }

    void ScalesGenerator::Construct()
    {
        EvenColor = RGBA::White;
        OddColor = RGBA::Black;
        AddOutput("Color", TEXGRAPH_RGBA);
        AddOutput("Mask", TEXGRAPH_FLOAT);
    }

    int ScalesGenerator::Execute(const Variant& param)
    {
        const Vec2 coord = param.getVec2Safe();
#if 0
        float width = 0.05;
        float x = fmodf(coord[0], 1);
        float y = fmodf(coord[1], 1);
        float ox = x;
        float oy = y;
        x += width * (0.5 - oy);
        y -= width * (ox - 0.5);

        if (y > 0.5){
            y = 1 - y;
            x = 1 - x;
        }

        float value = 0.0f;
        float displ = 0.0f;
        if (x > 0.5){
            if (y > 0.5 - width){
                value = 1;
                displ = ARC((y - (0.5 - width)) / width);
            }
            else if (x < 0.5 + width) {
                value = 1;
                displ = ARC((x - 0.5) / width);
            }
            else{
                float r = hypot(x - 0.5, y - 0.5);
                if (r < width) {
                    value = 1;
                    displ = ARC(r / width);
                }
            }
        }
        GetOutputSocket(0)->StoreValue(RGBA(1.0f - displ, 1.0f - displ, 1.0f - displ));
        GetOutputSocket(1)->StoreValue(value);
#endif

        const float widthFraction = 1.0f / ScaleSize.y;
        const unsigned row = coord.y / widthFraction;
        const float rowStart = row * widthFraction;
        const float rowEnd = (row + 1) * widthFraction;
        const float rowHeight = rowEnd - rowStart;
        const float yInRow = NORMALIZE(coord.y, rowStart, rowEnd);

        const bool evenRow = row % 2;
        float dist = 0.0f;
        if (evenRow) // even row
            dist = fabsf(cosf(coord.x * ScaleSize.x * PI));
        else // odd row
            dist = fabsf(sinf(coord.x * ScaleSize.x * PI));
        
        if (dist > yInRow)
            GetOutputSocket(0)->StoreValue(evenRow ? EvenColor : OddColor);
        else
            GetOutputSocket(0)->StoreValue(evenRow ? OddColor : EvenColor);

        return GRAPH_EXECUTE_COMPLETE;
    }

    void OddBlocksGenerator::Register(Context* context)
    {
        context->CopyBaseProperties("GraphNode", "OddBlocksGenerator");
        REGISTER_PROPERTY_MEMORY(OddBlocksGenerator, float, offsetof(OddBlocksGenerator, LineWidth), 1.0f, "Gutter Width", "Width of the gutter between blocks", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(OddBlocksGenerator, float, offsetof(OddBlocksGenerator, Scale), 1.0f, "Scale", "Sizing of the blocks", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(OddBlocksGenerator, float, offsetof(OddBlocksGenerator, Offset), 1.0f, "Offset", "", PS_VisualConsequence | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(OddBlocksGenerator, int, FAST_NOISE_ADDR(OddBlocksGenerator, m_seed), 0, "Seed", "", PS_Default | PS_Permutable);
        REGISTER_PROPERTY_MEMORY(OddBlocksGenerator, float, FAST_NOISE_ADDR(OddBlocksGenerator, m_frequency), 0.05f, "Frequency", "", PS_Default | PS_Permutable);
    }

    void OddBlocksGenerator::Construct()
    {
        noise_.SetSeed(0);
        noise_.SetFrequency(0.05f);
        noise_.SetCellularDistanceFunction(FastNoise::CellularDistanceFunction::Euclidean);
        noise_.SetCellularReturnType(FastNoise::CellularReturnType::CellValue);
        noise_.SetInterp(SprueEngine::FastNoise::Interp::InterpLinear);
        noise_.SetNoiseType(FastNoise::NoiseType::Cellular);
        AddOutput("Mask", TEXGRAPH_FLOAT);
    }

    int OddBlocksGenerator::Execute(const Variant& param)
    {
        const Vec2 p = param.getVec2Safe();
        Vec4 coord = TextureNode::Make4D(p, Period);
        
        Vec2 Pos = p * Scale;
        float width = LineWidth * 0.1; // scale down so input values are easier to adjust

        float bot = floor(Pos[1] - 1) + NORMALIZE(noise_.GetCellular(Pos[1] - 1, 0.0f), -1, 1);
        float lev = floor(Pos[1]) + NORMALIZE(noise_.GetCellular(Pos[1], 0.0f), -1, 1);
        float top = floor(Pos[1] + 1) + NORMALIZE(noise_.GetCellular(Pos[1] + 1, 0.0f), -1, 1);

        if (Pos[1] < lev)
            Pos[0] += Offset * NORMALIZE(noise_.GetCellular(Pos[1], 0.0f), -1, 1);
        else
            Pos[0] += Offset * NORMALIZE(noise_.GetCellular(Pos[1] + 1, 0.0f), -1, 1);

        float left = floor(Pos[0] - 1) + NORMALIZE(noise_.GetCellular(Pos[0] - 1, 0.0f), -1, 1);
        float mid = floor(Pos[0]) + NORMALIZE(noise_.GetCellular(Pos[0], 0.0f), -1, 1);
        float right = floor(Pos[0] + 1) + NORMALIZE(noise_.GetCellular(Pos[0] + 1, 0.0f), -1, 1);

        float Fac = 0.0f;
        if (((Pos[0] > left + width) && (Pos[0] < mid - width)) || ((Pos[0] > mid + width) && (Pos[0] < right - width)))
        {
            if (((Pos[1] > bot + width) && (Pos[1] < lev - width)) || ((Pos[1] > lev + width) && (Pos[1] < top - width)))
            {
                int stoneindex = 0;
                float seeda = left;
                float seedb = bot;
                float bounda = mid;
                float boundb = lev;

                if (Pos[0] > mid)
                { 
                    stoneindex += 2; 
                    seeda = mid; 
                    bounda = right; 
                }
                if (Pos[1] > lev)
                { 
                    stoneindex += 1; 
                    seedb = lev; 
                    boundb = top; 
                }

                int pattern = (int)floor(NORMALIZE(noise_.GetCellular(seeda, seedb), -1, 1) * 4);
                if (pattern == 0){
                    // horizontally halved
                    float nlev = (seedb + boundb) / 2;
                    if ((Pos[1] > nlev - width) && (Pos[1] < nlev + width))
                    {
                        Fac = 0;
                    }
                    else 
                    {
                        Fac = NORMALIZE(noise_.GetCellular(seeda, seedb, Pos[1]>nlev), -1, 1);
                    }
                }
                else {
                    Fac = NORMALIZE(noise_.GetCellular(seeda, seedb, -1), -1, 1);
                }
            }
        }

        GetOutputSocket(0)->StoreValue(Fac);
        return GRAPH_EXECUTE_COMPLETE;
    }
}