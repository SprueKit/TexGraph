#pragma once

#include <SprueEngine/Compute/ComputeDevice.h>
#include <SprueEngine/Compute/ComputeShader.h>
#include <SprueEngine/Core/SceneObject.h>
#include <SprueEngine/Core/SprueModel.h>
#include <SprueEngine/Core/SpruePieces.h>

#include <string>

namespace SprueEngine
{
    class SprueModel;

    /// Baseclass for a collection of classes that generate compute shader code for calculating the density
    /// of parametric shapes.
    class SPRUE DensityShaderBuilder : public TypeLimitedSceneObjectVisitor<SpruePiece>
    {
    public:
        virtual std::string BuildShader(SprueModel* model) = 0;
    };

}