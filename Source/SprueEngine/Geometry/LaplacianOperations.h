#pragma once

#include <SprueEngine/ClassDef.h>
#include <SprueEngine/MathGeoLib/AllMath.h>

#include <vector>

namespace SprueEngine
{
    class MeshData;
    class Skeleton;

    class LaplacianOperations
    {
    public:
        /// Associates each point to the nearest vertex in order to find the appropriate handles for laplacian deformation. First pt is original position, and the second point is the deformed position.
        static std::vector< std::pair<unsigned, Vec3> > GetClosestHandles(MeshData* meshData, const std::vector< std::pair<Vec3, Vec3> >& points);

        static void Deform(MeshData* meshData, const std::vector< std::pair<unsigned, Vec3> >& handles, float strength = 1.0f, bool useCotangentWeights = false);

        static void CalculateBoneWeights(MeshData* meshData, Skeleton* skeleton);
        static void CalculateBoneWeightsV2(MeshData* meshData, Skeleton* skeleton);
    };

}