#pragma once

#include <SprueEngine/Compute/DensityShaderBuilder.h>
#include <SprueEngine/Core/SceneObject.h>
#include <SprueEngine/Core/SpruePieces.h>
#include <SprueEngine/VectorBuffer.h>

namespace SprueEngine
{

    class SPRUE OpenCLDensityShaderBuilder : public DensityShaderBuilder
    {
    public:
        OpenCLDensityShaderBuilder(bool writeCode, bool writeData);

        virtual std::string BuildShader(SprueModel* model) override;
        virtual bool ShouldNotVisitChildren(SceneObject* object) override;
        virtual bool Visit(SpruePiece* child) override;
        void Finish();

        bool writeCode_;
        bool writeData_;
        std::string code_;
        VectorBuffer buffer_;
        VectorBuffer transforms_;
    };

}