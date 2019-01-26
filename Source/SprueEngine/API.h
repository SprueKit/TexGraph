#pragma once

#define API_FUNC __declspec(dllexport)

extern "C"
{

    struct API_FUNC MorphTargetInterop
    {
        const char* Name;
        float* Positions;
        float* Normals;
        float* Tangents;
        int* TargetIndices;
        int IndexCount;
    };

    struct API_FUNC GeometryInterop
    {
        const char* Name;
        float* Positions;
        float* Normals;
        float* Tangents;
        float* UV;
        int* BoneIndices;
        float* BoneWeights;
        int* indices;
        int VertexCount;
        int IndexCount;
        
        MorphTargetInterop* MorphTargets;
        int MorphCount;
    };

    struct API_FUNC ModelInterop
    {
        ModelInterop* Geometries;
        int GeoCount;
    };
    
    API_FUNC void SprueEngine_Init();
    API_FUNC void SprueEngine_StoreGeometry(const char* name, ModelInterop* data);

    API_FUNC void SprueEngine_DeleteModelData(ModelInterop* data);

   
    // Contouring API
    API_FUNC void SprueEngine_ContourNaiveSurfaceNets();
    API_FUNC void SprueEngine_ContourDC();
    
    // Processing API
    API_FUNC ModelInterop* SprueEngine_MergeCSGMeshes(ModelInterop* models, int* opCodes, int modelCount);
    API_FUNC void SprueEngine_GenerateUVCoordinates(ModelInterop* model, float gutter, float stretch, int quality);
    API_FUNC void SprueEngine_GenerateBoneWeights(ModelInterop* model);
    
    // Filtering API
    API_FUNC void SprueEngine_Smooth(ModelInterop* model, float power);
    API_FUNC void SprueEngine_Tesselate(ModelInterop* model, int levels);

}
