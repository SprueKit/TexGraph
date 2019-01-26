// TODO: Add OpenCL kernel code here.

#define SprueMin(a, b) (((a) < (b)) ? (a) : (b))
#define SprueMax(a, b) (((a) > (b)) ? (a) : (b))
#define MaxElem(a) (max(a.x, max(a.y, a.z)))

float length2(float3 a)
{ 
    return a.x * a.x + a.y * a.y + a.z * a.z;
}


float3 ClosestPoint(float3 a, float3 b, float3 point)
{ 
    const float3 dir = b - a;
    return a + clamp(dot(point - a, dir) / length2(dir), 0.0f, 1.0f) * dir;
}

float SphereDensity(const float3& pos, const float* data, const float* transforms, int& paramIndex, int& shapeIndex)
{ 
    const float radius = data[paramIndex];
    ++paramIndex;
    
    // TODO
    return length(pos) - radius;
}

float BoxDensity(const float3 pos, const float* data, const float* transforms, int& paramIndex, int& shapeIndex)
{ 
    const float xDim = data_[paramIndex++];
    const float yDim = data_[paramIndex++];
    const float zDim = data_[paramIndex++];

    float3 d = abs(pos) - float3(xDim, yDim, zDim);
    return SprueMin(MaxElem(d), 0.0f) + length(max(d, float3(0,0,0)));
}

float EllipsoidDensity(const float3 p, const float* data, const float* transforms, int& paramIndex, int& shapeIndex)
{ 
    const float paramX = data[paramIndex++];
    const float paramY = data[paramIndex++];
    const float paramZ = data[paramIndex++];
    return pow(p.x / paramX, 2.0f) + pow(p.y / paramY, 2.0f) + pow(p.z / paramZ, 2.0f) - 1.0f;
}

float CylinderDensity(const float3 p, const float* data, const float* transforms, int& paramIndex, int& shapeIndex)
{ 
    const float radius = data[paramIndex++];
    const float height = data[paramIndex++];

    float d = length(Vec2(p.x, p.z)) - radius;
    return max(d, abs(p.y) - height);
}

float CapsuleDensity(const float3 p, const float* data, const float* transforms, int& paramIndex, int& shapeIndex)
{ 
    float3 a(0.0f, 0.0f, data[paramIndex++] / 2.0f);
    float3 b(0.0f, 0.0f, -a.z);
    float3 nearest = ClosestPoint(a, b, pos);
    return length(pos - nearest) - data[paramIndex++];
}

float PlaneDensity(const float3 p, const float* data, const float* transforms, int& paramindex, int& shapeIndex)
{
    return dot(p, float3(data[paramIndex++], data[paramIndex++], data[paramIndex++])) + data[paramIndex++];
}

float TorusDensity(const float3 p, const float* data, const float* transforms, int& paramindex, int& shapeIndex)
{ 
    float2 q = float2(length(float2(p.x, p.z)) - data[paramindex++], p.y);
    return length(q) - data[paramindex++];
}

float CSGAdd(float lhs, float rhs)
{ 
    return min(lhs, rhs);
}

float CSGSubtract(float lhs, float rhs)
{ 
    return max(lhs, -rhs);
}

float CSGIntersect(float lhs, float rhs)
{ 
    return max(lhs, rhs);
}
