#pragma once

#include <SprueEngine/MathGeoLib/Geometry/AABB.h>
#include <SprueEngine/Math/BoneWeights.h>
#include <SprueEngine/MathGeoLib/Geometry/Circle.h>
#include <SprueEngine/MathGeoLib/Math/float2.h>
#include <SprueEngine/MathGeoLib/Math/float3.h>
#include <SprueEngine/MathGeoLib/Math/float3x3.h>
#include <SprueEngine/MathGeoLib/Math/float3x4.h>
#include <SprueEngine/MathGeoLib/Math/float4x4.h>
#include <SprueEngine/MathGeoLib/Geometry/OBB.h>
#include <SprueEngine/MathGeoLib/Geometry/Plane.h>
#include <SprueEngine/MathGeoLib/Geometry/Ray.h>
#include <SprueEngine/MathGeoLib/Geometry/Sphere.h>
#include <SprueEngine/MathGeoLib/Geometry/Triangle.h>
#include <SprueEngine/MathGeoLib/Math/Quat.h>

namespace SprueEngine
{
    typedef math::float2 Vec2;
    typedef math::float3 Vec3;
    typedef math::float4 Vec4;
    typedef math::float3x3 Mat3x3;
    typedef math::float3x4 Mat3x4;
    typedef math::float4x4 Mat4x4;
    typedef math::Quat Quat;
    typedef math::AABB BoundingBox;
    typedef math::Ray Ray;
    typedef math::Sphere Sphere;
    typedef math::Circle Disc;
    typedef math::Plane Plane;
    typedef math::OBB OrientedBoundingBox;
}