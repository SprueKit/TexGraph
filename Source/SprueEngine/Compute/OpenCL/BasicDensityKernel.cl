// Fill with a procedurally constructed density field


int field_index(const float3 pos)
{ 
    int x = clamp(pos.x, 0, 65 - 1);
    int y = clamp(pos.y, 0, 65 - 1);
    int z = clamp(pos.z, 0, 65 - 1);
    return (z * 65 * 65 + y * 65 + x);
}


kernel void GenerateDefaultField(
    const int4 offset,
    const int defaultMaterialIndex,
    global float* densities,
    global const float* transforms,
    global const float* shapeData)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int z = get_global_id(2);
    
    const float4 world_pos = { x + offset.x, y + offset.y, z + offset.z, 0 };
    const float density = DensityFunc(world_pos, shapeData, transforms);
    
    const int4 local_pos = { x, y, z, 0 };
    const int index = field_index(local_pos);
    
    densities[index] = density;
}