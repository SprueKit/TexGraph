#pragma once

#include <SprueEngine/FString.h>
#include <SprueEngine/MathGeoLib/AllMath.h>
#include <SprueEngine/Variant.h>

#include <sstream>
#include <string>

namespace SprueEngine
{

    static const struct ConversionMappingFunction {
        int lhs; int rhs; const char* expression;
    };
    
    ConversionMappingFunction VectorValueConversionTable[][16] = {
        // GLSL
        {
            { VT_Float, VT_Float, "%1" },
            { VT_Float, VT_Vec2, "%1.x" },
            { VT_Float, VT_Vec3, "%1.x" },
            { VT_Float, VT_Vec4, "%1.x" },
        
            { VT_Vec2, VT_Float, "vec2(%1, %1)" },
            { VT_Vec2, VT_Vec2, "%1" },
            { VT_Vec2, VT_Vec3, "%1.xy" },
            { VT_Vec2, VT_Vec4, "%1.xy" },

            { VT_Vec3, VT_Float, "vec3(%1, %1, %1)" },
            { VT_Vec3, VT_Vec2,  "vec3(%1.x, %1.y, 1)" },
            { VT_Vec3, VT_Vec3,  "%1" },
            { VT_Vec3, VT_Vec4,  "%1.xyz" },

            { VT_Vec4, VT_Float, "vec4(%1, %1, %1, %1)" },
            { VT_Vec4, VT_Vec2,  "vec4(%1.x, %1.y, 1, 1)" },
            { VT_Vec4, VT_Vec3,  "vec4(%1.x, %1.y, %1.z, 1)" },
            { VT_Vec4, VT_Vec4,  "%1" },
        },
        // HLSL/CG
        {
            { VT_Float, VT_Float, "%1" },
            { VT_Float, VT_Vec2, "%1.x" },
            { VT_Float, VT_Vec3, "%1.x" },
            { VT_Float, VT_Vec4, "%1.x" },

            { VT_Vec2, VT_Float, "float2(%1, %1)" },
            { VT_Vec2, VT_Vec2, "%1" },
            { VT_Vec2, VT_Vec3, "%1.xy" },
            { VT_Vec2, VT_Vec4, "%1.xy" },

            { VT_Vec3, VT_Float, "float3(%1, %1, %1)" },
            { VT_Vec3, VT_Vec2,  "float3(%1.x, %1.y, 1)" },
            { VT_Vec3, VT_Vec3,  "%1" },
            { VT_Vec3, VT_Vec4,  "%1.xyz" },

            { VT_Vec4, VT_Float, "float4(%1, %1, %1, %1)" },
            { VT_Vec4, VT_Vec2,  "float4(%1.x, %1.y, 1, 1)" },
            { VT_Vec4, VT_Vec3,  "float4(%1.x, %1.y, %1.z, 1)" },
            { VT_Vec4, VT_Vec4,  "%1" },
        },
        // OpenCL
        {
            { VT_Float, VT_Float, "%1" },
            { VT_Float, VT_Vec2,  "%1.x" },
            { VT_Float, VT_Vec3,  "%1.x" },
            { VT_Float, VT_Vec4,  "%1.x" },

            { VT_Vec2, VT_Float, "vec2(%1, %1)" },
            { VT_Vec2, VT_Vec2,  "%1" },
            { VT_Vec2, VT_Vec3,  "%1.xy" },
            { VT_Vec2, VT_Vec4,  "%1.xy" },

            { VT_Vec3, VT_Float, "vec3(%1, %1, %1)" },
            { VT_Vec3, VT_Vec2,  "vec3(%1.x, %1.y, 1)" },
            { VT_Vec3, VT_Vec3,  "%1" },
            { VT_Vec3, VT_Vec4,  "%1.xyz" },

            { VT_Vec4, VT_Float, "vec4(%1, %1, %1, %1)" },
            { VT_Vec4, VT_Vec2,  "vec4(%1.x, %1.y, 1, 1)" },
            { VT_Vec4, VT_Vec3,  "vec4(%1.x, %1.y, %1.z, 1)" },
            { VT_Vec4, VT_Vec4,  "%1" },
        },
        // C++
        {
            { VT_Float, VT_Float, "%1" },
            { VT_Float, VT_Vec2, "%1.x" },
            { VT_Float, VT_Vec3, "%1.x" },
            { VT_Float, VT_Vec4, "%1.x" },

            { VT_Vec2, VT_Float, "Vec2(%1, %1)" },
            { VT_Vec2, VT_Vec2, "%1" },
            { VT_Vec2, VT_Vec3, "%1.xy()" },
            { VT_Vec2, VT_Vec4, "%1.xy()" },

            { VT_Vec3, VT_Float, "Vec3(%1, %1, %1)" },
            { VT_Vec3, VT_Vec2, "Vec3(%1.x, %1.y, 1)" },
            { VT_Vec3, VT_Vec3, "%1" },
            { VT_Vec3, VT_Vec4, "%1.xyz" },

            { VT_Vec4, VT_Float, "Vec4(%1, %1, %1, %1)" },
            { VT_Vec4, VT_Vec2, "Vec4(%1.x, %1.y, 1, 1)" },
            { VT_Vec4, VT_Vec3, "Vec4(%1.x, %1.y, %1.z, 1)" },
            { VT_Vec4, VT_Vec4, "%1" },
        },
    };

    // Table for taking a constant and turning it into a vector
    ConversionMappingFunction VectorConstantConversionTable[][16] = {
        // GLSL
        { 
            { VT_Float, VT_Float, "%1" },
            { VT_Float, VT_Vec2,  "%1" },
            { VT_Float, VT_Vec3,  "%1" },
            { VT_Float, VT_Vec4,  "%1" },

            { VT_Vec2, VT_Float, "vec2(%1, %1)" },
            { VT_Vec2, VT_Vec2,  "vec2(%1, %2)" },
            { VT_Vec2, VT_Vec3,  "vec2(%1, %2)" },
            { VT_Vec2, VT_Vec4,  "vec2(%1, %2)" },

            { VT_Vec3, VT_Float, "vec3(%1, %1, %1)" },
            { VT_Vec3, VT_Vec2,  "vec3(%1, %2, 1)" },
            { VT_Vec3, VT_Vec3,  "vec3(%1, %2, %3)" },
            { VT_Vec3, VT_Vec4,  "vec3(%1, %2, %3)" },

            { VT_Vec4, VT_Float, "vec4(%1, %1, %1)" },
            { VT_Vec4, VT_Vec2,  "vec4(%1, %2, 1, 1)" },
            { VT_Vec4, VT_Vec3,  "vec4(%1, %2, %3, 1)" },
            { VT_Vec4, VT_Vec4,  "vec4(%1, %2, %3, %4)" },
        },
        // HLSL/CG
        {
            { VT_Float, VT_Float, "%1" },
            { VT_Float, VT_Vec2, "%1" },
            { VT_Float, VT_Vec3, "%1" },
            { VT_Float, VT_Vec4, "%1" },

            { VT_Vec2, VT_Float, "float2(%1, %1)" },
            { VT_Vec2, VT_Vec2,  "float2(%1, %2)" },
            { VT_Vec2, VT_Vec3,  "float2(%1, %2)" },
            { VT_Vec2, VT_Vec4,  "float2(%1, %2)" },

            { VT_Vec3, VT_Float, "float3(%1, %1, %1)" },
            { VT_Vec3, VT_Vec2,  "float3(%1, %2, 1)" },
            { VT_Vec3, VT_Vec3,  "float3(%1, %2, %3)" },
            { VT_Vec3, VT_Vec4,  "float3(%1, %2, %3)" },

            { VT_Vec4, VT_Float, "float4(%1, %1, %1)" },
            { VT_Vec4, VT_Vec2,  "float4(%1, %2, 1, 1)" },
            { VT_Vec4, VT_Vec3,  "float4(%1, %2, %3, 1)" },
            { VT_Vec4, VT_Vec4,  "float4(%1, %2, %3, %4)" },
        },
        // OpenCL
        {
            { VT_Float, VT_Float, "%1" },
            { VT_Float, VT_Vec2, "%1" },
            { VT_Float, VT_Vec3, "%1" },
            { VT_Float, VT_Vec4, "%1" },

            { VT_Vec2, VT_Float, "vec2(%1, %1)" },
            { VT_Vec2, VT_Vec2, "vec2(%1, %2)" },
            { VT_Vec2, VT_Vec3, "vec2(%1, %2)" },
            { VT_Vec2, VT_Vec4, "vec2(%1, %2)" },

            { VT_Vec3, VT_Float, "vec3(%1, %1, %1)" },
            { VT_Vec3, VT_Vec2, "vec3(%1, %2, 1)" },
            { VT_Vec3, VT_Vec3, "vec3(%1, %2, %3)" },
            { VT_Vec3, VT_Vec4, "vec3(%1, %2, %3)" },

            { VT_Vec4, VT_Float, "vec4(%1, %1, %1)" },
            { VT_Vec4, VT_Vec2, "vec4(%1, %2, 1, 1)" },
            { VT_Vec4, VT_Vec3, "vec4(%1, %2, %3, 1)" },
            { VT_Vec4, VT_Vec4, "vec4(%1, %2, %3, %4)" },
        },
        // C++
        {
            { VT_Float, VT_Float, "%1" },
            { VT_Float, VT_Vec2, "%1" },
            { VT_Float, VT_Vec3, "%1" },
            { VT_Float, VT_Vec4, "%1" },

            { VT_Vec2, VT_Float, "Vec2(%1, %1)" },
            { VT_Vec2, VT_Vec2, "Vec2(%1, %2)" },
            { VT_Vec2, VT_Vec3, "Vec2(%1, %2)" },
            { VT_Vec2, VT_Vec4, "Vec2(%1, %2)" },

            { VT_Vec3, VT_Float, "Vec3(%1, %1, %1)" },
            { VT_Vec3, VT_Vec2, "Vec3(%1, %2, 1)" },
            { VT_Vec3, VT_Vec3, "Vec3(%1, %2, %3)" },
            { VT_Vec3, VT_Vec4, "Vec3(%1, %2, %3)" },

            { VT_Vec4, VT_Float, "Vec4(%1, %1, %1)" },
            { VT_Vec4, VT_Vec2, "Vec4(%1, %2, 1, 1)" },
            { VT_Vec4, VT_Vec3, "Vec4(%1, %2, %3, 1)" },
            { VT_Vec4, VT_Vec4, "Vec4(%1, %2, %3, %4)" },
        },
    };

    int VectorElementCount(VariantType type)
    {
        if (type == VT_Float) return 1;
        if (type == VT_Vec2) return 2;
        if (type == VT_Vec3) return 3;
        if (type == VT_Vec4) return 4;
        if (type == VT_Int) return 1;
        if (type == VT_IntVec2) return 2;
        return 0;
    }

    std::string VectorElementBase(VariantType type)
    {
        if (type == VT_Vec2) return "vec";
        if (type == VT_Vec3) return "vec";
        if (type == VT_Vec4) return "vec";
        if (type == VT_IntVec2) return "int";
        return std::string();
    }

    std::string VectorConversion(VariantType lhs, VariantType rhs)
    {
        int lhsCt = VectorElementCount(lhs);
        int rhsCt = VectorElementCount(rhs);

        std::string lhsBase = VectorElementBase(lhs);

        switch (lhsCt)
        {
        case 1:
        {
            switch (rhsCt)
            {
            case 1:
                return "%1";
            case 2:
            case 3:
            case 4:
                return "%1.x";
            }
        }
        case 2:
        {
            switch (rhsCt)
            {
            case 1:
                return FString("%1(%%1, %%1)", lhsBase);
            case 2:
                return "%1";
            case 3:
            case 4:
                return "%1.xy";
            }
        }
        case 3:
        {
            switch (rhsCt)
            {
            case 1:
                return FString("%1(%%1, %%1, %%1)", lhsBase);
            case 2:
                return FString("%1(%%1.x, %%1.y, 1)", lhsBase);
            case 3:
                return "%1";
            case 4:
                return "%1.xyz";
            }
        }
        case 4:
        {
            switch (rhsCt)
            {
            case 1:
                return FString("%1(%%1, %%1, %%1, %%1)", lhsBase);
            case 2:
                return FString("%1(%%1.x, %%1.y, 1, 1)", lhsBase);
            case 3:
                return FString("%1(%%1.x, %%1.y, %%1.z, 1)", lhsBase);
            case 4:
                return "%1";
            }
        }
        }
        return std::string();
    }

    std::string ConvertVectorOp(VariantType lhs, VariantType rhs, char op)
    {
        int lhsCt = VectorElementCount(lhs);
        int rhsCt = VectorElementCount(rhs);

        // Easy case
        if (lhsCt == rhsCt)
            return FString("%%1 %1 %%2", op);
        return FString("%%1 %1 %2", op, VectorConversion(lhs, rhs));
    }

    std::string ConvertVectorCallOp(VariantType lhs, VariantType rhs, const char* func)
    {
        int lhsCt = VectorElementCount(lhs);
        int rhsCt = VectorElementCount(rhs);

        // Easy case
        if (lhsCt == rhsCt)
            return FString("%1(%%1, %%2)", func);
        return FString("%1(%%1, %2", func, VectorConversion(lhs, rhs));
    }

    std::string ConvertVectorCallOp(VariantType inputType, const char* func)
    {
        // This would look a lot different for C++ code
        return FString("%1(%%1)", func);
    }
}