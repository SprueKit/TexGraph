#include "PatternGen.h"

#include <SprueEngine/Core/Context.h>

namespace SprueEngine
{

// greatest common divisor
int gcd(int A, int B){
    int a = A, b = B;
    if (a == 0) { return b; }
    while (b != 0) {
        if (a > b) {
            a = a - b;
        }
        else {
            b = b - a;
        }
    }
    return a;
}

// smallest common multiple (assumes a, b > 0 )
int scm(int a, int b){ return a*b / gcd(a, b); }

void WeaveGenerator::Register(Context* context)
{
    context->CopyBaseProperties("GraphNode", "WeaveGenerator");
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, int, offsetof(WeaveGenerator, Overrun), 1, "Overrun", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, int, offsetof(WeaveGenerator, Underrun), 1, "Underrun", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, int, offsetof(WeaveGenerator, Skip), 1, "Skip", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, float, offsetof(WeaveGenerator, WarpWidth), 0.8f, "Warp Width", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, float, offsetof(WeaveGenerator, WeftWidth), 0.8f, "Weft Width", "", PS_TinyIncrement | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, RGBA, offsetof(WeaveGenerator, WarpColor), RGBA(1,1,1), "Warp Color", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, RGBA, offsetof(WeaveGenerator, WeftColor), RGBA(0,0,0), "Weft Color", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, RGBA, offsetof(WeaveGenerator, BaseColor), RGBA(0, 0, 0), "Background", "", PS_Default | PS_Permutable);
    REGISTER_PROPERTY_MEMORY(WeaveGenerator, IntVec2, offsetof(WeaveGenerator, Tiling), IntVec2(4, 4), "Tiling", "", PS_Default | PS_Permutable);
}

void WeaveGenerator::Construct()
{
    AddOutput("Color", TEXGRAPH_RGBA);
}

int WeaveGenerator::Execute(const Variant& param)
{
    Vec2 pos = param.getVec2Safe();
    int ny = Underrun + Overrun;
    int nx = scm(Skip, ny);

    float fx = fmodf(pos.x * Tiling.x, 1.0f);
    float fy = fmodf(pos.y * Tiling.y, 1.0f);

    int ix = int(floor(fx * nx));
    int iy = int(floor(fy * ny));

    float cx = fmodf(fx * nx, 1.0f);
    float cy = fmodf(fy * ny, 1.0f);

    int top = ((iy + Skip*ix) % ny) < Overrun;

    float lx = (1.0f - WarpWidth) / 2.0f;
    float hx = 1.0f - lx;
    float ly = (1.0f - WeftWidth) / 2.0f;
    float hy = 1.0f - lx;

    float dist = 0.0f;
    RGBA color = BaseColor;

    if (top) {
        if (cx > lx && cx < hx)
        {
            color = WarpColor;
            dist = fabsf(0.5f - cx);
        }
        else if (cy > ly && cy < hy)
        {
            color = WeftColor;
            dist = fabsf(0.5f - cy);
        }
    }
    else 
    {
        if (cy > ly && cy < hy)
        {
            color = WeftColor;
            dist = fabsf(0.5f - cy);
        }
        else if (cx > lx && cx < hx)
        {
            color = WarpColor;
            dist = fabsf(0.5f - cx);
        }
    }

    GetOutputSocket(0)->StoreValue(color);
    return GRAPH_EXECUTE_COMPLETE;
}

}