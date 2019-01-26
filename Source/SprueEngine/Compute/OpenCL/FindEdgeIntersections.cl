
#define EDGE_SIZE 65

int field_index(const float3 pos)
{ 
    int x = clamp(pos.x, 0.0f, (float)EDGE_SIZE - 1);
    int y = clamp(pos.y, 0.0f, (float)EDGE_SIZE - 1);
    int z = clamp(pos.z, 0.0f, (float)EDGE_SIZE - 1);
    return (z * EDGE_SIZE * EDGE_SIZE + y * EDGE_SIZE + x);
}

struct EdgeData
{
    float3 normal;
    float distance;
};

#define FIND_EDGE_INFO_INCREMENT 1.0f/8
#define FIND_EDGE_INFO_STEPS 8

float3 ApproximateZeroCross(const float3 lhsPos, const float3 rhsPos, float* tHolder, const global float* paramData, const global float* transformData)
{
    float minValue = FLT_MAX;
    float currentT = 0.f;
    float t = 0.f;
    for (int i = 0; i <= FIND_EDGE_INFO_STEPS; i++)
    {
        const float3 p = mix(lhsPos, rhsPos, currentT);
        const float d = fabs(DensityFunc(p, paramData, transformData));
        if (d < minValue)
        {
            t = currentT;
            minValue = d;
        }
        currentT += FIND_EDGE_INFO_INCREMENT;
    }
    *tHolder = t;
    return mix(lhsPos, rhsPos, t);
}

kernel void FindEdgeIntersections(
    global char* density,
    global const float* paramData,
    global const float* transformData,
    //global char* edges,
    global struct EdgeData* edgeValues)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int z = get_global_id(2);

    // Identify our cell
    const float3 pos = { x, y, z };
    const int index = field_index(pos);
    const int edgeIndex = index * 3;
    
    const float3 EDGE_POINTS[4] = {
        {0,0,0},
        {1,0,0},
        {0,1,0},
        {0,0,1}
    };

    // Calculate min corner density
    const float lhs = DensityFunc(pos + EDGE_POINTS[0], paramData, transformData);
    
    // For the 3 axis edges find the crossing point and normal
    for (int i = 1; i < 4; ++i)
    {
        const float3 thisPos = pos + EDGE_POINTS[i];
        const float rhs = DensityFunc(thisPos, paramData, transformData);
        float distance = FLT_MAX;
        const float3 crossingPosition = ApproximateZeroCross(pos, thisPos, &distance, paramdata, transformData);
        
        const float h = 0.001f;
        const float3 xOffset = { h, 0, 0 };
        const float3 yOffset = { 0, h, 0 };
        const float3 zOffset = { 0, 0, h };

        const float dx = DensityFunc(crossingPosition + xOffset, paramData, transformData) - DensityFunc(crossingPosition - xOffset, paramData, transformData);
        const float dy = DensityFunc(crossingPosition + yOffset, paramData, transformData) - DensityFunc(crossingPosition - yOffset, paramData, transformData);
        const float dz = DensityFunc(crossingPosition + zOffset, paramData, transformData) - DensityFunc(crossingPosition - zOffset, paramData, transformData);

        const float3 normal = normalize((float3)(dx, dy, dz));
        
        edges[edgeIndex + i] = (sign(lhs) != sign(sign(rhs))) ? 1 : 0;
        edgeValues[edgeIndex + i - 1].distance = distance;
        edgeValues[edgeIndex + i - 1].normal = normal;
    }
    
    density[index] = lhs < 0 ? -1 : 1; //store our density for voxel detection
}