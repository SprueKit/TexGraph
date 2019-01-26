#pragma once

#include <stdint.h>

struct PluginMeshData
{
    /// 3 x vertex-count, MANDATORY
    float* vertexData_;
    /// 3 x vertex-count, null-checked to determine existence
    float* vertexNormals_;
    /// 3 x vertex-count, null-checked to determine existence
    float * vertexTangents_;
    /// 3 x vertex-count, null-checked to determine existence
    float* vertexBinormals_;
    /// 2 x vertex-count, null-checked to determine existence
    float* uvData_;
    /// 4 x vertex-count, null-checked to determine existence
    float* vertexColors_;
    /// 4 x vertex-count, null-checked to determine existence
    float* vertexBoneWeights_;
    /// 4 x vertex-count, null-checked to determine existence
    unsigned char* vertexBoneIndices_;
    /// Index data.
    uint32_t* indices_;
    /// Number of vertices.
    uint32_t vertexCount_;
    /// Number of indices.
    uint32_t indexCount_;
};

struct BoneData
{
    const char* name_;
    unsigned char parent_;
    float position_[3];
    float rotation_[4];
    float scale_[3];
};

struct PluginModelData
{
    PluginMeshData* meshes_;
    uint32_t meshCount_;
    BoneData* bones_;
    uint32_t boneCount_;
    PluginMeshData* morphTargets_;
    const char** meshNames_;
    const char** morphTargetNames_;
};