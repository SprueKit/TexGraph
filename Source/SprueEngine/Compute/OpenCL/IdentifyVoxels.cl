
#define EDGE_CT 64

int field_index(const int3 pos)
{ 
    int x = clamp(pos.x, 0, EDGE_CT - 1);
    int y = clamp(pos.y, 0, EDGE_CT - 1);
    int z = clamp(pos.z, 0, EDGE_CT - 1);
    return (z * EDGE_CT * EDGE_CT + y * EDGE_CT + x);
}

#define DENSITY_EDGE_CT 65

int density_index(const int3 pos)
{
    int x = clamp(pos.x, 0, DENSITY_EDGE_CT - 1);
    int y = clamp(pos.y, 0, DENSITY_EDGE_CT - 1);
    int z = clamp(pos.z, 0, DENSITY_EDGE_CT - 1);
    return (z * DENSITY_EDGE_CT * DENSITY_EDGE_CT + y * DENSITY_EDGE_CT + x);
}

// Compute the the states of each leaf voxel
kernel void ScanVoxels(
    global const char* densities,
    global int* voxelIndices)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int z = get_global_id(2);
    
    const int3 CHILD_OFFSETS[8] = {
        { 0, 0, 0},
        { 0, 0, 1},
        { 0, 1, 0},
        { 0, 1, 1},
        { 1, 0, 0},
        { 1, 0, 1},
        { 1, 1, 0},
        { 1, 1, 1}
    };
    
    int3 pos = {x, y, z};
    int corners = 0;
    const int one = 1;
    for (int i = 0; i < 8; ++i)
    {
        const int fieldIdx = density_index(pos + CHILD_OFFSETS[i]);
        if (densities[fieldIdx] < 0)
            corners = corners | (one << i);
    }
    
    voxelIndices[field_index(pos)] = corners;
}